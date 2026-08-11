/*!
    \file    main.c
    \brief   EXMC NOR Flash demo

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
#include "exmc_norflash.h"

#define BUFFER_SIZE               0x100
#define WRITE_READ_ADDR           0xFFF200

nor_id_struct nor_id;
uint32_t writereadstatus = 0;
uint16_t writebuffer[BUFFER_SIZE];
uint16_t readbuffer[BUFFER_SIZE];
nor_status_struct status;

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
    uint32_t index;

    /* enable the CPU cache */
    cache_enable();
    /* configure the MPU */
    mpu_config();

    /* initialize LEDs */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    /* configure the USART */
    gd_eval_com_init(EVAL_COM);
    /* configure the EXMC access mode */
    exmc_norflash_init(EXMC_BANK0_NORSRAM_REGION1);
    /* read nor flash id */
    nor_read_id(&nor_id);

    printf("\r\nread NOR ID");
    /* print NOR ID */
    printf("\r\nNOR flash ID:0x%X 0x%X 0x%X 0x%X\r\n", nor_id.manufacturer_code, nor_id.device_code1,
           nor_id.device_code2, nor_id.device_code3);
    nor_return_to_read_mode();

    /* erase the nor flash block to be written data */
    status = nor_eraseblock(WRITE_READ_ADDR);
    if(NOR_SUCCESS == status) {
        printf("\r\nerase nor flash block successfully!");
    } else {
        printf("\r\nerase nor flash block failure!");

        /* failure, light on LED2 */
        gd_eval_led_on(LED2);
        while(1);
    }

    /* whether address cross-border */
    if((WRITE_READ_ADDR + BUFFER_SIZE) > NOR_MAX_ADDRESS) {
        printf("\n\raddress cross-border\n\r");

        /* failure, light on LED2 */
        gd_eval_led_on(LED2);
        while(1);
    }

    /* fill writeBuffer with the specified value */
    nor_fill_buffer(writebuffer, BUFFER_SIZE, 0x0006);

    /* write data to nor flash, WRITE_READ_ADDR: the starting address of the write data */
    status = nor_write_buffer(writebuffer, WRITE_READ_ADDR, BUFFER_SIZE);
    if(NOR_SUCCESS == status) {
        printf("\r\nwrite data to nor flash block successfully!");
    } else {
        printf("\r\nwrite data to nor flash block failure!");

        /* failure, light on LED2 */
        gd_eval_led_on(LED2);
        while(1);
    }

    /* read data from nor flash, WRITE_READ_ADDR: the starting address of the read data*/
    nor_readbuffer(readbuffer, WRITE_READ_ADDR, BUFFER_SIZE);

    /* read and write data comparison for equality */
    writereadstatus = 0;
    for(index = 0x00; index < BUFFER_SIZE; index++) {
        if(readbuffer[index] != writebuffer[index]) {
            writereadstatus++;
            break;
        }
    }

    printf("\n\rthe result to access the nor flash:\n\r");
    if(writereadstatus == 0) {
        printf("\n\raccess nor flash successfully!\n\r");
        gd_eval_led_on(LED1);
    } else {
        printf("\n\raccess nor flash failure!\n\r");

        /* failure, light on LED2 */
        gd_eval_led_on(LED2);
        while(1);
    }

    printf("\n\rprintf data to be read: \n\r");
    printf("\n\r");
    for(index = 0; index < BUFFER_SIZE; index++) {
        printf("%d ", readbuffer[index]);
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
    mpu_init_struct.region_base_address = BANK_NORFLASH_ADDR;
    mpu_init_struct.region_size         = MPU_REGION_SIZE_16MB;
    mpu_init_struct.access_permission   = MPU_AP_FULL_ACCESS;
    mpu_init_struct.access_bufferable   = MPU_ACCESS_BUFFERABLE;
    mpu_init_struct.access_cacheable    = MPU_ACCESS_NON_CACHEABLE;
    mpu_init_struct.access_shareable    = MPU_ACCESS_SHAREABLE;
    mpu_init_struct.region_number       = MPU_REGION_NUMBER1;
    mpu_init_struct.subregion_disable   = MPU_SUBREGION_ENABLE;
    mpu_init_struct.instruction_exec    = MPU_INSTRUCTION_EXEC_PERMIT;
    mpu_init_struct.tex_type            = MPU_TEX_TYPE0;
    mpu_region_config(&mpu_init_struct);
    mpu_region_enable();

    /* enable the MPU */
    ARM_MPU_Enable(MPU_MODE_PRIV_DEFAULT);
}
