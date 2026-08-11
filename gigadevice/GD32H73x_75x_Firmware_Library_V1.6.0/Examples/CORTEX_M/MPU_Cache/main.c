/*!
    \file    main.c
    \brief   Cortex-M MPU example

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

#define AXI_SRAM_ADDR          0x24000000
#define DATANUM                320

__IO uint32_t trans_end __attribute__((section(".ARM.__at_0x20000000"))) = 0;
__ALIGNED(32) static uint32_t buf_dst[DATANUM];
static const uint32_t buf_src[DATANUM] = {
    0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F, 0x10111213, 0x14151617, 0x18191A1B, 0x1C1D1E1F,
    0x20212223, 0x24252627, 0x28292A2B, 0x2C2D2E2F, 0x30313233, 0x34353637, 0x38393A3B, 0x3C3D3E3F,
    0x40414243, 0x44454647, 0x48494A4B, 0x4C4D4E4F, 0x50515253, 0x54555657, 0x58595A5B, 0x5C5D5E5F,
    0x60616263, 0x64656667, 0x68696A6B, 0x6C6D6E6F, 0x70717273, 0x74757677, 0x78797A7B, 0x7C7D7E7F,
    0x80818283, 0x84858687, 0x88898A8B, 0x8C8D8E8F, 0x90919293, 0x94959697, 0x98999A9B, 0x9C9D9E9F,
    0xA0A1A2A3, 0xA4A5A6A7, 0xA8A9AAAB, 0xACADAEAF, 0xB0B1B2B3, 0xB4B5B6B7, 0xB8B9BABB, 0xBCBDBEBF,
    0xC0C1C2C3, 0xC4C5C6C7, 0xC8C9CACB, 0xCCCDCECF, 0xD0D1D2D3, 0xD4D5D6D7, 0xD8D9DADB, 0xDCDDDEDF,
    0xE0E1E2E3, 0xE4E5E6E7, 0xE8E9EAEB, 0xECEDEEEF, 0xF0F1F2F3, 0xF4F5F6F7, 0xF8F9FAFB, 0xFCFDFEFF,
    0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F, 0x10111213, 0x14151617, 0x18191A1B, 0x1C1D1E1F,
    0x20212223, 0x24252627, 0x28292A2B, 0x2C2D2E2F, 0x30313233, 0x34353637, 0x38393A3B, 0x3C3D3E3F,
    0x40414243, 0x44454647, 0x48494A4B, 0x4C4D4E4F, 0x50515253, 0x54555657, 0x58595A5B, 0x5C5D5E5F,
    0x60616263, 0x64656667, 0x68696A6B, 0x6C6D6E6F, 0x70717273, 0x74757677, 0x78797A7B, 0x7C7D7E7F,
    0x80818283, 0x84858687, 0x88898A8B, 0x8C8D8E8F, 0x90919293, 0x94959697, 0x98999A9B, 0x9C9D9E9F,
    0xA0A1A2A3, 0xA4A5A6A7, 0xA8A9AAAB, 0xACADAEAF, 0xB0B1B2B3, 0xB4B5B6B7, 0xB8B9BABB, 0xBCBDBEBF,
    0xC0C1C2C3, 0xC4C5C6C7, 0xC8C9CACB, 0xCCCDCECF, 0xD0D1D2D3, 0xD4D5D6D7, 0xD8D9DADB, 0xDCDDDEDF,
    0xE0E1E2E3, 0xE4E5E6E7, 0xE8E9EAEB, 0xECEDEEEF, 0xF0F1F2F3, 0xF4F5F6F7, 0xF8F9FAFB, 0xFCFDFEFF,
    0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F, 0x10111213, 0x14151617, 0x18191A1B, 0x1C1D1E1F,
    0x20212223, 0x24252627, 0x28292A2B, 0x2C2D2E2F, 0x30313233, 0x34353637, 0x38393A3B, 0x3C3D3E3F,
    0x40414243, 0x44454647, 0x48494A4B, 0x4C4D4E4F, 0x50515253, 0x54555657, 0x58595A5B, 0x5C5D5E5F,
    0x60616263, 0x64656667, 0x68696A6B, 0x6C6D6E6F, 0x70717273, 0x74757677, 0x78797A7B, 0x7C7D7E7F,
    0x80818283, 0x84858687, 0x88898A8B, 0x8C8D8E8F, 0x90919293, 0x94959697, 0x98999A9B, 0x9C9D9E9F,
    0xA0A1A2A3, 0xA4A5A6A7, 0xA8A9AAAB, 0xACADAEAF, 0xB0B1B2B3, 0xB4B5B6B7, 0xB8B9BABB, 0xBCBDBEBF,
    0xC0C1C2C3, 0xC4C5C6C7, 0xC8C9CACB, 0xCCCDCECF, 0xD0D1D2D3, 0xD4D5D6D7, 0xD8D9DADB, 0xDCDDDEDF,
    0xE0E1E2E3, 0xE4E5E6E7, 0xE8E9EAEB, 0xECEDEEEF, 0xF0F1F2F3, 0xF4F5F6F7, 0xF8F9FAFB, 0xFCFDFEFF,
    0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F, 0x10111213, 0x14151617, 0x18191A1B, 0x1C1D1E1F,
    0x20212223, 0x24252627, 0x28292A2B, 0x2C2D2E2F, 0x30313233, 0x34353637, 0x38393A3B, 0x3C3D3E3F,
    0x40414243, 0x44454647, 0x48494A4B, 0x4C4D4E4F, 0x50515253, 0x54555657, 0x58595A5B, 0x5C5D5E5F,
    0x60616263, 0x64656667, 0x68696A6B, 0x6C6D6E6F, 0x70717273, 0x74757677, 0x78797A7B, 0x7C7D7E7F,
    0x80818283, 0x84858687, 0x88898A8B, 0x8C8D8E8F, 0x90919293, 0x94959697, 0x98999A9B, 0x9C9D9E9F,
    0xA0A1A2A3, 0xA4A5A6A7, 0xA8A9AAAB, 0xACADAEAF, 0xB0B1B2B3, 0xB4B5B6B7, 0xB8B9BABB, 0xBCBDBEBF,
    0xC0C1C2C3, 0xC4C5C6C7, 0xC8C9CACB, 0xCCCDCECF, 0xD0D1D2D3, 0xD4D5D6D7, 0xD8D9DADB, 0xDCDDDEDF,
    0xE0E1E2E3, 0xE4E5E6E7, 0xE8E9EAEB, 0xECEDEEEF, 0xF0F1F2F3, 0xF4F5F6F7, 0xF8F9FAFB, 0xFCFDFEFF,
    0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F, 0x10111213, 0x14151617, 0x18191A1B, 0x1C1D1E1F,
    0x20212223, 0x24252627, 0x28292A2B, 0x2C2D2E2F, 0x30313233, 0x34353637, 0x38393A3B, 0x3C3D3E3F,
    0x40414243, 0x44454647, 0x48494A4B, 0x4C4D4E4F, 0x50515253, 0x54555657, 0x58595A5B, 0x5C5D5E5F,
    0x60616263, 0x64656667, 0x68696A6B, 0x6C6D6E6F, 0x70717273, 0x74757677, 0x78797A7B, 0x7C7D7E7F,
    0x80818283, 0x84858687, 0x88898A8B, 0x8C8D8E8F, 0x90919293, 0x94959697, 0x98999A9B, 0x9C9D9E9F,
    0xA0A1A2A3, 0xA4A5A6A7, 0xA8A9AAAB, 0xACADAEAF, 0xB0B1B2B3, 0xB4B5B6B7, 0xB8B9BABB, 0xBCBDBEBF,
    0xC0C1C2C3, 0xC4C5C6C7, 0xC8C9CACB, 0xCCCDCECF, 0xD0D1D2D3, 0xD4D5D6D7, 0xD8D9DADB, 0xDCDDDEDF,
    0xE0E1E2E3, 0xE4E5E6E7, 0xE8E9EAEB, 0xECEDEEEF, 0xF0F1F2F3, 0xF4F5F6F7, 0xF8F9FAFB, 0xFCFDFEFF,
};

void cache_enable(void);
void mpu_config(void);
void dma_config(void);
void nvic_config(void);
ErrStatus memory_compare(uint32_t *src, uint32_t *dst, uint32_t length);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/

int main(void)
{
    static uint32_t i;
    static uint32_t temp;

    /* enable the CPU cache */
    cache_enable();
    /* initialize the LEDs, and key */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_key_init(KEY_TAMPER, KEY_MODE_GPIO);

    /* configure MPU */
    mpu_config();
    /* configure DMA and NVIC */
    dma_config();
    nvic_config();
    /* first access, on misses */
    for(i = 0; i < DATANUM; i++) {
        temp += buf_dst[i];
    }

    /* enable DMA transfer */
    dma_channel_enable(DMA0, DMA_CH1);
    /* wait until transfer is finished */
    while(0 == trans_end){
    }

    if(ERROR == memory_compare((uint32_t *)buf_src, buf_dst, DATANUM)) {
        gd_eval_led_on(LED1);
    } else {
        gd_eval_led_off(LED1);
    }

    /* wait until key is pressed and released */
    while(SET == gd_eval_key_state_get(KEY_TAMPER)) {
    }

    /* clean and invalidate buffer in D-Cache */
    SCB_CleanInvalidateDCache_by_Addr(buf_dst, DATANUM * 4);

    if(ERROR != memory_compare((uint32_t *)buf_src, buf_dst, DATANUM)) {
        gd_eval_led_on(LED2);
    } else {
        gd_eval_led_off(LED2);
    }

    while(1) {
    }
}

/*!
    \brief      enable the CPU cache
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
    \brief      configure the MPU attributes
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

    /* configure the MPU attributes for AXI SRAM */
    mpu_init_struct.region_base_address  = AXI_SRAM_ADDR;
    mpu_init_struct.region_size          = MPU_REGION_SIZE_512KB;
    mpu_init_struct.access_permission    = MPU_AP_FULL_ACCESS;
    mpu_init_struct.access_bufferable    = MPU_ACCESS_BUFFERABLE;
    mpu_init_struct.access_cacheable     = MPU_ACCESS_CACHEABLE;
    mpu_init_struct.access_shareable     = MPU_ACCESS_NON_SHAREABLE;
    mpu_init_struct.region_number        = MPU_REGION_NUMBER1;
    mpu_init_struct.subregion_disable    = MPU_SUBREGION_ENABLE;
    mpu_init_struct.instruction_exec     = MPU_INSTRUCTION_EXEC_PERMIT;
    mpu_init_struct.tex_type             = MPU_TEX_TYPE0;
    mpu_region_config(&mpu_init_struct);
    mpu_region_enable();

    /* enable the MPU */
    ARM_MPU_Enable(MPU_MODE_PRIV_DEFAULT);
}

/*!
    \brief      configure the DMA
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dma_config(void)
{
    dma_single_data_parameter_struct dma_init_struct;

    rcu_periph_clock_enable(RCU_DMA0);
    /* initialize DMA0 channel 1 */
    dma_deinit(DMA0, DMA_CH1);
    dma_single_data_para_struct_init(&dma_init_struct);

    dma_init_struct.request             = DMA_REQUEST_M2M;
    dma_init_struct.direction           = DMA_MEMORY_TO_MEMORY;
    dma_init_struct.memory0_addr        = (uint32_t)buf_dst;
    dma_init_struct.memory_inc          = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_32BIT;
    dma_init_struct.number              = DATANUM;
    dma_init_struct.periph_addr         = (uint32_t)buf_src;
    dma_init_struct.periph_inc          = DMA_PERIPH_INCREASE_ENABLE;
    dma_init_struct.priority            = DMA_PRIORITY_ULTRA_HIGH;
    dma_single_data_mode_init(DMA0, DMA_CH1, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH1);
    dma_interrupt_enable(DMA0, DMA_CH1, DMA_INT_FTF);
}

/*!
    \brief      configure the NVIC
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_config(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);
    nvic_irq_enable(DMA0_Channel1_IRQn, 0, 0);
}

/*!
    \brief      memory compare function
    \param[in]  src: source data pointer
    \param[in]  dst: destination data pointer
    \param[in]  length: the compare data length
    \param[out] none
    \retval     ErrStatus: ERROR or SUCCESS
*/
ErrStatus memory_compare(uint32_t *src, uint32_t *dst, uint32_t length)
{
    while(length--) {
        if(*src++ != *dst++) {
            return ERROR;
        }
    }
    return SUCCESS;
}
