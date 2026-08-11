/*!
    \file    main.c
    \brief   ADC1 resolution oversample

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

#define BOARD_ADC_CHANNEL ADC_CHANNEL_15
#define ADC_GPIO_PORT GPIOA
#define ADC_GPIO_PIN GPIO_PIN_3

/* configure the different system clocks */
void rcu_config(void);
/* configure the GPIO peripheral */
void gpio_config(void);
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
    /* ADC configuration */
    adc_config();

    while(1) {
        /* ADC resolusion 14B */
        printf("\r\n resolusion 14B:\r\n");
        adc_resolution_config(ADC1, ADC_RESOLUTION_14B);
        adc_oversample_mode_config(ADC1, ADC_OVERSAMPLING_ALL_CONVERT, ADC_OVERSAMPLING_SHIFT_NONE, 0);
        adc_oversample_mode_disable(ADC1);
        /* 1 time sample */
        adc_enable(ADC1);
        adc_software_trigger_enable(ADC1, ADC_ROUTINE_CHANNEL);
        while(RESET == adc_flag_get(ADC1, ADC_FLAG_EOC));
        delay_1ms(500);
        printf(" 1 time sample,the data is %d\r\n", ADC_RDATA(ADC1));
        adc_disable(ADC1);

        /* 16 times sample ,no shift */
        adc_oversample_mode_config(ADC1, ADC_OVERSAMPLING_ALL_CONVERT, ADC_OVERSAMPLING_SHIFT_NONE, 15);
        adc_oversample_mode_enable(ADC1);
        adc_enable(ADC1);
        adc_software_trigger_enable(ADC1, ADC_ROUTINE_CHANNEL);
        while(RESET == adc_flag_get(ADC1, ADC_FLAG_EOC));
        delay_1ms(500);
        printf(" 16 times sample,no shift,the data is %d\r\n", ADC_RDATA(ADC1));
        adc_disable(ADC1);

        /* 16 times sample ,4 bits shift */
        adc_oversample_mode_config(ADC1, ADC_OVERSAMPLING_ALL_CONVERT, ADC_OVERSAMPLING_SHIFT_4B, 15);
        adc_enable(ADC1);
        adc_software_trigger_enable(ADC1, ADC_ROUTINE_CHANNEL);
        while(RESET == adc_flag_get(ADC1, ADC_FLAG_EOC));
        delay_1ms(500);
        printf(" 16 times sample,4 bits shift,the data is %d\r\n", ADC_RDATA(ADC1));
        adc_disable(ADC1);

        /* ADC resolusion 10B */
        printf("\r\n resolusion 10B:\r\n");
        adc_resolution_config(ADC1, ADC_RESOLUTION_10B);
        adc_oversample_mode_config(ADC1, ADC_OVERSAMPLING_ALL_CONVERT, ADC_OVERSAMPLING_SHIFT_NONE, 0);
        adc_oversample_mode_disable(ADC1);
        /* 1 time sample */
        adc_enable(ADC1);
        adc_software_trigger_enable(ADC1, ADC_ROUTINE_CHANNEL);
        while(RESET == adc_flag_get(ADC1, ADC_FLAG_EOC));
        delay_1ms(500);
        printf(" 1 time sample,the data is %d\r\n", ADC_RDATA(ADC1));
        adc_disable(ADC1);

        /* 16 times sample ,no shift */
        adc_oversample_mode_config(ADC1, ADC_OVERSAMPLING_ALL_CONVERT, ADC_OVERSAMPLING_SHIFT_NONE, 15);
        adc_oversample_mode_enable(ADC1);
        adc_enable(ADC1);
        adc_software_trigger_enable(ADC1, ADC_ROUTINE_CHANNEL);
        while(RESET == adc_flag_get(ADC1, ADC_FLAG_EOC));
        delay_1ms(500);
        printf(" 16 times sample,no shift,the data is %d\r\n", ADC_RDATA(ADC1));
        adc_disable(ADC1);

        /* 16 times sample ,4 bits shift */
        adc_oversample_mode_config(ADC1, ADC_OVERSAMPLING_ALL_CONVERT, ADC_OVERSAMPLING_SHIFT_4B, 15);
        adc_enable(ADC1);
        adc_software_trigger_enable(ADC1, ADC_ROUTINE_CHANNEL);
        while(RESET == adc_flag_get(ADC1, ADC_FLAG_EOC));
        delay_1ms(500);
        printf(" 16 times sample,4 bits shift,the data is %d\r\n", ADC_RDATA(ADC1));
        adc_disable(ADC1);
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
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC0);
    rcu_periph_clock_enable(RCU_ADC1);
}

/*!
    \brief      configure the GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* config the GPIO as analog mode */
    gpio_mode_set(ADC_GPIO_PORT, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, ADC_GPIO_PIN);
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
    adc_deinit(ADC1);
    /* ADC clock config */
    adc_clock_config(ADC0, ADC_CLK_SYNC_HCLK_DIV6);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC1, ADC_DATAALIGN_RIGHT);
    /* ADC channel length config */
    adc_channel_length_config(ADC1, ADC_ROUTINE_CHANNEL, 1);
    /* ADC routine channel config */
    adc_routine_channel_config(ADC1, 0, BOARD_ADC_CHANNEL, 240);
    /* ADC trigger config */
    adc_external_trigger_config(ADC1, ADC_ROUTINE_CHANNEL, EXTERNAL_TRIGGER_DISABLE);

    /* enable ADC interface */
    adc_enable(ADC1);
    /* wait for ADC stability */
    delay_1ms(1);
    /* ADC calibration mode config */
    adc_calibration_mode_config(ADC1, ADC_CALIBRATION_OFFSET);
    /* ADC calibration number config */
    adc_calibration_number(ADC1, ADC_CALIBRATION_NUM1);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC1);
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
