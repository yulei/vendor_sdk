/*!
    \file    main.c
    \brief   NAND test demo

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
#include "exmc_nandflash.h"

#define BUFFER_SIZE                 (0x100U)
#define NAND_GD_MAKERID             (0xC8U)
#define NAND_HY_MAKERID             (0xADU)
#define NAND_DEVICEID               (0xF1U)

nand_id_struct nand_id;
uint8_t txbuffer[BUFFER_SIZE], rxbuffer[BUFFER_SIZE];
__IO uint32_t writereadstatus = 0, status = 0;
uint32_t j = 0;
uint32_t k = 0;
uint32_t writereadaddr ;
uint16_t zone, block, page, pageoffset;

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
    /* enable the CPU Cache */
    cache_enable();
    /* configure the MPU */
    mpu_config();

    /* LED initialize */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    /* configure the USART */
    gd_eval_com_init(EVAL_COM);
    /* configure the EXMC access mode */
    exmc_nandflash_init();
    /* read NAND ID */
    nand_read_id(&nand_id);

    printf("read NAND ID");
    /* print NAND ID */
    printf("\r\nNand flash ID:0x%X 0x%X 0x%X 0x%X\r\n", nand_id.maker_id, nand_id.device_id,
           nand_id.third_id, nand_id.fourth_id);

    if(((NAND_GD_MAKERID == nand_id.maker_id) || (NAND_HY_MAKERID == nand_id.maker_id)) && (NAND_DEVICEID == nand_id.device_id)) {
        /* set the read and write the address */
        zone = 0;
        block = 10;
        page = 0;
        pageoffset = 1100;
        writereadaddr = ((zone * NAND_ZONE_SIZE + block) * NAND_BLOCK_SIZE + page)
                        * NAND_PAGE_SIZE + pageoffset;

        /* whether address cross-border */
        if((writereadaddr + BUFFER_SIZE) > NAND_MAX_ADDRESS) {
            printf("\r\naddress cross-border");

            /* failure, light on LED2 */
            gd_eval_led_on(LED2);
            while(1);
        }

        /* fill writebuffer with 0x00.. */
        fill_buffer_nand(txbuffer, BUFFER_SIZE, 0x00);

        /* write data to nand flash */
        status = nand_write(writereadaddr, txbuffer, BUFFER_SIZE);
        if(NAND_OK == status) {
            printf("\r\nwrite data successfully!");
        } else {
            printf("\r\nwrite data failure!");

            /* failure, light on LED2 */
            gd_eval_led_on(LED2);
            while(1);
        }

        /* read data from nand flash */
        status = nand_read(writereadaddr, rxbuffer, BUFFER_SIZE);
        if(NAND_OK == status) {
            printf("\r\nread data successfully!");
        } else {
            printf("\r\nread data failure!\n\r");

            /* failure, light on LED2 */
            gd_eval_led_on(LED2);
            while(1);
        }

        /* Read and write data comparison for equality */
        writereadstatus = 0;
        for(j = 0; j < BUFFER_SIZE; j++) {
            if(txbuffer[j] != rxbuffer[j]) {
                writereadstatus++;
                break;
            }
        }

        printf("\r\nthe result to access the nand flash:");
        if(writereadstatus == 0) {
            printf("\r\naccess NAND flash successfully!");

            gd_eval_led_on(LED1);
        } else {
            printf("\r\naccess NAND flash failure!");

            /* failure, light on LED2 */
            gd_eval_led_on(LED2);
            while(1);
        }
        printf("\r\nprintf data to be read:\r\n");
        for(k = 0; k < BUFFER_SIZE; k ++) {
            printf("0x%02X ", rxbuffer[k]);
        }
    } else {
        printf("\n\rread NAND ID failure!\n\r");

        /* failure, light on LED2 */
        gd_eval_led_on(LED2);
        while(1);
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
    mpu_region_struct_para_init(&mpu_init_struct);

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

    /* configure the MPU attributes for NAND */
    mpu_init_struct.region_base_address = BANK_NAND_ADDR;
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
