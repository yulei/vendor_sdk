/*!
    \file    main.c
    \brief   TIMER0 output composite PWM and output match pulse demo for GD32H73x_75x
    
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

/* configure the TIMER peripheral */
void timer_config(void);
/* configure the GPIO ports */
void gpio_config(void);
/* enable the CPU cache */
void cache_enable(void);

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
    \brief      configure the GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);

    /*configure PA8(TIMER0 CH0) as alternate function*/
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_8);
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_8);

    /*configure PA9(TIMER0 CH1) as alternate function*/
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_9);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_9);
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_9);

    /*configure PA10(TIMER0 CH2) as alternate function*/
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_10);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_10);
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_10);
}

/*!
    \brief      configure the TIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer_config(void)
{
    /* -----------------------------------------------------------------------
    TIMER0 configuration: 
    - CH0, CH1 and CH2 output composite PWM waveform.
    - CH2 enable the output match pulse function.
    TIMER0 frequency = CK_TIMER/ (299+1) = 1MHz. The PWM frequency is 1MHz/ (99+1) = 10KHz.
    - CH0 configuration in composite PWM mode, the high level duty cycle 80% (100%-(60-40)/100*100%).
    - CH1 configuration in composite PWM mode, the high level duty cycle 20% ((60-40)/100*100%).
    - CH2 will output 2 pulses when the matching event occurs.
    ----------------------------------------------------------------------- */
    timer_oc_parameter_struct timer_ocinitpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER0);
    timer_deinit(TIMER0);

    /* TIMER0 configuration */
    timer_initpara.prescaler         = 299;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 99;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER0, &timer_initpara);

    /* TIMER0 CH1/CH1/CH2 output configuration */
    timer_ocinitpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocinitpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocinitpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocinitpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocinitpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocinitpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    timer_channel_output_config(TIMER0, TIMER_CH_0, &timer_ocinitpara);
    timer_channel_output_config(TIMER0, TIMER_CH_1, &timer_ocinitpara);
    timer_channel_output_config(TIMER0, TIMER_CH_2, &timer_ocinitpara);

    /* CH0 configuration in PWM mode 0 */
    timer_channel_output_mode_config(TIMER0, TIMER_CH_0, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);
    /* CH0 forced low when the counter matches 40 */
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, 40);
    /* CH0 forced high when the counter matches 60 */
    timer_channel_additional_compare_value_config(TIMER0, TIMER_CH_0, 60);
    /* CH0 configuration in composite PWM mode, high level duty cycle 80%(100%-(60-40)/100*100%) */
    timer_channel_composite_pwm_mode_config(TIMER0, TIMER_CH_0, ENABLE);

    /* CH1 configuration in PWM mode 1 */
    timer_channel_output_mode_config(TIMER0, TIMER_CH_1, TIMER_OC_MODE_PWM1);
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_1, TIMER_OC_SHADOW_DISABLE);
    /* CH1 forced high when the counter matches 40 */
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_1, 40);
    /* CH1 forced low when the counter matches 60 */
    timer_channel_additional_compare_value_config(TIMER0, TIMER_CH_1, 60);
    /* CH1 configuration in composite PWM mode, high level duty cycle 20%((60-40)/100*100%) */
    timer_channel_composite_pwm_mode_config(TIMER0, TIMER_CH_1, ENABLE);

    /* CH2 configuration in PWM mode 0 */
    timer_channel_output_mode_config(TIMER0, TIMER_CH_2, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_2, TIMER_OC_SHADOW_DISABLE);
    /* CH2 forced high when the counter matches 40 */
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_2, 40);
    /* CH2 forced low when the counter matches 60 */
    timer_channel_additional_compare_value_config(TIMER0, TIMER_CH_2, 60);
    /* CH2 configuration in composite PWM mode, high level duty cycle 80%(100%-(60-40)/100*100%) */
    timer_channel_composite_pwm_mode_config(TIMER0, TIMER_CH_2, ENABLE);
    /* CH2 will output 2 pulses when the matche event occurs */
    timer_output_match_pulse_select(TIMER0, TIMER_CH_2, TIMER_PULSE_OUTPUT_CNT_UP);
    
    timer_primary_output_config(TIMER0, ENABLE);
    /* auto-reload preload enable */
    timer_enable(TIMER0);
}

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
    /* configure the GPIO ports */
    gpio_config();
    /* configure the TIMER peripheral */
    timer_config();

    while(1);
}
