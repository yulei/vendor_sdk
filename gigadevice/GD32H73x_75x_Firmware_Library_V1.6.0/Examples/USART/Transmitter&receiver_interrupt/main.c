/*!
    \file    main.c
    \brief   USART transmit and receive interrupt

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

/* using USART FIFO function */
//#define USING_USART_FIFO

#define ARRAYNUM(arr_nanme)      (uint32_t)(sizeof(arr_nanme) / sizeof(*(arr_nanme)))
#define TRANSMIT_SIZE            (ARRAYNUM(transmitter_buffer) - 1)

uint8_t transmitter_buffer[] = "\n\rUSART interrupt test\n\r";
uint8_t receiver_buffer[32];
uint8_t transfersize = TRANSMIT_SIZE;
uint8_t receivesize = 32;
__IO uint8_t txcount = 0;
__IO uint16_t rxcount = 0;

void com_usart_init(void);
void cache_enable(void);
#ifdef USING_USART_FIFO
void usart_tftif_rffif_flag_flush(void);
#endif

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

    /* USART interrupt configuration */
    nvic_irq_enable(USART0_IRQn, 2, 0);

    /* initialize the com */
    com_usart_init();

#ifdef USING_USART_FIFO
    /* flush USART_INT_FLAG_TFT and USART_INT_FLAG_RFF interrupt flag */
    usart_tftif_rffif_flag_flush();
#endif

    /* enable USART TBE interrupt */
    usart_interrupt_enable(USART0, USART_INT_TBE);

    /* wait until USART send the transmitter_buffer */
    while(txcount < transfersize) {
    }

    while(RESET == usart_flag_get(USART0, USART_FLAG_TC)) {
    }

    usart_interrupt_enable(USART0, USART_INT_RBNE);

    /* wait until USART receive the receiver_buffer */
    while(rxcount < receivesize) {
    }
    if(rxcount == receivesize) {
        printf("\n\rUSART receive successfully!\n\r");
    }

    while(1) {
    }
}

/*!
    \brief      initialize the USART configuration of the com
    \param[in]  none
    \param[out] none
    \retval     none
*/
void com_usart_init(void)
{
    /* enable COM GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* enable USART clock */
    rcu_periph_clock_enable(RCU_USART0);

    /* connect port to USART TX */
    gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_9);
    /* connect port to USART RX */
    gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_10);

    /* configure USART TX as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_9);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_9);

    /* configure USART RX as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_10);

    /* USART configure */
    usart_deinit(USART0);
    usart_word_length_set(USART0, USART_WL_8BIT);
    usart_stop_bit_set(USART0, USART_STB_1BIT);
    usart_parity_config(USART0, USART_PM_NONE);
    usart_baudrate_set(USART0, 115200U);
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
#ifdef USING_USART_FIFO
    usart_transmit_fifo_threshold_config(USART0, USART_TFTCFG_THRESHOLD_1_2);
    usart_receive_fifo_threshold_config(USART0, USART_RFTCFG_THRESHOLD_1_2);
    usart_fifo_enable(USART0);
#endif /* USING_USART_FIFO */

    usart_enable(USART0);
}

#ifdef USING_USART_FIFO
/*!
    \brief      flush USART_INT_FLAG_TFT and USART_INT_FLAG_RFF interrupt flag
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usart_tftif_rffif_flag_flush(void)
{
    /* flush USART_INT_FLAG_TFT flag */
    if(RESET == (USART_REG_VAL2(USART0, USART_INT_FLAG_TFT) & BIT(USART_BIT_POS2(USART_INT_FLAG_TFT)))) {
        usart_disable(USART0);
        usart_enable(USART0);
    }

    /* flush USART_INT_FLAG_RFF flag */
    if(RESET != (USART_REG_VAL2(USART0, USART_INT_FLAG_RFF) & BIT(USART_BIT_POS2(USART_INT_FLAG_RFF)))) {
        usart_interrupt_flag_clear(USART0, USART_INT_FLAG_RFF);
    }
}
#endif /* USING_USART_FIFO */

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
