/*!
    \file    main.c
    \brief   EXMC SDRAM using bank swap function demo

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
#include "systick.h"
#include <stdio.h>
#include "exmc_sdram.h"

/* SDRAM */
#define BUFFER_SIZE                ((uint32_t)0x00000400)
#define WRITE_READ_ADDR            ((uint32_t)0x00000000)

static uint32_t writereadstatus = 0;
static uint8_t txbuffer[BUFFER_SIZE];
static uint8_t rxbuffer[BUFFER_SIZE];

void rcu_exmc_config(void);
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

    /* configure the clock of EXMC */
    rcu_exmc_config();
    /* enable the CPU cache */
    cache_enable();
    /* configure the MPU */
    mpu_config();

    /* systick clock configure */
    systick_config();

    /* configure the EXMC access mode */
    exmc_synchronous_dynamic_ram_init(EXMC_SDRAM_DEVICE0);
    /* enable the EXMC bank swap function */
    exmc_norsram_sdram_remap_config(EXMC_BANK_NORPSRAM_SDRAM_SWAP);

    delay_1ms(1000);

    /* fill txbuffer */
    fill_buffer(txbuffer, BUFFER_SIZE, 0x0000);

    /* write data to SDRAM */
    sdram_writebuffer_8(EXMC_SDRAM_DEVICE0, txbuffer, WRITE_READ_ADDR, BUFFER_SIZE);

    /* read data from SDRAM */
    sdram_readbuffer_8(EXMC_SDRAM_DEVICE0, rxbuffer, WRITE_READ_ADDR, BUFFER_SIZE);

    delay_1ms(1000);

    /* compare two buffers */
    for(i = 0; i < BUFFER_SIZE; i++) {
        if(rxbuffer[i] != txbuffer[i]) {
            writereadstatus ++;
            break;
        }
    }

    if(writereadstatus) {
        /* SDRAM test failed! */
        while(1);
    }

    while(1);
}

/*!
    \brief      configure the EXMC clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_exmc_config(void)
{
    /* PLL1R as EXMC source clock */
    rcu_pll_source_config(RCU_PLLSRC_HXTAL);
    rcu_pll_input_output_clock_range_config(IDX_PLL1, RCU_PLL1RNG_1M_2M, RCU_PLL1VCO_192M_836M);
    
    /* PLL1R clock = HXTAL/PSC*N/R = 25/25*332/1 = 332MHz */
    rcu_pll1_config(HXTAL_VALUE/1000000, 332, 1, 1, 1);
    rcu_pll_clock_output_enable(RCU_PLL1R);
    rcu_osci_on(RCU_PLL1_CK);
    while(ERROR == rcu_osci_stab_wait(RCU_PLL1_CK)){
    }
    rcu_exmc_clock_config(RCU_EXMCSRC_PLL1R);
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

    /* configure the MPU attributes for SDRAM */
    mpu_init_struct.region_base_address = SDRAM_DEVICE0_ADDR;
    mpu_init_struct.region_size         = MPU_REGION_SIZE_32MB;
    mpu_init_struct.access_permission   = MPU_AP_FULL_ACCESS;
    mpu_init_struct.access_bufferable   = MPU_ACCESS_NON_BUFFERABLE;
    mpu_init_struct.access_cacheable    = MPU_ACCESS_CACHEABLE;
    mpu_init_struct.access_shareable    = MPU_ACCESS_NON_SHAREABLE;
    mpu_init_struct.region_number       = MPU_REGION_NUMBER1;
    mpu_init_struct.subregion_disable   = MPU_SUBREGION_ENABLE;
    mpu_init_struct.instruction_exec    = MPU_INSTRUCTION_EXEC_PERMIT;
    mpu_init_struct.tex_type            = MPU_TEX_TYPE0;    
    mpu_region_config(&mpu_init_struct);
    mpu_region_enable();

    /* configure the MPU attributes for SDRAM swapped address */
    mpu_init_struct.region_base_address = SDRAM_DEVICE0_ADDR_SWAP;
    mpu_init_struct.region_size         = MPU_REGION_SIZE_16MB;
    mpu_init_struct.access_permission   = MPU_AP_FULL_ACCESS;
    mpu_init_struct.access_bufferable   = MPU_ACCESS_NON_BUFFERABLE;
    mpu_init_struct.access_cacheable    = MPU_ACCESS_CACHEABLE;
    mpu_init_struct.access_shareable    = MPU_ACCESS_NON_SHAREABLE;
    mpu_init_struct.region_number       = MPU_REGION_NUMBER2;
    mpu_init_struct.subregion_disable   = MPU_SUBREGION_ENABLE;
    mpu_init_struct.instruction_exec    = MPU_INSTRUCTION_EXEC_PERMIT;
    mpu_init_struct.tex_type            = MPU_TEX_TYPE0;    
    mpu_region_config(&mpu_init_struct);
    mpu_region_enable();

    /* enable the MPU */
    ARM_MPU_Enable(MPU_MODE_PRIV_DEFAULT);
}
