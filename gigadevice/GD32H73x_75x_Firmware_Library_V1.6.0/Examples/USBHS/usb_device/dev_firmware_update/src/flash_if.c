/*!
    \file    flash_if.c
    \brief   USB DFU device flash interface functions

    \version 2026-03-24, V1.6.0, firmware for GD32H73x_75x
*/

/*
    Copyright (c) 2026, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "flash_if.h"
#include "bss.h"

__IO uint8_t vfimg_set = 0;
__IO uint32_t certificate_size = 0;

/* local function prototypes ('static') */
static uint8_t flash_if_init(void);
static uint8_t flash_if_deinit(void);
static uint8_t flash_if_erase(uint32_t addr);
static uint8_t flash_if_write(uint8_t *buf, uint32_t addr, uint32_t len);
static uint8_t* flash_if_read(uint8_t *buf, uint32_t addr, uint32_t len);
static uint8_t flash_if_checkaddr(uint32_t addr);

static fmc_state_enum fmc_ready_wait(uint32_t timeout);

dfu_mem_prop dfu_flash_cb =
{
    (const uint8_t *)FLASH_IF_STRING,

    flash_if_init,
    flash_if_deinit,
    flash_if_erase,
    flash_if_write,
    flash_if_read,
    flash_if_checkaddr,
    60U, /* flash erase timeout in ms */
    80U  /* flash programming timeout in ms (80us * RAM Buffer size (1024 Bytes) */
};

/*!
    \brief      write the option byte
    \param[in]  Mem_Add: memory address
    \param[in]  data: pointer to the data
    \param[out] none
    \retval     state of FMC, refer to fmc_state_enum
*/
fmc_state_enum option_byte_write(uint32_t mem_add, uint8_t* data)
{
    fmc_state_enum status ;

    /* clear pending flags */
    fmc_flag_clear(FMC_FLAG_PGSERR);
    fmc_flag_clear(FMC_FLAG_WPERR);
    fmc_flag_clear(FMC_FLAG_END);

    status = fmc_ready_wait(FMC_TIMEOUT_COUNT);

    if(FMC_READY != status) {
        return status;
    }

    uint32_t write_data = (uint32_t)data[0] | 
                          (uint32_t)(data[1] << 8) | 
                          (uint32_t)(data[2] << 16) | 
                          (uint32_t)(data[3] << 24);

    *(__IO uint32_t*)mem_add = write_data;

    return status;
}


void ob_write_all(uint8_t *data)
{
    uint32_t ob_addr = FMC_OB_START + 4;
    uint8_t *ob_data_addr = data + 4;

    option_byte_write(ob_addr, ob_data_addr);
    option_byte_write(ob_addr + 0x0C, ob_data_addr + 0x0C);
    option_byte_write(ob_addr + 0x14, ob_data_addr + 0x14);
    option_byte_write(ob_addr + 0x1C, ob_data_addr + 0x1C);
    option_byte_write(ob_addr + 0x24, ob_data_addr + 0x24);
    option_byte_write(ob_addr + 0x34, ob_data_addr + 0x34);
}

/*!
    \brief      flash memory interface initialization routine
    \param[in]  none
    \param[out] none
    \retval     MAL_OK if the operation is right, MAL_FAIL else
*/
static uint8_t flash_if_init(void)
{
    /* unlock the internal flash */
    fmc_unlock();

    return MEM_OK;
}

/*!
    \brief      flash memory interface de-initialization routine
    \param[in]  none
    \param[out] none
    \retval     MAL_OK if the operation is right, MAL_FAIL else
*/
static uint8_t flash_if_deinit(void)
{
    /* lock the internal flash */
    fmc_lock();

    return MEM_OK;
}

/*!
    \brief      erase flash sector
    \param[in]  addr: flash address to be erased
    \param[out] none
    \retval     MAL_OK if the operation is right, MAL_FAIL else
*/
static uint8_t flash_if_erase(uint32_t addr)
{
    fmc_unlock();

    fmc_sector_erase(addr);

    fmc_lock();

    return MEM_OK;
}

/*!
    \brief      flash memory write routine
    \param[in]  buf: data buffer pointer
    \param[in]  addr: flash address to be written
    \param[in]  len: length of data to be written (in bytes)
    \param[out] none
    \retval     MAL_OK if the operation is right, MAL_FAIL else
*/
static uint8_t flash_if_write(uint8_t *buf, uint32_t addr, uint32_t len)
{
    __IO uint32_t idx = 0U, spc = 0U;
    uint32_t eft = 0U;

    if((addr == FMC_OB_START) && (len == 0x3C)) {
        ob_unlock();

        spc = (FMC_OBSTAT0_EFT & FMC_OBSTAT0_EFT_SPC) >> 8;

        ob_write_all(buf);

        eft = FMC_SCRADDR_EFT;

        if((spc != 0xAA) && (spc != 0xCC) && (buf[5] == 0xAA)) {
            FMC_STAT = 0xFFFFFFFF;

            if(FMC_DCRPADDR_EFT & FMC_DCRPADDR_EFT_DCRP_EREN) {
                uint32_t dcrp_start = FMC_DCRPADDR_EFT & FMC_DCRPADDR_EFT_DCRP_AREA_START;
                uint32_t dcrp_end = (FMC_DCRPADDR_EFT & FMC_DCRPADDR_EFT_DCRP_AREA_END) >> 16;

                if(dcrp_start <= dcrp_end) {
                    FMC_DCRPADDR_MDF &= 0x80000000;
                    FMC_DCRPADDR_MDF |= 0x000000FF;
                }

                FMC_DCRPADDR_MDF &= ~FMC_DCRPADDR_EFT_DCRP_EREN;
            }

            if(FMC_SCRADDR_EFT & FMC_SCRADDR_EFT_SCR_EREN) {
                uint32_t scr_start = FMC_SCRADDR_EFT & FMC_SCRADDR_EFT_SCR_AREA_START;
                uint32_t scr_end = (FMC_SCRADDR_EFT & FMC_SCRADDR_EFT_SCR_AREA_END) >> 16;

                if(scr_start <= scr_end) {
                    FMC_DCRPADDR_MDF |= FMC_DCRPADDR_MDF_DCRP_EREN;

                    FMC_SCRADDR_MDF = 0x800000FF;

                    FMC_OBSTAT0_MDF &= ~FMC_OBSTAT0_MDF_SCR;
                } else {
                    FMC_SCRADDR_MDF &= ~FMC_SCRADDR_MDF_SCR_EREN;
                }
            }
        } else if(eft != FMC_SCRADDR_MDF) {
            ob_start();
            ob_lock();

            while(FMC_FLAG_BUSY == (FMC_STAT & FMC_FLAG_BUSY));

            uint8_t scr_enable = 0;
            uint32_t scr_start = 0, scr_end = 0, scr_eren = 0;
            BSS_SecureAcessArea_struct sec_area;

            scr_start = FMC_SCRADDR_MDF & FMC_SCRADDR_EFT_SCR_AREA_START;
            scr_end = (FMC_SCRADDR_MDF & FMC_SCRADDR_EFT_SCR_AREA_END) >> 16;
            scr_enable = (FMC_OBSTAT0_MDF & FMC_OBSTAT0_MDF_SCR) >> 21;
            scr_eren = (FMC_SCRADDR_MDF & FMC_SCRADDR_EFT_SCR_EREN) >> 31;

            if(scr_enable && (scr_start <= scr_end)) {
                sec_area.scr_area_end = scr_end;
                sec_area.scr_area_start = scr_start;
                sec_area.scr_eren = scr_eren;

                if(vfimg_set) {
                    uint32_t mcu_rsv_value = *(uint32_t *)0x40022818;
                    uint8_t buf[4];
                    buf[0] = mcu_rsv_value & 0x000000FF;
                    buf[1] = (mcu_rsv_value & 0x0000FF00) >> 8;
                    buf[2] = (mcu_rsv_value & 0x00FF0000) >> 16;
                    buf[3] = (mcu_rsv_value & 0xFF000000) >> 24;

                    buf[0] |= 0x80;

                    efuse_mcu_reserved_write(&buf[0]);

                    vfimg_set = 0;
                }

                BSS->resetAndInitializeSecureAreas(&sec_area);
            }
        } else {
        }

        ob_start();
        ob_lock();

        while(FMC_FLAG_BUSY == (FMC_STAT & FMC_FLAG_BUSY));

        NVIC_SystemReset();
    } else if ((addr >= FLASH_START_ADDR) && (addr < FLASH_END_ADDR)) {
        /* unlock the flash program erase controller */
        fmc_unlock();

        /* not an aligned data */
        if(len & 0x03U) {
            for(idx = len; idx < ((len & 0xFFFCU) + 4U); idx++) {
                buf[idx] = 0xFFU;
            }
        }

        /* data received are word multiple */
        for(idx = 0U; idx < len; idx += 4U) {
            fmc_word_program(addr, *(uint32_t *)(buf + idx));
            addr += 4U;
        }

        fmc_lock();
    } else if((addr >= OCRAM_START) && (addr < OCRAM_END)) {
        for(idx = 0U; idx < len; idx++) {
            *(uint8_t *)addr = *(buf + idx);
            addr++;
        }
    } else if((addr == AES_IV_START) && (len == AES_IV_LEN)) {
        fmc_aes_iv_config((uint32_t *)&buf[12]);

        NVIC_SystemReset();
    } else if((addr == EFUSE_START) && (len == 0x30)) {
        if(buf[4] & 0x80) {
            if(FMC_OBSTAT0_EFT & FMC_OBSTAT0_EFT_SCR) {
                vfimg_set = 1;

                return MEM_OK;
            }
        }

        efuse_user_control_write(&buf[0]);
        efuse_mcu_reserved_write(&buf[4]);
        efuse_dp_write(&buf[8]);
        efuse_user_data_write(&buf[32]);

        NVIC_SystemReset();
    } else {
        /* no operation */
    }

    return MEM_OK;
}

/*!
    \brief      flash memory read routine
    \param[in]  buf: data buffer pointer
    \param[in]  addr: flash address to be read from
    \param[in]  len: length of data to be read (in bytes)
    \param[out] none
    \retval     pointer to the physical address where data should be read
*/
static uint8_t *flash_if_read(uint8_t *buf, uint32_t addr, uint32_t len)
{
    if(((addr >= FLASH_START_ADDR) && (addr < FLASH_END_ADDR)) || 
           ((addr >= OCRAM_START) && (addr < OCRAM_END)) || 
              ((addr >= FMC_OB_START) && (addr < FMC_OB_END))) {
        return  (uint8_t *)(addr);
    } else if((addr == EFUSE_START) && (len == EFUSE_LEN)) {
        return (uint8_t *)(addr);
    } else if((addr == AES_IV_START) && (len == AES_IV_LEN)) {
        return (uint8_t *)(addr);
    } else if(addr == 0xA0000300) {
        NVIC_SystemReset();

        return 0;
    } else if(addr == 0xA0000200) {
        certificate_size = 136;

        return (uint8_t *)(&certificate_size);
    } else if(addr == 0x20009000) {
        return (uint8_t *)(addr);
    } else if(addr == 0x1FF0FC40) {
        return (uint8_t *)(addr);
    } else {
        return buf;
    }
}

/*!
    \brief      check if the address is an allowed address for this memory
    \param[in]  addr: flash address to be checked
    \param[out] none
    \retval     MAL_OK if the operation is right, MAL_FAIL else
*/
static uint8_t flash_if_checkaddr(uint32_t addr)
{
    if((addr >= FLASH_START_ADDR) && (addr <= FLASH_END_ADDR)) {
        return MEM_OK;
    } else if((addr >= OCRAM_START) && (addr <= OCRAM_END)) {
        return MEM_OK;
    } else if((addr >= FMC_OB_START) && (addr <= FMC_OB_END)) {
        return MEM_OK;
    } else if((addr >= EFUSE_START) && (addr <= EFUSE_END)) {
        return MEM_OK;
    } else if((addr >= AES_IV_START) && (addr <= AES_IV_END)) {
        return MEM_OK;
    } else {
        return MEM_FAIL;
    }
}

/*!
    \brief      get FMC state
    \param[in]  none
    \param[out] none
    \retval     state of FMC, refer to fmc_state_enum
      \arg        FMC_READY: operation has been completed
      \arg        FMC_BUSY: operation is in progress
      \arg        FMC_WPERR: erase/program protection error
      \arg        FMC_PGSERR: program sequence error
      \arg        FMC_RPERR: read protection error
      \arg        FMC_RSERR: read secure error
      \arg        FMC_ECCCOR: one bit correct error
      \arg        FMC_ECCDET: two bits detect error
      \arg        FMC_OBMERR: option byte modify error
*/
static fmc_state_enum fmc_state_get(void)
{
    fmc_state_enum fmc_state = FMC_READY;

    if((uint32_t)0x00U != (FMC_STAT & FMC_STAT_BUSY)) {
        fmc_state = FMC_BUSY;
    } else {
        if((uint32_t)0x00U != (FMC_STAT & FMC_STAT_WPERR)) {
            fmc_state = FMC_WPERR;
        } else if((uint32_t)0x00U != (FMC_STAT & FMC_STAT_PGSERR)) {
            fmc_state = FMC_PGSERR;
        } else if((uint32_t)0x00U != (FMC_STAT & FMC_STAT_PGSERR)) {
            fmc_state = FMC_PGSERR;
        } else if((uint32_t)0x00U != (FMC_STAT & FMC_STAT_RPERR)) {
            fmc_state = FMC_RPERR;
        } else if((uint32_t)0x00U != (FMC_STAT & FMC_STAT_RSERR)) {
            fmc_state = FMC_RSERR;
        } else if((uint32_t)0x00U != (FMC_STAT & FMC_STAT_ECCCOR)) {
            fmc_state = FMC_ECCCOR;
        } else if((uint32_t)0x00U != (FMC_STAT & FMC_STAT_ECCDET)) {
            fmc_state = FMC_ECCDET;
        } else if((uint32_t)0x00U != (FMC_STAT & FMC_STAT_OBMERR)) {
            fmc_state = FMC_OBMERR;
        } else {
            /* illegal parameters */
        }
    }

    /* return the FMC state */
    return fmc_state;
}

/*!
    \brief      check whether FMC is ready or not
    \param[in]  timeout: timeout count
    \param[out] none
    \retval     state of FMC, refer to fmc_state_enum
      \arg        FMC_READY: operation has been completed
      \arg        FMC_BUSY: operation is in progress
      \arg        FMC_WPERR: erase/program protection error
      \arg        FMC_PGSERR: program sequence error
      \arg        FMC_RPERR: read protection error
      \arg        FMC_RSERR: read secure error
      \arg        FMC_ECCCOR: one bit correct error
      \arg        FMC_ECCDET: two bits detect error
      \arg        FMC_OBMERR: option byte modify error
      \arg        FMC_TOERR: timeout error
*/
static fmc_state_enum fmc_ready_wait(uint32_t timeout)
{
    fmc_state_enum fmc_state = FMC_BUSY;

    /* wait for FMC ready */
    do {
        /* get FMC state */
        fmc_state = fmc_state_get();
        timeout--;
    } while((FMC_BUSY == fmc_state) && (0U != timeout));

    if(FMC_BUSY == fmc_state) {
        fmc_state = FMC_TOERR;
    }

    /* return the FMC state */
    return fmc_state;
}
