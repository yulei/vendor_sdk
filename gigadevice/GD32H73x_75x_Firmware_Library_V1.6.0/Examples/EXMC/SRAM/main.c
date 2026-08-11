/*!
    \file    main.c
    \brief   EXMC SRAM demo

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
#include <stdio.h>
#include "gd32h759i_eval.h"
#include "exmc_sram.h"

#define BUFFER_SIZE             100                  /*!< write or read buffer size */
#define WRITE_READ_ADDR         0x0000              /*!< SRAM write or read address */

/* SRAM */
uint16_t txbuffer[BUFFER_SIZE];
uint16_t rxbuffer[BUFFER_SIZE];
uint16_t writereadstatus = 0;

void cache_enable(void);
void mpu_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    uint16_t i = 0;

    /* enable the CPU Cache */
    cache_enable();
    /* configure the MPU */
    mpu_config();

    /* initialize LEDs */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    /* configure the USART */
    gd_eval_com_init(EVAL_COM);
    /* configure the EXMC access mode */
    exmc_sram_init(EXMC_BANK0_NORSRAM_REGION0);
    /* fill txbuffer */
    fill_buffer_16(txbuffer, BUFFER_SIZE, 0x1215);
    /* write data to SRAM  */
    exmc_sram_writebuffer_16(txbuffer, WRITE_READ_ADDR, BUFFER_SIZE);
    /* read data from SRAM */
    exmc_sram_readbuffer_16(rxbuffer, WRITE_READ_ADDR, BUFFER_SIZE);
    /* compare two buffers */
    for(i = 0; i < BUFFER_SIZE; i++) {
        if(rxbuffer[i] != txbuffer[i]) {
            writereadstatus ++;
            break;
        }
    }

    if(writereadstatus) {
        printf("\r\nSRAM test failed!");

        /* failure, light on LED2 */
        gd_eval_led_on(LED2);
    } else {
        printf("\r\nSRAM test succeeded!");
        printf("\r\nthe data is:\r\n");
        for(i = 0; i < BUFFER_SIZE; i++) {
            printf("%6x", rxbuffer[i]);
            if(((i + 1) % 6) == 0) {
                printf("\r\n");
            }
        }
        /* success, light on LED1 */
        gd_eval_led_on(LED1);
    }

    while(1);
}

/*!
    \brief      enable the CPU cache
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cache_enable(void)
{
    /* enable I-Cache and D-Cache */
    SCB_EnableICache();
    SCB_EnableDCache();
}

/*!
    \brief      configure the MPU
    \param[in]  none
    \param[out] none
    \retval     none
*/
void mpu_config(void)
{
    mpu_region_init_struct mpu_init_struct;
    
    /* disable the MPU */
    ARM_MPU_Disable();
    ARM_MPU_SetRegion(0, 0);
    
    /* configure the MPU attributes for the entire 4GB area, Reserved, no access */
    /* This configuration is highly recommended to prevent Speculative Prefetching of external memory, 
       which may cause CPU read locks and even system errors */
    mpu_init_struct.region_base_address  = 0x0;
    mpu_init_struct.region_size          = MPU_REGION_SIZE_4GB;
    mpu_init_struct.access_permission    = MPU_AP_NO_ACCESS;
    mpu_init_struct.access_bufferable    = MPU_ACCESS_NON_BUFFERABLE;
    mpu_init_struct.access_cacheable     = MPU_ACCESS_NON_CACHEABLE;
    mpu_init_struct.access_shareable     = MPU_ACCESS_SHAREABLE;
    mpu_init_struct.region_number        = MPU_REGION_NUMBER0;
    mpu_init_struct.subregion_disable    = 0x87;
    mpu_init_struct.instruction_exec     = MPU_INSTRUCTION_EXEC_NOT_PERMIT;
    mpu_init_struct.tex_type             = MPU_TEX_TYPE0;
    mpu_region_config(&mpu_init_struct);
    mpu_region_enable();

    /* configure the MPU attributes for NOR */
    mpu_init_struct.region_base_address = BANK_SRAM_ADDR;
    mpu_init_struct.region_size         = MPU_REGION_SIZE_512KB;
    mpu_init_struct.access_permission   = MPU_AP_FULL_ACCESS;
    mpu_init_struct.access_bufferable   = MPU_ACCESS_NON_BUFFERABLE;
    mpu_init_struct.access_cacheable    = MPU_ACCESS_NON_CACHEABLE;
    mpu_init_struct.access_shareable    = MPU_ACCESS_SHAREABLE;
    mpu_init_struct.region_number       = MPU_REGION_NUMBER1;
    mpu_init_struct.subregion_disable   = MPU_SUBREGION_ENABLE;
    mpu_init_struct.instruction_exec    = MPU_INSTRUCTION_EXEC_PERMIT;
    mpu_init_struct.tex_type            = MPU_TEX_TYPE1;
    mpu_region_config(&mpu_init_struct);
    mpu_region_enable();

    /* enable the MPU */
    ARM_MPU_Enable(MPU_MODE_PRIV_DEFAULT);
}
