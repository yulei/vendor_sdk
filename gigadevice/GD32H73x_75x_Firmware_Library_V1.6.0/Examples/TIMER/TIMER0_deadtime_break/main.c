/*!
    \file    main.c
    \brief   TIMER0 deadtime break demo for GD32H73x_75x
    
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

/* configure the GPIO ports */
void gpio_config(void);
/* configure the TRIGSEL peripheral */
void trigsel_config(void);
/* configure the TIMER peripheral */
void timer_config(void);
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
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOE);

    /*configure PA8/PA7(TIMER0 CH0/MCH0) as alternate function*/
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_7 | GPIO_PIN_8);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_7 | GPIO_PIN_8);
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_7 | GPIO_PIN_8);

    /*configure PA9/PB0(TIMER0 CH1/MCH1) as alternate function*/
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_9);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_9);
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_9);

    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_0);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_0);
    gpio_af_set(GPIOB, GPIO_AF_1, GPIO_PIN_0);

    /*configure PA6(TIMER0 BRKIN0) as alternate function*/
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6);
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_6);
    
    /*configure PE4(TIMER0 BRKIN1) as alternate function*/
    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4);
    gpio_af_set(GPIOE, GPIO_AF_1, GPIO_PIN_4);
}

/*!
    \brief      configure the TRIGSEL peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void trigsel_config(void)
{
    /* enable TRIGSEL clock */
    rcu_periph_clock_enable(RCU_TRIGSEL);
    /* select TIMER0_BRKIN0 to trigger TIMER0_BRKIN0 pin */
    trigsel_init(TRIGSEL_OUTPUT_TIMER0_BRKIN0, TRIGSEL_INPUT_TIMER0_BRKIN0);
    /* select TIMER0_BRKIN1 to trigger TIMER0_BRKIN1 pin */
    trigsel_init(TRIGSEL_OUTPUT_TIMER0_BRKIN1, TRIGSEL_INPUT_TIMER0_BRKIN1);
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
    - generate 2 complementary PWM signals.

    - TIMER0 frequency is fixed to systemcoreclock, TIMER0 prescaler is equal to
      300, so TIMER0 counter frequency is 1MHz.

    - the duty cycle is computed as the following description:
      the CH0 duty cycle is set to 40% so MCH0 is set to 60%.
      the CH1 duty cycle is set to 80% so MCH1 is set to 20%.

    - the deadtime of CH0/MCH0 and CH1/MCH1 with different values.
      CH0 and MCH0 insert a dead time equal to ((64+36)/(300MHz/4)*2 = 2.67us.
      CH1 and MCH1 insert a dead time equal to ((32+8)/(300MHz/4)*8 = 4.27us.

    - configure the break feature, BRKIN0 and BRKIN1 active at high level, and
      using the automatic output enable feature.

    - use the locking parameters level0.
    ----------------------------------------------------------------------- */
    timer_oc_parameter_struct timer_ocinitpara;
    timer_parameter_struct timer_initpara;
    timer_break_parameter_struct timer_breakpara;
    timer_free_complementary_parameter_struct timer_freecompara;

    rcu_periph_clock_enable(RCU_TIMER0);
    timer_deinit(TIMER0);

    /* TIMER0 configuration */
    timer_struct_para_init(&timer_initpara);
    timer_initpara.prescaler         = 299;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV4;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER0, &timer_initpara);

    /* CH0/1 & MCH0/1 configuration in PWM mode */
    timer_channel_output_struct_para_init(&timer_ocinitpara);
    timer_ocinitpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocinitpara.outputnstate = TIMER_CCXN_ENABLE;
    timer_ocinitpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocinitpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocinitpara.ocidlestate  = TIMER_OC_IDLE_STATE_HIGH;
    timer_ocinitpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    /* config CH0/1 & MCH0/1 as output*/
    timer_channel_output_config(TIMER0, TIMER_CH_0, &timer_ocinitpara);
    timer_channel_output_config(TIMER0, TIMER_CH_1, &timer_ocinitpara);

    /* config MCH0~3 output complementary */
    timer_multi_mode_channel_mode_config(TIMER0, TIMER_MCH_0, TIMER_MCH_MODE_COMPLEMENTARY);
    timer_multi_mode_channel_mode_config(TIMER0, TIMER_MCH_1, TIMER_MCH_MODE_COMPLEMENTARY);

    /* CH0 configuration in PWM mode 0,duty cycle 40% */
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, 400);
    timer_channel_output_mode_config(TIMER0, TIMER_CH_0, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);

    /* CH1 configuration in PWM mode 0,duty cycle 80% */
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_1, 800);
    timer_channel_output_mode_config(TIMER0, TIMER_CH_1, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_1, TIMER_OC_SHADOW_DISABLE);

    /* BREAK configuration */
    timer_break_struct_para_init(&timer_breakpara);
    timer_breakpara.runoffstate         = TIMER_ROS_STATE_ENABLE;
    timer_breakpara.ideloffstate        = TIMER_IOS_STATE_ENABLE;
    timer_breakpara.deadtime            = 0U;
    timer_breakpara.outputautostate     = TIMER_OUTAUTO_ENABLE;
    timer_breakpara.protectmode         = TIMER_CCHP_PROT_OFF;
    timer_breakpara.break0state         = TIMER_BREAK0_ENABLE;
    timer_breakpara.break0filter        = 0U;
    timer_breakpara.break0polarity      = TIMER_BREAK0_POLARITY_HIGH;
    timer_breakpara.break0lock          = TIMER_BREAK0_LK_DISABLE;
    timer_breakpara.break0release       = TIMER_BREAK0_UNRELEASE;
    timer_breakpara.break1state         = TIMER_BREAK1_ENABLE;
    timer_breakpara.break1filter        = 0U;
    timer_breakpara.break1polarity      = TIMER_BREAK1_POLARITY_HIGH;
    timer_breakpara.break1lock          = TIMER_BREAK1_LK_DISABLE;
    timer_breakpara.break1release       = TIMER_BREAK1_UNRELEASE;
    timer_break_config(TIMER0, &timer_breakpara);

    /* select TIMER0_BRKIN0 with TIMER0_BRKIN0 pin */
    timer_break_external_source_config(TIMER0, TIMER_BREAK0, TIMER_BRKIN0, ENABLE);
    timer_break_external_source_config(TIMER0, TIMER_BREAK0, TIMER_BRKIN1, DISABLE);
    timer_break_external_source_config(TIMER0, TIMER_BREAK0, TIMER_BRKIN2, DISABLE);
    /* select TIMER0_BRKIN0 will not be inverted */
    timer_break_external_polarity_config(TIMER0, TIMER_BREAK0, TIMER_BRKIN0, TIMER_BRKIN_POLARITY_LOW);
    /* select TIMER0_BRKIN1 with TIMER0_BRKIN1 pin */
    timer_break_external_source_config(TIMER0, TIMER_BREAK1, TIMER_BRKIN1, ENABLE);
    /* select TIMER0_BRKIN1 will not be inverted */
    timer_break_external_polarity_config(TIMER0, TIMER_BREAK1, TIMER_BRKIN1, TIMER_BRKIN_POLARITY_LOW);

    /* independent deadtime configuration */
    timer_free_complementary_struct_para_init(&timer_freecompara);
    timer_freecompara.freecomstate = TIMER_FCCHP_STATE_ENABLE;
    timer_freecompara.runoffstate  = TIMER_ROS_STATE_DISABLE;
    timer_freecompara.ideloffstate = TIMER_IOS_STATE_DISABLE;
    timer_freecompara.deadtime     = 164;
    timer_channel_free_complementary_config(TIMER0, TIMER_CH_0, &timer_freecompara);
    timer_freecompara.deadtime     = 200;
    timer_channel_free_complementary_config(TIMER0, TIMER_CH_1, &timer_freecompara);

    /* TIMER0 primary output function enable */
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
    /* configure the TRIGSEL peripheral */
    trigsel_config();
    /* configure the TIMER peripheral */
    timer_config();

    while(1);
}
