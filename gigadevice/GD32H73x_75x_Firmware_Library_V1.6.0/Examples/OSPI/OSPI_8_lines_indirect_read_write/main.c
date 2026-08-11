/*!
    \file    main.c
    \brief   OSPI 8 lines indirect read and write

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

#include "gd32h73x_75x.h"
#include "gd32h759i_eval.h"
#include <stdio.h>
#include "gd25x512me.h"

#define countof(a)               (sizeof(a) / sizeof(*(a)))
#define buffersize               (countof(tx_buffer) - 1)
#define GD25X512ME_ID            0xC8481AFF
#define FLASH_WRITE_ADDRESS      0x000000
#define FLASH_READ_ADDRESS       FLASH_WRITE_ADDRESS
#define OSPI_INTERFACE           OSPI0
//#define OSPI_INTERFACE         OSPI1

uint8_t tx_buffer[] = "GDMH7xx octal-flash OSPI mode with 8 lines in indirect mode read&write test!\r\n";
uint8_t rx_buffer[buffersize];

ospi_parameter_struct ospi_struct = {0};
uint32_t flashid = 0;
uint8_t i = 0;

void cache_enable(void);
ErrStatus memory_compare(uint8_t *src, uint8_t *dst, uint16_t length);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* enable I-Cache and D-Cache */
    cache_enable();

    /* configure USART */
    gd_eval_com_init(EVAL_COM);

    printf("\n\r#########################################################################\n\r");
    printf("\n\rOSPI read flash ID and read/write with 8 lines in indirect mode test!\n\r");

    /* initialize OSPI/OSPIM  and GPIO */
    ospi_flash_init(OSPI_INTERFACE, &ospi_struct);

    /* reset ospi flash */
    ospi_flash_reset_enable(OSPI_INTERFACE, &ospi_struct, SPI_MODE);
    ospi_flash_reset_memory(OSPI_INTERFACE, &ospi_struct, SPI_MODE);
    ospi_flash_reset_enable(OSPI_INTERFACE, &ospi_struct, OSPI_MODE);
    ospi_flash_reset_memory(OSPI_INTERFACE, &ospi_struct, OSPI_MODE);

    /* read flash ID */
    flashid = ospi_flash_read_id(OSPI_INTERFACE, &ospi_struct, SPI_MODE);

    if(GD25X512ME_ID == flashid) {
        printf("\n\rThe device ID is 0x%X\n\r", flashid);
        printf("\n\rThe data written with 8 lines in indirect mode to flash is:\n");

        printf("\n\r%s\n\r", tx_buffer);
    } else {
        printf("\n\rFailed to read device ID\n\r");
        while(1) {
        }
    }

    /* configure OSPI FLASH dummy cycles */
    ospi_flash_write_enbale(OSPI_INTERFACE, &ospi_struct, SPI_MODE);
    ospi_flash_write_volatilecfg_register(OSPI_INTERFACE, &ospi_struct, SPI_MODE, GD25X512ME_3BYTES_SIZE, GD25X512ME_CFG_REG1_ADDR,
                                          GD25X512ME_CFG_16_DUMMY_CYCLES);

    /* configure OSPI FLASH enter STR OSPI mode */
    ospi_flash_write_enbale(OSPI_INTERFACE, &ospi_struct, SPI_MODE);
    ospi_flash_write_volatilecfg_register(OSPI_INTERFACE, &ospi_struct, SPI_MODE, GD25X512ME_3BYTES_SIZE, GD25X512ME_CFG_REG0_ADDR,
                                          GD25X512ME_CFG_OCTAL_STR_WO);
    ospi_flash_autopolling_mem_ready(OSPI_INTERFACE, &ospi_struct, OSPI_MODE);

    /* erase specified address */
    ospi_flash_autopolling_mem_ready(OSPI_INTERFACE, &ospi_struct, OSPI_MODE);
    ospi_flash_write_enbale(OSPI_INTERFACE, &ospi_struct, OSPI_MODE);
    ospi_flash_block_erase(OSPI_INTERFACE, &ospi_struct, OSPI_MODE, GD25X512ME_3BYTES_SIZE, FLASH_WRITE_ADDRESS, GD25X512ME_ERASE_4K);
    ospi_flash_autopolling_mem_ready(OSPI_INTERFACE, &ospi_struct, OSPI_MODE);

    /* write data of tx_buffer to flash */
    ospi_flash_autopolling_mem_ready(OSPI_INTERFACE, &ospi_struct, OSPI_MODE);
    ospi_flash_write_enbale(OSPI_INTERFACE, &ospi_struct, OSPI_MODE);
    ospi_flash_page_program(OSPI_INTERFACE, &ospi_struct, OSPI_MODE, GD25X512ME_3BYTES_SIZE, tx_buffer, FLASH_WRITE_ADDRESS, buffersize);
    ospi_flash_autopolling_mem_ready(OSPI_INTERFACE, &ospi_struct, OSPI_MODE);

    /* read data from flash */
    ospi_flash_read(OSPI_INTERFACE, &ospi_struct, OSPI_MODE, GD25X512ME_3BYTES_SIZE, rx_buffer, FLASH_READ_ADDRESS, buffersize);

    if(ERROR != memory_compare(tx_buffer, rx_buffer, buffersize)) {
        printf("\n\rThe data read with 8 lines in indirect mode from flash is:\n");
        
        printf("\n\r%s\n\r", rx_buffer);
        
        printf("\r\n\r\nOSPI read/write with 8 lines in indirect test success!\r\n");
    } else {
        printf("\r\n\r\nOSPI read/write with 8 lines in indirect test failed!\r\n");
        while(1) {
        }
    }

    while(1) {
    }
}

/*!
    \brief      enable the CPU Chache
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cache_enable(void)
{
    /* enable I-Cache */
    SCB_EnableICache();

    /* enable D-Cache */
    SCB_EnableDCache();
}

/*!
    \brief      memory compare function
    \param[in]  src: source data pointer
    \param[in]  dst: destination data pointer
    \param[in]  length: the compare data length
    \param[out] none
    \retval     ErrStatus: ERROR or SUCCESS
*/
ErrStatus memory_compare(uint8_t *src, uint8_t *dst, uint16_t length)
{
    while(length --) {
        if(*src++ != *dst++) {
            return ERROR;
        }
    }
    return SUCCESS;
}
