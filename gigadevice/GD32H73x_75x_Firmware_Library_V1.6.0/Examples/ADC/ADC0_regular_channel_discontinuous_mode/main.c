/*!
    \file    main.c
    \brief   ADC0_regular_channel_discontinuous_mode

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
#include "systick.h"
#include <stdio.h>

#define BUFFER_SIZE           32
__attribute__((aligned(32))) uint32_t adc_value[8];

/* configure the different system clocks */
void rcu_config(void);
/* configure the GPIO peripheral */
void gpio_config(void);
/* configure the DMA peripheral */
void dma_config(void);
/* configure the ADC peripheral */
void adc_config(void);
/* enable the CPU cache */
void cache_enable(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* enable the CPU cache */
    cache_enable();
    /* system clocks configuration */
    rcu_config();
    /* systick configuration */
    systick_config();
    /* GPIO configuration */
    gpio_config();
    /* configure COM port */
    gd_eval_com_init(EVAL_COM);
    /* DMA configuration */
    dma_config();
    /* ADC configuration */
    adc_config();

    while(1) {
        delay_1ms(1000);
        /* D-Cache Invalidate by address */
        SCB_InvalidateDCache_by_Addr((uint32_t *)adc_value, BUFFER_SIZE);
        printf("\r\n ADC0 routine channel discontinuous mode 1st data: \r\n");
        printf("\r\n ADC0 routine channel 0 data = %d \r\n", adc_value[0]);
        printf("\r\n ADC0 routine channel 1 data = %d \r\n", adc_value[1]);
        printf("\r\n ADC0 routine channel 2 data = %d \r\n", adc_value[2]);
        printf("\r\n ADC0 routine channel discontinuous mode 2st data: \r\n");
        printf("\r\n ADC0 routine channel 3 data = %d \r\n", adc_value[3]);
        printf("\r\n ADC0 routine channel 4 data = %d \r\n", adc_value[4]);
        printf("\r\n ADC0 routine channel 5 data = %d \r\n", adc_value[5]);
        printf("\r\n ADC0 routine channel discontinuous mode 3st data: \r\n");
        printf("\r\n ADC0 routine channel 6 data = %d \r\n", adc_value[6]);
        printf("\r\n ADC0 routine channel 7 data = %d \r\n", adc_value[7]);
        printf("\r\n ***********************************\r\n");
    }
}

/*!
    \brief      configure the different system clocks
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOC);
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC0);
    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA0);
    rcu_periph_clock_enable(RCU_DMAMUX);
    /* config TRIGSEL clock */
    rcu_periph_clock_enable(RCU_TRIGSEL);
}

/*!
    \brief      configure the GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* config the GPIO as analog mode, for ADC */
    gpio_mode_set(GPIOC, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_0);
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE,
                  GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
    /* config the GPIO as TRIGSEL */
    gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_0);
    gpio_af_set(GPIOA, GPIO_AF_13, GPIO_PIN_0);
}

/*!
    \brief      configure the DMA peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dma_config(void)
{
    /* ADC_DMA_channel configuration */
    dma_single_data_parameter_struct dma_single_data_parameter;

    /* ADC DMA_channel configuration */
    dma_deinit(DMA0, DMA_CH0);

    /* initialize DMA single data mode */
    dma_single_data_parameter.request = DMA_REQUEST_ADC0;
    dma_single_data_parameter.periph_addr = (uint32_t)(&ADC_RDATA(ADC0));
    dma_single_data_parameter.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_single_data_parameter.memory0_addr = (uint32_t)(&adc_value);
    dma_single_data_parameter.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_single_data_parameter.periph_memory_width = DMA_PERIPH_WIDTH_32BIT;
    dma_single_data_parameter.direction = DMA_PERIPH_TO_MEMORY;
    dma_single_data_parameter.number = 8U;
    dma_single_data_parameter.priority = DMA_PRIORITY_HIGH;
    dma_single_data_mode_init(DMA0, DMA_CH0, &dma_single_data_parameter);

    /* enable DMA circulation mode */
    dma_circulation_enable(DMA0, DMA_CH0);

    /* enable DMA channel */
    dma_channel_enable(DMA0, DMA_CH0);
}

/*!
    \brief      configure the ADC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_config(void)
{
    /* reset ADC */
    adc_deinit(ADC0);
    /* ADC clock config */
    adc_clock_config(ADC0, ADC_CLK_SYNC_HCLK_DIV6);
    /* ADC mode config */
    adc_sync_mode_config(ADC_SYNC_MODE_INDEPENDENT);
    /* ADC continous function enable */
    adc_special_function_config(ADC0, ADC_SCAN_MODE, ENABLE);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);

    /* ADC channel length config */
    adc_channel_length_config(ADC0, ADC_ROUTINE_CHANNEL, 8);
    /* ADC routine channel config */
    adc_routine_channel_config(ADC0, 0, ADC_CHANNEL_10, 240);
    adc_routine_channel_config(ADC0, 1, ADC_CHANNEL_17, 240);
    adc_routine_channel_config(ADC0, 2, ADC_CHANNEL_14, 240);
    adc_routine_channel_config(ADC0, 3, ADC_CHANNEL_15, 240);
    adc_routine_channel_config(ADC0, 4, ADC_CHANNEL_18, 240);
    adc_routine_channel_config(ADC0, 5, ADC_CHANNEL_19, 240);
    adc_routine_channel_config(ADC0, 6, ADC_CHANNEL_3, 240);
    adc_routine_channel_config(ADC0, 7, ADC_CHANNEL_7, 240);
    /* ADC trigger config */
    trigsel_init(TRIGSEL_OUTPUT_ADC0_ROUTRG, TRIGSEL_INPUT_TRIGSEL_IN0);
    adc_external_trigger_config(ADC0, ADC_ROUTINE_CHANNEL, EXTERNAL_TRIGGER_RISING);
    /* ADC discontinuous mode */
    adc_discontinuous_mode_config(ADC0, ADC_ROUTINE_CHANNEL, 3);
    /* ADC DMA function enable */
    adc_dma_request_after_last_enable(ADC0);
    adc_dma_mode_enable(ADC0);


    /* enable ADC interface */
    adc_enable(ADC0);
    /* wait for ADC stability */
    delay_1ms(1);
    /* ADC calibration mode config */
    adc_calibration_mode_config(ADC0, ADC_CALIBRATION_OFFSET);
    /* ADC calibration number config */
    adc_calibration_number(ADC0, ADC_CALIBRATION_NUM1);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC0);
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
