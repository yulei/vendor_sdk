/*!
    \file    flash_operation.c
    \brief   flash operation driver

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

#include "flash_operation.h"

static fmc_state_enum fmc_ready_wait_check(uint32_t timeout);

/*!
    \brief      erase data of flash
    \param[in]  address: sector address/code
    \param[in]  file_length: length of the file 
    \param[in]  report_buffer: report buffer
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
fmc_state_enum flash_erase(uint32_t address, uint32_t file_length)
{
    uint16_t page_count = 0U, i = 0U;

    fmc_state_enum fmc_state;

    if(0U == (file_length % PAGE_SIZE)) {
        page_count = (uint16_t)(file_length / PAGE_SIZE);
    } else {
        page_count = (uint16_t)(file_length / PAGE_SIZE + 1U);
    }

    /* clear pending flags */
    fmc_flag_clear(FMC_FLAG_PGSERR);
    fmc_flag_clear(FMC_FLAG_WPERR);
    fmc_flag_clear(FMC_FLAG_END);

    /* unlock the flash program erase controller */
    fmc_unlock();

    for(i = 0U; i < page_count; i++) {
        /* call the standard flash erase-page function */
        fmc_state = fmc_sector_erase(address);

        address += PAGE_SIZE;
    }

    SCB_CleanInvalidateDCache();

    fmc_lock();

    return fmc_state;
}

/*!
    \brief      write data to sectors of flash
    \param[in]  data: data to be written
    \param[in]  addr: sector address/code
    \param[in]  len: length of data to be written (in bytes)
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
fmc_state_enum iap_data_write(uint8_t *data, uint32_t addr, uint32_t len)
{
    uint32_t idx = 0U;
    fmc_state_enum fmc_state = FMC_READY;

    /* check if the address is in protected area */
    if(IS_PROTECTED_AREA(addr)) {
        return fmc_state;
    }

    if(len & 0x03U) {/* not an aligned data */
        for(idx = len; idx < ((len & 0xFFFCU) + 4U); idx++) {
            data[idx] = 0xFFU;
        }
    }

   /* unlock the flash program erase controller */
    fmc_unlock();

    /* data received are word multiple */
    for(idx = 0U; idx < len; idx += 4U) {
        fmc_state = fmc_word_program(addr, *(uint32_t *)(data + idx));

        if(FMC_READY == fmc_state) {
            addr += 4U;
        } else {
            addr += 4U;
            fmc_flag_clear(FMC_FLAG_PGSERR);
            fmc_flag_clear(FMC_FLAG_WPERR);
            fmc_flag_clear(FMC_FLAG_END);
            fmc_state = FMC_READY;
        }
    }

    fmc_lock();

    return fmc_state;
}


/*!
    \brief      write option byte
    \param[in]  mem_addr: option byte address
    \param[in]  data: data to be written
    \param[in]  len: length of data to be written (in bytes)
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
fmc_state_enum option_byte_write(uint32_t mem_add, uint8_t* data, uint32_t len)
{
    fmc_state_enum status = FMC_READY;
    uint32_t write_data;
    uint8_t i = 0;

    for(i = 0; i < len / 4; i++) {
        write_data = (uint32_t)data[0 + 4*i] | (uint32_t)(data[1 + 4*i] << 8) | (uint32_t)(data[2 + 4*i] << 16) | (uint32_t)(data[3 + 4*i] << 24);

        /* clear pending flags */
        fmc_flag_clear(FMC_FLAG_PGSERR);
        fmc_flag_clear(FMC_FLAG_WPERR);
        fmc_flag_clear(FMC_FLAG_END);

        status = fmc_ready_wait_check(FMC_TIMEOUT_COUNT);

        if(FMC_READY != status) {
            return status;
        }

        *(__IO uint32_t*)mem_add = write_data;
        
        mem_add += 4;
    }

    return status;
}

/*!
    \brief      write efuse
    \param[in]  buf: data buffer to be written
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
fmc_state_enum efuse_program(uint8_t *buf)
{
    efuse_flag_clear(EFUSE_FLAG_ILLEGAL_ACCESS_ERR_CLR | EFUSE_FLAG_PROGRAM_VOLTAGE_ERR_CLR);
    efuse_flag_clear(EFUSE_FLAG_READ_COMPLETE_CLR | EFUSE_FLAG_PROGRAM_COMPLETE_CLR);
    if(SUCCESS != efuse_user_control_write(&buf[0])) {
        return FMC_WPERR;
    }
    efuse_flag_clear(EFUSE_FLAG_ILLEGAL_ACCESS_ERR_CLR | EFUSE_FLAG_PROGRAM_VOLTAGE_ERR_CLR);
    efuse_flag_clear(EFUSE_FLAG_READ_COMPLETE_CLR | EFUSE_FLAG_PROGRAM_COMPLETE_CLR);
    if(SUCCESS != efuse_mcu_reserved_write(&buf[4])) {
        return FMC_WPERR;
    }
    efuse_flag_clear(EFUSE_FLAG_ILLEGAL_ACCESS_ERR_CLR | EFUSE_FLAG_PROGRAM_VOLTAGE_ERR_CLR);
    efuse_flag_clear(EFUSE_FLAG_READ_COMPLETE_CLR | EFUSE_FLAG_PROGRAM_COMPLETE_CLR);
    if(SUCCESS != efuse_dp_write(&buf[8])) {
        return FMC_WPERR;
    }
    efuse_flag_clear(EFUSE_FLAG_ILLEGAL_ACCESS_ERR_CLR | EFUSE_FLAG_PROGRAM_VOLTAGE_ERR_CLR);
    efuse_flag_clear(EFUSE_FLAG_READ_COMPLETE_CLR | EFUSE_FLAG_PROGRAM_COMPLETE_CLR);
    if(SUCCESS != efuse_aes_key_write(&buf[16])) {
        return FMC_WPERR;
    }
    efuse_flag_clear(EFUSE_FLAG_ILLEGAL_ACCESS_ERR_CLR | EFUSE_FLAG_PROGRAM_VOLTAGE_ERR_CLR);
    efuse_flag_clear(EFUSE_FLAG_READ_COMPLETE_CLR | EFUSE_FLAG_PROGRAM_COMPLETE_CLR);
    if(SUCCESS != efuse_user_data_write(&buf[32])) {
        return FMC_WPERR;
    }
    efuse_flag_clear(EFUSE_FLAG_ILLEGAL_ACCESS_ERR_CLR | EFUSE_FLAG_PROGRAM_VOLTAGE_ERR_CLR);
    efuse_flag_clear(EFUSE_FLAG_READ_COMPLETE_CLR | EFUSE_FLAG_PROGRAM_COMPLETE_CLR);

    return FMC_READY;
}

/*!
    \brief      jump to execute address
    \param[in]  addr: execute address
    \param[out] none
    \retval     none
*/
void jump_to_execute(uint32_t addr)
{
    uint32_t exe_addr = 0x0;

    /* set interrupt vector base address */
    SCB->VTOR = addr;
    __DSB();

    exe_addr = *(uint32_t*)(addr + 4);
    (*((void (*)())exe_addr))();
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
static fmc_state_enum fmc_ready_wait_check(uint32_t timeout)
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
