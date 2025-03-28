/*!
    \file    main.c
    \brief   wakeup from deepsleep

    \version 2023-12-31, V1.4.0, firmware for GD32E50x
*/

/*
    Copyright (c) 2023, GigaDevice Semiconductor Inc.

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

#include <stdio.h>
#include "i2c_ie.h"
#include "gd32e507z_eval.h"
#include "systick.h"

#define I2C2_OWN_ADDRESS7 0x82
uint8_t i2c_buffer_receiver[16] = {0};
volatile uint8_t *i2c_rxbuffer;

void rcu_config(void);
void i2c_gpio_config(void);
void i2c_config(void);
void i2c_nvic_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure systick*/
    systick_config();
    /* initialize LED1 */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    /* configure wakeup key */
    gd_eval_key_init(KEY_A, KEY_MODE_GPIO);
    /* configure the I2C2 clock source selection */
    rcu_i2c2_clock_config(RCU_I2C2SRCSRC_CKIRC8M);

    /* configure RCU */
    rcu_config();
    /* configure GPIO */
    i2c_gpio_config();
    /* configure I2C */
    i2c_config();
    /* configure NVIC */
    i2c_nvic_config();
    /* initialize the EXTI line 20 */
    exti_init(EXTI_20, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    /* enable wakeup from deep-sleep mode */
    i2c_wakeup_from_deepsleep_enable(I2C2);
    /* enable the I2C2 interrupt */
    i2c2_interrupt_enable(I2C2, I2C2_INT_ERR | I2C2_INT_STPDET | I2C2_INT_ADDM | I2C2_INT_RBNE);
    while(1) {
        if(SET != gpio_input_bit_get(KEY_A_GPIO_PORT, KEY_A_PIN)) {
            gd_eval_led_on(LED2);
            pmu_to_deepsleepmode(PMU_LDO_LOWPOWER, PMU_LOWDRIVER_ENABLE, WFI_CMD);
        }
    }
}

/*!
    \brief      enable the peripheral clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable GPIOC clock */
    rcu_periph_clock_enable(RCU_GPIOC);
    /* enable GPIOA clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* enable AFIO clock */
    rcu_periph_clock_enable(RCU_AF);
    /* enable I2C2 clock */
    rcu_periph_clock_enable(RCU_I2C2);
    /* enable PMU clock */
    rcu_periph_clock_enable(RCU_PMU);
}

/*!
    \brief      configure the I2C2 GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
*/
void i2c_gpio_config(void)
{
    /* configure AFIO port alternate function */
    gpio_afio_port_config(AFIO_PC9_I2C2_CFG, ENABLE);
    gpio_afio_port_config(AFIO_PA8_I2C2_CFG, ENABLE);

    /* connect PA8 to I2C2_SCL */
    /* connect PC9 to I2C2_SDA */
    gpio_init(GPIOC, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
    gpio_init(GPIOA, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
}

/*!
    \brief      configure the I2C2 interface
    \param[in]  none
    \param[out] none
    \retval     none
*/
void i2c_config(void)
{
    /* configure I2C timing */
    i2c_timing_config(I2C2, 0, 0x8, 0);
    /* configure I2C address */
    i2c_address_config(I2C2, I2C2_OWN_ADDRESS7, I2C2_ADDFORMAT_7BITS);
    /* configure number of bytes to be transferred */
    i2c_transfer_byte_number_config(I2C2, 16);
    /* enable I2C2 */
    i2c_enable(I2C2);
}
/*!
    \brief      configure the NVIC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void i2c_nvic_config(void)
{
    nvic_irq_enable(I2C2_EV_IRQn, 0, 3);
    nvic_irq_enable(I2C2_ER_IRQn, 0, 2);
    nvic_irq_enable(I2C2_WKUP_IRQn, 0, 1);
}

/*!
    \brief      toggle the led every 200ms
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_spark(void)
{
    static __IO uint32_t timingdelaylocal = 0U;

    if(timingdelaylocal) {

        if(timingdelaylocal < 200U) {
            gd_eval_led_on(LED1);
        } else {
            gd_eval_led_off(LED1);
        }

        timingdelaylocal--;
    } else {
        timingdelaylocal = 400U;
    }
}
