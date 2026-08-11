/*!
    \file    main.c
    \brief   DMA reload transfer number by EXTI interrupt in discontinuous mode demo

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
#include "main.h"
#include "string.h"

#define USART0_DATA_ADDRESS       (uint32_t)&USART_TDATA(USART0)

__attribute__((aligned(32))) uint8_t welcome[BUFFER_SIZE] = "Hi, this is an example: Reload_exti !\n";
__IO uint32_t arr_num = 0;
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
    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA0);
    rcu_periph_clock_enable(RCU_DMAMUX);
    /* USART configuration */
    gd_eval_com_init(EVAL_COM);
    /* USART DMA enable for transmission */
    usart_dma_transmit_config(EVAL_COM, USART_TRANSMIT_DMA_ENABLE);
    /* configure DMA interrupt */
    nvic_config();
    /* initialize LED2 */
    led_config();
    /* configure WAKEUP key */
    gd_eval_key_init(KEY_WAKEUP, KEY_MODE_EXTI);
    arr_num = strlen((char *)welcome);
    strcpy((char *)welcome, "Hi, this is an example: Reload_exti !\n");
    /* clean the cache line */
    SCB_CleanDCache_by_Addr((uint32_t *)welcome, BUFFER_SIZE);
    /* configure DMA channel 0 */
    dma_config();

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
    /* turn off LED2 */
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
    nvic_irq_enable(DMA0_Channel0_IRQn, 1U, 0U);
}

/*!
    \brief      configure the DMA channel
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dma_config(void)
{
    dma_single_data_parameter_struct  dma_init_struct;

    /* initialize DMA0 channel 0 */
    dma_deinit(DMA0, DMA_CH0);
    dma_single_data_para_struct_init(&dma_init_struct);
    dma_init_struct.request             = DMA_REQUEST_USART0_TX;
    dma_init_struct.direction           = DMA_MEMORY_TO_PERIPH;
    dma_init_struct.memory0_addr        = (uint32_t)welcome;
    dma_init_struct.memory_inc          = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.periph_memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number              = arr_num;
    dma_init_struct.periph_addr         = USART0_DATA_ADDRESS;
    dma_init_struct.periph_inc          = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.priority            = DMA_PRIORITY_ULTRA_HIGH;
    dma_single_data_mode_init(DMA0, DMA_CH0, &dma_init_struct);

    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH0);
    /* disable the DMAMUX_MUXCH0 synchronization mode */
    dmamux_synchronization_disable(DMAMUX_MUXCH0);

    /* enable DMA transfer complete interrupt */
    dma_interrupt_enable(DMA0, DMA_CH0, DMA_INT_FTF);
    /* enable DMA channel 0 */
    dma_channel_enable(DMA0, DMA_CH0);
}
