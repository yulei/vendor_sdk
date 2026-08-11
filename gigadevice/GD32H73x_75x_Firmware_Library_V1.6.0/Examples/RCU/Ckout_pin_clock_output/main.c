/*!
    \file    main.c
    \brief   clock output via CKOUT pin example

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

    /* initialize the systick */
    systick_config();
    /* enable peripheral clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* configure clock output 0 pin */
    gpio_af_set(GPIOA, GPIO_AF_0, GPIO_PIN_8);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8);

    /* enable peripheral clock */
    rcu_periph_clock_enable(RCU_GPIOC);
    /* configure clock output 1 pin */
    gpio_af_set(GPIOC, GPIO_AF_0, GPIO_PIN_9);
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_9);
    delay_1ms(1000);

    /* enable IRC64M clock */
    rcu_irc64mdiv_clock_config(RCU_IRC64M_DIV1);
    rcu_osci_on(RCU_IRC64M);
    if(ERROR == rcu_osci_stab_wait(RCU_IRC64M)){
        while(1){
        }
    }
    /* configure CKOUT0 to output IRC64MDIV clock */
    rcu_ckout0_config(RCU_CKOUT0SRC_IRC64MDIV,RCU_CKOUT0_DIV1);
    delay_1ms(1000);

    /* enable HXTAL clock */
    rcu_osci_on(RCU_HXTAL);
    if(ERROR == rcu_osci_stab_wait(RCU_HXTAL)){
        while(1){
        }
    }
    /* configure CKOUT0 to output HXTAL clock */
    rcu_ckout0_config(RCU_CKOUT0SRC_HXTAL, RCU_CKOUT0_DIV1);
    delay_1ms(1000);

    /* configure the pll1 input and output clock range */
    rcu_pll_input_output_clock_range_config(IDX_PLL1,RCU_PLL1RNG_4M_8M,RCU_PLL1VCO_192M_836M);
    /* configure the PLL1 clock: CK_PLL1P/CK_PLL1Q/CK_PLL1R = HXTAL_VALUE / 5 * 40 / 1*/
    rcu_pll1_config(5, 40,1,1,1);
    /* enable PLL1R clock output */
    rcu_pll_clock_output_enable(RCU_PLL1R);
    /* enable PLL1 clock */
    rcu_osci_on(RCU_PLL1_CK);

    if(ERROR == rcu_osci_stab_wait(RCU_PLL1_CK)){
        while(1){
        }
    }
     /* configure CKOUT1 to output CK_PLL1R / 8 */
    rcu_ckout1_config(RCU_CKOUT1SRC_PLL1R, RCU_CKOUT1_DIV8);
    delay_1ms(1000);

    /* configure the pll2 input and output clock range */
    rcu_pll_input_output_clock_range_config(IDX_PLL2,RCU_PLL2RNG_4M_8M,RCU_PLL2VCO_192M_836M);
    /* configure the PLL2 clock: CK_PLL2P/CK_PLL2Q/CK_PLL2R = HXTAL_VALUE / 5 * (40 + 0.5) / 1*/
    rcu_pll2_config(5, 40,1,1,1);
    /* pll2 fractional latch disable */
    rcu_pll_fractional_latch_disable(IDX_PLL2);
    /* configure fractional part of the multiplication factor for PLL2 VCO */
    rcu_pll_fractional_config(IDX_PLL2, 4096);
    /* pll2 fractional latch enable */
    rcu_pll_fractional_latch_enable(IDX_PLL2);
    /* enable PLL2R clock output */
    rcu_pll_clock_output_enable(RCU_PLL2R);
    /* enable PLL2 clock */
    rcu_osci_on(RCU_PLL2_CK);

    if(ERROR == rcu_osci_stab_wait(RCU_PLL2_CK)){
        while(1){
        }
    }
    /* configure CKOUT1 to output CK_PLL2R / 8 */
    rcu_ckout1_config(RCU_CKOUT1SRC_PLL2R, RCU_CKOUT1_DIV8);
    delay_1ms(1000);

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
