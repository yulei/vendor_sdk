/*!
    \file    main.c
    \brief   DMA transfer data from RAM to RAM

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

#define BUFFER_SIZE     (256U)

void cache_enable(void);
void fill_buffer_size(void);
void dma_config(void);
ErrStatus memory_compare(uint16_t *src, uint16_t *dst, uint16_t length);

__attribute__((aligned(32))) uint16_t source[BUFFER_SIZE];
__attribute__((aligned(32))) uint16_t destination[BUFFER_SIZE];
ErrStatus status;

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* enable the CPU chache */
    cache_enable();
    /* initialize LED */
    gd_eval_led_init(LED1);
    /* fill the buffer with specified value */
    fill_buffer_size();
    /* clean the cache line */
    SCB_CleanDCache_by_Addr((uint32_t *)source, BUFFER_SIZE * 2);
    
    /* configure DMA channels */
    dma_config();
    while(!dma_flag_get(DMA1, DMA_CH0, DMA_INTF_FTFIF)) {
    }
    dma_flag_clear(DMA1, DMA_CH0, DMA_INTF_FTFIF);
    /* invalidate the cache line */
    SCB_InvalidateDCache_by_Addr((uint32_t *)destination, BUFFER_SIZE * 2);
    status = memory_compare((uint16_t *)source, (uint16_t *)destination, BUFFER_SIZE);

    if(SUCCESS == status) {
        gd_eval_led_on(LED1);
    } else {
        gd_eval_led_off(LED1);
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
    /* enable i-cache */
    SCB_EnableICache();

    /* enable d-cache */
    SCB_EnableDCache();
}

/*!
    \brief      fill the buffer with specified value
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fill_buffer_size(void)
{
    uint16_t i;

    for(i = 0U; i < BUFFER_SIZE; i++) {
        source[i] = i;
    }
}

/*!
    \brief      configure the DMA channel
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dma_config(void)
{
    dma_multi_data_parameter_struct dma_init_parameter;

    /* peripheral clock enable */
    rcu_periph_clock_enable(RCU_DMA1);

    dma_deinit(DMA1, DMA_CH0);
    dma_multi_data_para_struct_init(&dma_init_parameter);
    dma_init_parameter.periph_addr        = (uint32_t)source;
    dma_init_parameter.periph_width       = DMA_PERIPH_WIDTH_16BIT;
    dma_init_parameter.periph_inc         = DMA_PERIPH_INCREASE_ENABLE;
    dma_init_parameter.memory0_addr       = (uint32_t)destination;
    dma_init_parameter.memory_width       = DMA_MEMORY_WIDTH_16BIT;
    dma_init_parameter.memory_inc         = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_parameter.memory_burst_width = DMA_MEMORY_BURST_4_BEAT;
    dma_init_parameter.periph_burst_width = DMA_PERIPH_BURST_4_BEAT;
    dma_init_parameter.critical_value     = DMA_FIFO_2_WORD;
    dma_init_parameter.circular_mode      = DMA_CIRCULAR_MODE_DISABLE;
    dma_init_parameter.direction          = DMA_MEMORY_TO_MEMORY;
    dma_init_parameter.number             = BUFFER_SIZE;
    dma_init_parameter.priority           = DMA_PRIORITY_ULTRA_HIGH;
    dma_multi_data_mode_init(DMA1, DMA_CH0, &dma_init_parameter);
    dma_channel_enable(DMA1, DMA_CH0);
}

/*!
    \brief      memory compare function
    \param[in]  src : source data
    \param[in]  dst : destination data
    \param[in]  length : the compare data length
    \param[out] none
    \retval     ErrStatus : ERROR or SUCCESS
*/
ErrStatus memory_compare(uint16_t *src, uint16_t *dst, uint16_t length)
{
    while(length--) {
        if(*src++ != *dst++) {
            return ERROR;
        }
    }
    return SUCCESS;
}
