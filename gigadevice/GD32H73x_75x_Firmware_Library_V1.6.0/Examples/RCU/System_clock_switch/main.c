/*!
    \file    main.c
    \brief   system clock switch example

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

static void _delay(uint32_t timeout);
static void switch_system_clock_to_200m_hxtal(void);
static void switch_system_clock_to_400m_irc64m(void);
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

    /* initialize the USART */
    gd_eval_com_init(EVAL_COM);
    printf("\r\nCK_SYS switch test demo\r\n");

    /* disable the USART */
    usart_disable(EVAL_COM);
    /* switch system clock to 200MHz by HXTAL */
    switch_system_clock_to_200m_hxtal();
    gd_eval_com_init(EVAL_COM);
    /* print out the clock frequency of system */
    printf("\r\nCK_SYS is %d", rcu_clock_freq_get(CK_SYS));
    _delay(1000);

    /* switch system clock to 400MHz by IRC64M */
    switch_system_clock_to_400m_irc64m();
    gd_eval_com_init(EVAL_COM);
    /* print out the clock frequency of system */
    printf("\r\nCK_SYS is %d", rcu_clock_freq_get(CK_SYS));

    while(1) {
    }
}

/*!
    \brief      delay function
    \param[in]  timeout: time out
    \param[out] none
    \retval     none
*/
static void _delay(uint32_t timeout)
{
    __IO uint32_t i, j;

    for(i = 0; i < timeout; i++) {
        for(j = 0; j < 5000; j++) {
        }
    }
}

/*!
    \brief      switch system clock to 200M by HXTAL
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void switch_system_clock_to_200m_hxtal(void)
{
    uint32_t timeout = 0U;
    uint32_t stab_flag = 0U;

    /* select IRC64M as system clock source, deinitialize the RCU */
    rcu_system_clock_source_config(RCU_CKSYSSRC_IRC64MDIV);
    rcu_deinit();
    /* enable HXTAL */
    RCU_CTL |= RCU_CTL_HXTALEN;

    /* wait until HXTAL is stable or the startup time is longer than HXTAL_STARTUP_TIMEOUT */
    do {
        timeout++;
        stab_flag = (RCU_CTL & RCU_CTL_HXTALSTB);
    } while((0U == stab_flag) && (HXTAL_STARTUP_TIMEOUT != timeout));

    /* if fail */
    if(0U == (RCU_CTL & RCU_CTL_HXTALSTB)) {
        while(1) {
        }
    }

    /* HXTAL is stable */
    /* AHB = SYSCLK */
    RCU_CFG0 |= RCU_AHB_CKSYS_DIV1;
    /* APB4 = AHB / 2 */
    RCU_CFG0 |= RCU_APB4_CKAHB_DIV2;
    /* APB3 = AHB / 2 */
    RCU_CFG0 |= RCU_APB3_CKAHB_DIV2;
    /* APB2 = AHB / 1 */
    RCU_CFG0 |= RCU_APB2_CKAHB_DIV1;
    /* APB1 = AHB / 2 */
    RCU_CFG0 |= RCU_APB1_CKAHB_DIV2;

    /* PLL select HXTAL, configure PLL input and output range */
    RCU_PLLALL &= ~(RCU_PLLALL_PLLSEL | RCU_PLLALL_PLL0VCOSEL | RCU_PLLALL_PLL0RNG);
    RCU_PLLALL |= (RCU_PLLSRC_HXTAL | RCU_PLLALL_PLL0VCOSEL | RCU_PLL0RNG_4M_8M);

    /* PLL0P = HXTAL / 5 * 40 = 200 MHz */
    RCU_PLL0 &= ~(RCU_PLL0_PLL0N | RCU_PLL0_PLL0PSC | RCU_PLL0_PLL0P | RCU_PLL0_PLL0R | RCU_PLL0_PLLSTBSRC);
    RCU_PLL0 |= (((40 - 1) << 6U) | (5 << 0U) | ((1 - 1) << 16U) | ((2 - 1) << 24U));
    RCU_PLLADDCTL &= ~(RCU_PLLADDCTL_PLL0Q);
    RCU_PLLADDCTL |= ((2 - 1) << 0U);

    /* enable PLL0P, PLL0Q, PLL0R */
    RCU_PLLADDCTL |= RCU_PLLADDCTL_PLL0PEN | RCU_PLLADDCTL_PLL0QEN | RCU_PLLADDCTL_PLL0REN;

    /* enable PLL0 */
    RCU_CTL |= RCU_CTL_PLL0EN;

    /* wait until PLL0 is stable */
    while(0U == (RCU_CTL & RCU_CTL_PLL0STB)) {
    }

    /* select PLL0P as system clock */
    RCU_CFG0 &= ~RCU_CFG0_SCS;
    RCU_CFG0 |= RCU_CKSYSSRC_PLL0P;

    /* wait until PLL0P is selected as system clock */
    while(RCU_SCSS_PLL0P != (RCU_CFG0 & RCU_CFG0_SCSS)) {
    }
}

/*!
    \brief      switch system clock to 400M by IRC64M
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void switch_system_clock_to_400m_irc64m(void)
{
    uint32_t timeout = 0U;
    uint32_t stab_flag = 0U;

    if(RCU_SCSS_IRC64MDIV != (RCU_CFG0 & RCU_CFG0_SCSS)){
        /* disable IRC64M */
        rcu_osci_off(RCU_IRC64M);
    }
    
    /* PLL select IRC64MDIV, config IRC64MDIV as IRC64M / 8 */
    rcu_irc64mdiv_clock_config(RCU_IRC64M_DIV8);
    
    rcu_deinit();
    /* enable IRC64M */
    RCU_CTL |= RCU_CTL_IRC64MEN;

    /* wait until IRC64M is stable or the startup time is longer than IRC64M_STARTUP_TIMEOUT */
    do {
        timeout++;
        stab_flag = (RCU_CTL & RCU_CTL_IRC64MSTB);
    } while((0U == stab_flag) && (IRC64M_STARTUP_TIMEOUT != timeout));

    /* if fail */
    if(0U == (RCU_CTL & RCU_CTL_IRC64MSTB)) {
        while(1) {
        }
    }

    /* select IRC64M as system clock source, deinitialize the RCU */
    rcu_system_clock_source_config(RCU_CKSYSSRC_IRC64MDIV);
    
    /* AHB = SYSCLK / 2 */
    RCU_CFG0 |= RCU_AHB_CKSYS_DIV2;
    /* APB4 = AHB / 2 */
    RCU_CFG0 |= RCU_APB4_CKAHB_DIV2;
    /* APB3 = AHB / 2 */
    RCU_CFG0 |= RCU_APB3_CKAHB_DIV2;
    /* APB2 = AHB / 1 */
    RCU_CFG0 |= RCU_APB2_CKAHB_DIV1;
    /* APB1 = AHB / 2 */
    RCU_CFG0 |= RCU_APB1_CKAHB_DIV2;

	/* PLL input and output range */
    RCU_PLLALL &= ~(RCU_PLLALL_PLLSEL | RCU_PLLALL_PLL0VCOSEL | RCU_PLLALL_PLL0RNG);
    RCU_PLLALL |= (RCU_PLLSRC_IRC64MDIV | RCU_PLL0RNG_4M_8M);

    /* PLLP = IRC64MDIV(8M) / 2 * 100 / 1 = 400 MHz */
    RCU_PLL0 &= ~(RCU_PLL0_PLL0N | RCU_PLL0_PLL0PSC | RCU_PLL0_PLL0P | RCU_PLL0_PLL0R | RCU_PLL0_PLLSTBSRC);
    RCU_PLL0 |= (((100 - 1) << 6U) | (2 << 0U) | ((1 - 1) << 16U) | ((2 - 1) << 24U));
    RCU_PLLADDCTL &= ~(RCU_PLLADDCTL_PLL0Q);
    RCU_PLLADDCTL |= ((2 - 1) << 0U);

    /* enable PLL0P, PLL0Q, PLL0R */
    RCU_PLLADDCTL |= RCU_PLLADDCTL_PLL0PEN | RCU_PLLADDCTL_PLL0QEN | RCU_PLLADDCTL_PLL0REN;

    /* enable PLL0 */
    RCU_CTL |= RCU_CTL_PLL0EN;

    /* wait until PLL0 is stable */
    while(0U == (RCU_CTL & RCU_CTL_PLL0STB)) {
    }

    /* select PLL0P as system clock */
    RCU_CFG0 &= ~RCU_CFG0_SCS;
    RCU_CFG0 |= RCU_CKSYSSRC_PLL0P;

    /* wait until PLL0P is selected as system clock */
    while(RCU_SCSS_PLL0P != (RCU_CFG0 & RCU_CFG0_SCSS)) {
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
