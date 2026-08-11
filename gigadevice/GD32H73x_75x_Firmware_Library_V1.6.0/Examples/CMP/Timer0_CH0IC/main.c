/*!
    \file    main.c
    \brief   CMP Timer0_CH0IC demo

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
#include "gd32h759i_eval.h"

/* configure RCU peripheral */
void rcu_config(void);
/* configure GPIO peripheral */
void gpio_config(void);
/* configure CMP peripheral */
void cmp_config(void);
/* configure TIMER peripheral */
void timer_config(void);
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
    /* configure systick */
    systick_config();

    /* initialize LED2 */
    gd_eval_led_init(LED2);

    /* configure RCU peripheral */
    rcu_config();
    /* configure GPIO peripheral */
    gpio_config();
    /* configure CMP peripheral */
    cmp_config();
    /* configure TIMER peripheral */
    timer_config();

    while(1) {
        if(SET == timer_flag_get(TIMER0, TIMER_FLAG_CH0)) {
            /* toggle LED */
            gd_eval_led_toggle(LED2);

            /* clear TIMER CH0 flag */
            timer_flag_clear(TIMER0, TIMER_FLAG_CH0);
        }
    }
}

/*!
    \brief      configure RCU peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable GPIOB clock */
    rcu_periph_clock_enable(RCU_GPIOB);
    /* enable SYSCFG clock */
    rcu_periph_clock_enable(RCU_SYSCFG);
    /* enable TIMER0 clock */
    rcu_periph_clock_enable(RCU_TIMER0);
    /* enable CMP clock */
    rcu_periph_clock_enable(RCU_CMP);
}

/*!
    \brief      configure GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* configure PB0 as CMP0 input */
    gpio_mode_set(GPIOB, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_0);
}

/*!
    \brief      configure CMP peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cmp_config(void)
{
    /* initialize CMP0 */
    cmp_deinit(CMP0);
    /* when using VREFINT as inverting input, the voltage bridge and voltage scaler should be enable */
    cmp_voltage_scaler_enable(CMP0);
    cmp_scaler_bridge_enable(CMP0);

    /* configure CMP0 non-inverting input */
    cmp_noninverting_input_select(CMP0, CMP_NONINVERTING_INPUT_PB0_PE9);
    /* configure CMP0 mode */
    cmp_mode_init(CMP0, CMP_MODE_HIGHSPEED, CMP_INVERTING_INPUT_VREFINT, CMP_HYSTERESIS_NO);
    /* configure CMP0 output */
    cmp_output_init(CMP0, CMP_OUTPUT_POLARITY_NONINVERTED);

    /* enable CMP0 */
    cmp_enable(CMP0);
    /* delay 1ms */
    delay_1ms(1);
}

/*!
    \brief      configure TIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer_config(void)
{
    /* initialize TIMER0 */
    timer_parameter_struct timer_init_parameter;
    timer_ic_parameter_struct timer_icinitpara;

    timer_deinit(TIMER0);

    /* TIMER0 configuration */
    timer_init_parameter.prescaler         = 599;
    timer_init_parameter.alignedmode       = TIMER_COUNTER_EDGE;
    timer_init_parameter.counterdirection  = TIMER_COUNTER_UP;
    timer_init_parameter.period            = 65535;
    timer_init_parameter.clockdivision     = TIMER_CKDIV_DIV1;
    timer_init_parameter.repetitioncounter = 0;
    timer_init(TIMER0, &timer_init_parameter);

    /* TIMER CH0 input capture configuration */
    timer_icinitpara.icpolarity  = TIMER_IC_POLARITY_RISING;
    timer_icinitpara.icselection = TIMER_IC_SELECTION_DIRECTTI;
    timer_icinitpara.icprescaler = TIMER_IC_PSC_DIV1;
    timer_icinitpara.icfilter    = 0x15;
    timer_input_capture_config(TIMER0, TIMER_CH_0, &timer_icinitpara);

    /* configure TIMER CH0 input */
    syscfg_timer_input_source_select(TIMER0_CI0_INPUT_CMP0_OUT);

    /* clear TIMER CH0 flag */
    timer_flag_clear(TIMER0, TIMER_FLAG_CH0);

    /* enable TIMER0 counter */
    timer_enable(TIMER0);
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
