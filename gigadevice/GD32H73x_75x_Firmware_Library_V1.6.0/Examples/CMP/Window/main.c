/*!
    \file    main.c
    \brief   CMP window demo

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

typedef enum {
    STATE_OVER_THRESHOLD,
    STATE_WITHIN_THRESHOLD,
    STATE_UNDER_THRESHOLD
} cmp_state_enum;

/* configure RCU peripheral */
void rcu_config(void);
/* configure GPIO peripheral */
void gpio_config(void);
/* configure CMP peripheral */
void cmp_config(void);
/* check CMP output state */
cmp_state_enum cmp_output_state_check(void);
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

    /* initialize LEDs */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);

    /* configure RCU peripheral */
    rcu_config();
    /* configure GPIO peripheral */
    gpio_config();
    /* configure CMP peripheral */
    cmp_config();

    while(1) {
        /* input voltage is over the thresholds: higher and lower thresholds */
        if(STATE_OVER_THRESHOLD == cmp_output_state_check()) {
            gd_eval_led_on(LED1);
            gd_eval_led_on(LED2);
        }
        /* input voltage is within the thresholds: higher and lower thresholds */
        if(STATE_WITHIN_THRESHOLD == cmp_output_state_check()) {
            delay_1ms(500);
            if(STATE_WITHIN_THRESHOLD == cmp_output_state_check()) {
                gd_eval_led_on(LED1);
                gd_eval_led_off(LED2);
            }
        }
        /* input voltage is under the thresholds: higher and lower thresholds */
        if(STATE_UNDER_THRESHOLD == cmp_output_state_check()) {
            gd_eval_led_off(LED1);
            gd_eval_led_off(LED2);
        }
    }
}

/*!
    \brief      check CMP output state
    \param[in]  none
    \param[out] none
    \retval     cmp_state
*/
cmp_state_enum cmp_output_state_check(void)
{
    cmp_state_enum state = STATE_WITHIN_THRESHOLD;

    /* check if cmp0 output level is high and cmp1 output level is high */
    if((CMP_OUTPUTLEVEL_HIGH == cmp_output_level_get(CMP0))
            && (CMP_OUTPUTLEVEL_HIGH == cmp_output_level_get(CMP1))) {
        state = STATE_OVER_THRESHOLD;
    }
    /* check if cmp0 output level is low and cmp1 output level is high */
    if((CMP_OUTPUTLEVEL_LOW == cmp_output_level_get(CMP0))
            && (CMP_OUTPUTLEVEL_HIGH == cmp_output_level_get(CMP1))) {
        state = STATE_WITHIN_THRESHOLD;
    }
    /* check if cmp0 output level is low and cmp1 output level is low */
    if((CMP_OUTPUTLEVEL_LOW == cmp_output_level_get(CMP0))
            && (CMP_OUTPUTLEVEL_LOW == cmp_output_level_get(CMP1))) {
        state = STATE_UNDER_THRESHOLD;
    }
    return state;
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

    /* initialize CMP1 */
    cmp_deinit(CMP1);
    /* when using VREFINT as inverting input, the voltage bridge and voltage scaler should be enable */
    cmp_voltage_scaler_enable(CMP1);
    cmp_scaler_bridge_enable(CMP1);
    /* configure CMP1 mode */
    cmp_mode_init(CMP1, CMP_MODE_HIGHSPEED, CMP_INVERTING_INPUT_1_2VREFINT, CMP_HYSTERESIS_NO);
    /* configure CMP1 output */
    cmp_output_init(CMP1, CMP_OUTPUT_POLARITY_NONINVERTED);

    /* enable CMP window */
    cmp_window_enable();

    /* enable CMP0 */
    cmp_enable(CMP0);
    /* enable CMP1 */
    cmp_enable(CMP1);
    /* delay 1ms */
    delay_1ms(1);
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
