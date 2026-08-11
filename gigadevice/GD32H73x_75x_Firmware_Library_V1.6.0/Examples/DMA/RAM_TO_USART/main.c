/*!
    \file    main.c
    \brief   usart dma demo

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
#include "gd32h73x_75x_it.h"
#include "string.h"

#define USART0_DATA_ADDRESS       ((uint32_t)&USART_TDATA(USART0))
#define ARRAYNUM(arr_nanme)       (uint32_t)(sizeof(arr_nanme) / sizeof(*(arr_nanme)))
#define BUFFER_SIZE               64

__attribute__((aligned(32))) uint8_t welcome[BUFFER_SIZE] = "Hi, this is an example: RAM_TO_USART by DMA !\n";
__IO FlagStatus g_transfer_complete = RESET;

void cache_enable(void);
void led_config(void);
void nvic_config(void);
void dma_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* enable the cpu chache */
    cache_enable();
    /* enable DMA1 clock */
    rcu_periph_clock_enable(RCU_DMA1);
    /* enable DMAMUX clock */
    rcu_periph_clock_enable(RCU_DMAMUX);
    /* initialize USART */
    gd_eval_com_init(EVAL_COM);
    /* enable USART DMA */
    usart_dma_transmit_config(USART0, USART_TRANSMIT_DMA_ENABLE);
    /* initialize LED2 */
    led_config();
    /* configure DMA1 interrupt */
    nvic_config();
    /* clean the cache lines */
    SCB_CleanDCache_by_Addr((uint32_t *)welcome, BUFFER_SIZE);
    /* configure DMA1 */
    dma_config();

    /* waiting for the transfer to complete */
    while(RESET == g_transfer_complete) {
    }
    /* light LED2 */
    gd_eval_led_on(LED2);

    while(1) {
    }
}

/*!
    \brief      enable the cpu cache
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
    \brief      LED2 configuration
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_config(void)
{
    gd_eval_led_init(LED2);
    /* LED2 off */
    gd_eval_led_off(LED2);
}

/*!
    \brief      configure DMA interrupt
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_config(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);
    nvic_irq_enable(DMA1_Channel7_IRQn, 0, 0);
}

/*!
    \brief      configure DMA
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dma_config(void)
{
    dma_single_data_parameter_struct dma_init_struct;

    /* deinitialize DMA1 channel7(USART0 tx) */
    dma_deinit(DMA1, DMA_CH7);
    dma_single_data_para_struct_init(&dma_init_struct);

    dma_init_struct.request             = DMA_REQUEST_USART0_TX;
    dma_init_struct.direction           = DMA_MEMORY_TO_PERIPH;
    dma_init_struct.memory0_addr        = (uint32_t)welcome;
    dma_init_struct.memory_inc          = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
    dma_init_struct.number              = strlen((char *)welcome);
    dma_init_struct.periph_addr         = USART0_DATA_ADDRESS;
    dma_init_struct.periph_inc          = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.priority            = DMA_PRIORITY_ULTRA_HIGH;
    dma_single_data_mode_init(DMA1, DMA_CH7, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA1, DMA_CH7);
    /* enable DMA1 channel7 transfer complete interrupt */
    dma_interrupt_enable(DMA1, DMA_CH7, DMA_CHXCTL_FTFIE);
    /* enable DMA1 channel7 */
    dma_channel_enable(DMA1, DMA_CH7);
}
