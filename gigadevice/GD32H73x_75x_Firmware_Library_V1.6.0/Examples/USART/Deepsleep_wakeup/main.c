/*!
    \file    main.c
    \brief   Deepsleep wakeup

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
#include "systick.h"

extern __IO uint8_t counter0;

static void system_clock_reconfig(void);
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

    systick_config();

    gd_eval_led_init(LED1);
    rcu_periph_clock_enable(RCU_SYSCFG);

    /* USART configuration the CK_IRC64MDIV as USART clock */
    rcu_usart_clock_config(IDX_USART0, RCU_USARTSRC_IRC64MDIV);
    gd_eval_com_init(EVAL_COM);

    nvic_irq_enable(USART0_IRQn, 0, 0);
    /* USART0 Wakeup EXTI line configuration */
    exti_init(EXTI_27, EXTI_INTERRUPT, EXTI_TRIG_RISING);

    delay_1ms(2000);

    /* use start bit wakeup MCU */
    usart_wakeup_mode_config(EVAL_COM, USART_WUM_STARTB);

    /* enable USART */
    usart_enable(EVAL_COM);

    /* ensure USART is enabled */
    while(RESET == usart_flag_get(EVAL_COM, USART_FLAG_REA)) {
    }

    /* check USART is not transmitting */
    while(SET == usart_flag_get(EVAL_COM, USART_FLAG_BSY)) {
    }

    usart_wakeup_enable(EVAL_COM);
    /* enable the WUIE interrupt */
    usart_interrupt_enable(EVAL_COM, USART_INT_WU);

    /* enable PMU APB clock */
    rcu_periph_clock_enable(RCU_PMU);
    /* enter deep-sleep mode */
    pmu_to_deepsleepmode(WFI_CMD);

    /* wait a WUIE interrupt event */
    while(0x00 == counter0);

    /* disable USART peripheral in deepsleep mode */
    usart_wakeup_disable(EVAL_COM);

    while(RESET == usart_flag_get(EVAL_COM, USART_FLAG_RBNE));

    usart_data_receive(EVAL_COM);

    usart_receive_config(EVAL_COM, USART_RECEIVE_ENABLE);

    while(RESET == usart_flag_get(EVAL_COM, USART_FLAG_TC));

    /* disable the USART */
    usart_disable(EVAL_COM);

    /* reconfigure system clock */
    system_clock_reconfig();

    systick_config();

    while(1) {
    }
}

/*!
    \brief      reconfigure system clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void system_clock_reconfig(void)
{
    /* enable HXTAL */
    RCU_CTL |= RCU_CTL_HXTALEN;

    while(SUCCESS != rcu_osci_stab_wait(RCU_HXTAL)) {
    }

    /* insert TCM wait state at 600MHz */
    RCU_APB4EN |= 0x01;
    SYSCFG_SRAMCFG1 |= SYSCFG_SRAMCFG1_TCM_WAITSTATE;
    
    /* configure AHB */
    rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV2);

    /* configure APB1, APB2, APB3, APB4 */
    rcu_apb1_clock_config(RCU_APB1_CKAHB_DIV2);
    rcu_apb2_clock_config(RCU_APB2_CKAHB_DIV1);
    rcu_apb3_clock_config(RCU_APB3_CKAHB_DIV2);
    rcu_apb4_clock_config(RCU_APB4_CKAHB_DIV2);

    /* PLL select HXTAL, configure PLL input and output range */
    RCU_PLLALL &= ~(RCU_PLLALL_PLLSEL | RCU_PLLALL_PLL0VCOSEL | RCU_PLLALL_PLL0RNG);
    RCU_PLLALL |= (RCU_PLLSRC_HXTAL | RCU_PLL0RNG_4M_8M);

    /* PLLP = HXTAL / 5 * 120 / 1 = 600 MHz */
    RCU_PLL0 &= ~(RCU_PLL0_PLL0N | RCU_PLL0_PLL0PSC | RCU_PLL0_PLL0P | RCU_PLL0_PLL0R | RCU_PLL0_PLLSTBSRC);
    RCU_PLL0 |= (((120 - 1) << 6U) | (5 << 0U) | ((1 - 1) << 16U) | ((2 - 1) << 24U));
    RCU_PLLADDCTL &= ~(RCU_PLLADDCTL_PLL0Q);
    RCU_PLLADDCTL |= ((2 - 1) << 0U);

    /* enable PLLP, PLLQ, PLLR */
    RCU_PLLADDCTL |= RCU_PLLADDCTL_PLL0PEN | RCU_PLLADDCTL_PLL0QEN | RCU_PLLADDCTL_PLL0REN;
    /* enable PLL */
    RCU_CTL |= RCU_CTL_PLL0EN;

    /* wait until PLL is stable */
    while(0U == (RCU_CTL & RCU_CTL_PLL0STB)) {
    }

    /* select PLL as system clock */
    RCU_CFG0 &= ~RCU_CFG0_SCS;
    RCU_CFG0 |= RCU_CKSYSSRC_PLL0P;

    /* wait until PLL is selected as system clock */
    while(RCU_SCSS_PLL0P != (RCU_CFG0 & RCU_CFG0_SCSS)) {
    }
}

/*!
    \brief      LED spark
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_spark(void)
{
    static __IO uint32_t time_delay = 0;

    if(0x00 != time_delay) {
        if(time_delay < 500) {
            /* light on */
            gd_eval_led_on(LED1);
        } else {
            /* light off */
            gd_eval_led_off(LED1);
        }

        time_delay--;
    } else {
        time_delay = 1000;
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
