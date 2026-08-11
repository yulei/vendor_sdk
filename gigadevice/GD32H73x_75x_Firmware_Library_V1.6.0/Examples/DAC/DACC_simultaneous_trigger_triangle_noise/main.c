/*!
    \file    main.c
    \brief   DAC concurrent mode simultaneous trigger with same triangle wave demo
    
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

/* configure RCU peripheral */
void rcu_config(void);
/* configure GPIO peripheral */
void gpio_config(void);
/* configure TRIGSEL peripheral */
void triggersel_config(void);
/* configure DAC peripheral */
void dac_config(void);
/* configure TIMER peripheral */
void timer5_config(void);
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
    /* enable the CPU Chache */
    cache_enable();
    /* configure RCU peripheral */
    rcu_config();
    /* configure GPIO peripheral */
    gpio_config();
    /* configure TRIGSEL peripheral */
    triggersel_config();
    /* configure DAC peripheral */
    dac_config();
    /* configure TIMER peripheral */
    timer5_config();

    while(1){
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
    \brief      configure RCU peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable GPIOA clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* enable DAC clock */
    rcu_periph_clock_enable(RCU_DAC);
    /* enable TRIGSEL clock */
    rcu_periph_clock_enable(RCU_TRIGSEL);
    /* enable TIMER clock */
    rcu_periph_clock_enable(RCU_TIMER5);
}

/*!
    \brief      configure GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* configure PA4 and PA5 as DAC output */
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_4| GPIO_PIN_5);
}

/*!
    \brief      configure the TRIGSEL
    \param[in]  none
    \param[out] none
    \retval     none
*/
void triggersel_config(void)
{
    trigsel_init(TRIGSEL_OUTPUT_DAC0_OUT0_EXTRG, TRIGSEL_INPUT_TIMER5_TRGO0);
    trigsel_init(TRIGSEL_OUTPUT_DAC0_OUT1_EXTRG, TRIGSEL_INPUT_TIMER5_TRGO0);
}

/*!
    \brief      configure DAC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac_config(void)
{
    /* initialize DAC */
    dac_deinit(DAC0);
    /* DAC trigger config */
    dac_trigger_source_config(DAC0, DAC_OUT0, DAC_TRIGGER_EXTERNAL);
    dac_trigger_source_config(DAC0, DAC_OUT1, DAC_TRIGGER_EXTERNAL);
    /* DAC trigger enable */
    dac_trigger_enable(DAC0, DAC_OUT0);
    dac_trigger_enable(DAC0, DAC_OUT1);
    /* DAC wave mode config */
    dac_wave_mode_config(DAC0, DAC_OUT0, DAC_WAVE_MODE_TRIANGLE);
    dac_triangle_noise_config(DAC0, DAC_OUT0, DAC_TRIANGLE_AMPLITUDE_1023);
    dac_mode_config(DAC0, DAC_OUT0, NORMAL_PIN_BUFFON);
    dac_wave_mode_config(DAC0, DAC_OUT1, DAC_WAVE_MODE_TRIANGLE);
    dac_triangle_noise_config(DAC0, DAC_OUT1, DAC_TRIANGLE_AMPLITUDE_1023);
    dac_mode_config(DAC0, DAC_OUT1, NORMAL_PIN_BUFFON);

    /* DAC enable */
    dac_concurrent_enable(DAC0);
    dac_concurrent_data_set(DAC0, DAC_ALIGN_12B_R, 0x7F0, 0x1F0);
}

/*!
    \brief       configure TIMER5 peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer5_config(void)
{
    timer_parameter_struct timer_initpara;

    /* TIMER deinitialize */
    timer_deinit(TIMER5);

    /* TIMER configuration */
    timer_struct_para_init(&timer_initpara);
    timer_initpara.prescaler         = 299;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;

    /* initialize TIMER init parameter struct */
    timer_init(TIMER5, &timer_initpara);

    /* TIMER master mode output trigger source: Update event */
    timer_master_output0_trigger_source_select(TIMER5, TIMER_TRI_OUT0_SRC_UPDATE);

    /* enable TIMER */
    timer_enable(TIMER5);
}
