/*!
    \file    main.c
    \brief   TRIGSEL select CMP0_OUT as trigger source to TRIGSEL OUT0 example

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

/* configure the CMP peripheral */
void cmp_config(void);

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
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* enable the CPU cache */
    cache_enable();
    /* enable the GPIOA clock */
    rcu_periph_clock_enable(RCU_GPIOA);

    /* configure trigsel output pin */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_15);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_15);
    gpio_af_set(GPIOA, GPIO_AF_13, GPIO_PIN_15);

    /* enable TRIGSEL clock */
    rcu_periph_clock_enable(RCU_TRIGSEL);
    /* configure TRIGSEL OUT0 select CMP0_OUT as trigger source */
    trigsel_init(TRIGSEL_OUTPUT_TRIGSEL_OUT0, TRIGSEL_INPUT_CMP0_OUT);
    /* lock trigger register */
    trigsel_register_lock_set(TRIGSEL_OUTPUT_TRIGSEL_OUT0);

    /* configure comparator */
    cmp_config();

    while(1){
    }
}

/*!
    \brief      configure the CMP peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cmp_config(void)
{
    /* enable GPIOB clock */
    rcu_periph_clock_enable(RCU_GPIOB);
    /* enable comparator clock */
    rcu_periph_clock_enable(RCU_CMP);

    /* configure comparator plus input: PB0 */
    gpio_mode_set(GPIOB, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_0);

    /* deinitialize comparator */
    cmp_deinit(CMP0);
    cmp_voltage_scaler_enable(CMP0);
    cmp_scaler_bridge_enable(CMP0);
    /* selecte PB0 as the noninverting input for CMP0 */
    cmp_noninverting_input_select(CMP0, CMP_NONINVERTING_INPUT_PB0_PE9);
    /* configure mode */
    cmp_mode_init(CMP0, CMP_MODE_HIGHSPEED, CMP_INVERTING_INPUT_VREFINT, CMP_HYSTERESIS_NO);
    /* configure comparator output */
    cmp_output_init(CMP0, CMP_OUTPUT_POLARITY_NONINVERTED);
    /* enable comparator */
    cmp_enable(CMP0);

    delay_1ms(1);
}
