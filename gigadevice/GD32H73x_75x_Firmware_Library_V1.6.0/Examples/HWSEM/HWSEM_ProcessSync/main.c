/*!
    \file    main.c
    \brief   HWSEM Process Sync example

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
#include "main.h"
#include "systick.h"

void cache_enable(void);
void led_config(void);
void rcu_config(void);
void nvic_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/

int main(void)
{
    /* configure systick */
    systick_config();
    /* configure LEDs */
    led_config();
    /* enable the cache */
    cache_enable();
    /* configure the clocks */
    rcu_config();
    /* configure the WAKEUP key */
    gd_eval_key_init(KEY_WAKEUP, KEY_MODE_EXTI);

    while(1) {
        /* try to lock the semaphore with process ID */
        if(SUCCESS == hwsem_lock_set(SEM1, PROCESS_1)) {
            /* turn on LED1 */
            gd_eval_led_on(LED1);
            delay_1ms(5000);

            /* try to release the semaphore with process ID */
            if(SUCCESS == hwsem_lock_release(SEM1, PROCESS_1)) {
                hwsem_flag_clear(SEM1);
                delay_1ms(5000);
            }
        }
    }
}

/*!
    \brief      enable the chache
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cache_enable(void)
{
    /* enable I-Cache */
    SCB_EnableICache();

    /* enable D-Cache */
    SCB_EnableDCache();
}

/*!
    \brief      configure the LEDs
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_config(void)
{
    /* initialize the LEDs */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
}

/*!
    \brief      configure the HWSEM clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable HWSEM clock */
    rcu_periph_clock_enable(RCU_HWSEM);
}

/*!
    \brief      configure the NVIC
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_config(void)
{
    /* enable HWSEM interrupt */
    hwsem_interrupt_enable(SEM1);
    /* enable NVIC */
    nvic_irq_enable(HWSEM_IRQn, 0, 0);
}
