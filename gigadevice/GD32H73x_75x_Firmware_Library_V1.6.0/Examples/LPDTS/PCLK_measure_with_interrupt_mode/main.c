/*!
    \file    main.c
    \brief   LPDTS measure(PCLK) using interrupt example

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
#include <stdio.h>
#include "gd32h759i_eval.h"

__IO FlagStatus g_tempmeasure_complete = RESET;

void cache_enable(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/

int main(void)
{
    int32_t temperature;
    ErrStatus ret;
    lpdts_parameter_struct  lpdts_init_struct;
    /* enable the CPU Cache */
    cache_enable();

    /* systick configuration */
    systick_config();
    /* initialize the USART */
    gd_eval_com_init(EVAL_COM);

    nvic_irq_enable(LPDTS_IRQn, 0U, 0U);
    /* reset the LPDTS registers */
    lpdts_deinit();
    /* enable LPDTS clock */
    rcu_periph_clock_enable(RCU_LPDTS);

    /* initialize the LPDTS */
    lpdts_struct_para_init(&lpdts_init_struct);
    lpdts_init(&lpdts_init_struct);
    /* set threshold value */
    lpdts_high_threshold_set(0U);
    lpdts_low_threshold_set(0U);
    /* select reference clock */
    lpdts_ref_clock_source_config(REF_PCLK);
    /* enable end of measurement interrupt */
    lpdts_interrupt_enable(LPDTS_INT_EM);

    /* enable LPDTS temperature sensor */
    lpdts_enable();
    /* wait till TSRF flag is set */
    while(RESET == lpdts_flag_get(LPDTS_FLAG_TSR)) {
    }
    /* software trigger start */
    lpdts_soft_trigger_enable();

    /* infinite loop */
    while(1) {
        if(SET == g_tempmeasure_complete) {
            /* disable LPDTS software trigger */
            lpdts_soft_trigger_disable();
            /* wait till TSR flag is set */
            while(RESET == lpdts_flag_get(LPDTS_FLAG_TSR)) {
            }
            /* get LPDTS temperature */
            ret = lpdts_temperature_get(&temperature);

            /* disable LPDTS temperature sensor */
            lpdts_disable();
            delay_1ms(1000);
            if(ret == SUCCESS){
                printf("The temperature is %d degrees Celsius now.\r\n", temperature);
                g_tempmeasure_complete = RESET;
            } else {
                printf("Get temperature failed.\r\n");
                g_tempmeasure_complete = RESET;
            }
        }
    }
}

/*!
    \brief      enable the CPU Chache
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cache_enable(void)
{
    /* Enable I-Cache */
    SCB_EnableICache();

    /* Enable D-Cache */
    SCB_EnableDCache();
}
