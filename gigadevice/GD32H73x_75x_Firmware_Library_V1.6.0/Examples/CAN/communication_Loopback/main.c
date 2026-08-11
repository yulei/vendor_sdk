/*!
    \file    main.c
    \brief   CAN loopback and silent communication mode

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
#include "string.h"

/* select CAN */
#define DEV_CAN0_USED
//#define DEV_CAN1_USED
//#define DEV_CAN2_USED

#ifdef DEV_CAN0_USED
#define CANX    CAN0
#elif defined DEV_CAN1_USED
#define CANX    CAN1
#elif defined DEV_CAN2_USED
#define CANX    CAN2
#else
#error ("DEV_CANx_USED should be defined");
#endif /* DEV_CAN0_USED */

#define  BUFFER_SIZE    8U

/* global variable declarations */
can_mailbox_descriptor_struct transmit_message;
can_mailbox_descriptor_struct receive_message;

const uint8_t tx_data[BUFFER_SIZE] = {0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8};
uint8_t rx_data[BUFFER_SIZE];

/* function declarations */
void bsp_board_config(void);
void can_loopback_init(void);
void can_loopback_test(void);
void cache_enable(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* enable the CPU Cache */
    cache_enable();
    /* configure CAN port and LED */
    bsp_board_config();
    /* initialize CAN */
    can_loopback_init();
    /* loopback of polling */
    can_loopback_test();

    while(1) {
    }
}

/*!
    \brief      board configuration
    \param[in]  none
    \param[out] none
    \retval     none
*/
void bsp_board_config(void)
{
    /* enable CAN port clock */
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOF);

    /* configure LEDs */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);

    gd_eval_led_off(LED1);
    gd_eval_led_off(LED2);

    /* configure CAN0_RX GPIO */
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_0);
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_0);
    gpio_af_set(GPIOD, GPIO_AF_9, GPIO_PIN_0);
    /* configure CAN0_TX GPIO */
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_1);
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_1);
    gpio_af_set(GPIOD, GPIO_AF_9, GPIO_PIN_1);

    /* configure CAN1_RX GPIO */
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_5);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5);
    gpio_af_set(GPIOB, GPIO_AF_9, GPIO_PIN_5);
    /* configure CAN1_TX GPIO */
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_13);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_13);
    gpio_af_set(GPIOB, GPIO_AF_9, GPIO_PIN_13);

    /* configure CAN2_RX GPIO */
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_6);
    gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6);
    gpio_af_set(GPIOF, GPIO_AF_2, GPIO_PIN_6);
    /* configure CAN2_TX GPIO */
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_7);
    gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_7);
    gpio_af_set(GPIOF, GPIO_AF_2, GPIO_PIN_7);
}

/*!
    \brief      initialize CAN and filter
    \param[in]  none
    \param[out] none
    \retval     none
*/
void can_loopback_init(void)
{
    can_parameter_struct can_parameter;

    /* enable CAN clock */
    rcu_can_clock_config(IDX_CAN0, RCU_CANSRC_APB2);
    rcu_can_clock_config(IDX_CAN1, RCU_CANSRC_APB2);
    rcu_can_clock_config(IDX_CAN2, RCU_CANSRC_APB2);
    rcu_periph_clock_enable(RCU_CAN0);
    rcu_periph_clock_enable(RCU_CAN1);
    rcu_periph_clock_enable(RCU_CAN2);

    /* initialize CAN register */
    can_deinit(CANX);
    /* initialize CAN */
    can_struct_para_init(CAN_INIT_STRUCT, &can_parameter);

    /* initialize CAN parameters */
    can_parameter.internal_counter_source = CAN_TIMER_SOURCE_BIT_CLOCK;
    can_parameter.self_reception = DISABLE;
    can_parameter.mb_tx_order = CAN_TX_HIGH_PRIORITY_MB_FIRST;
    can_parameter.mb_tx_abort_enable = ENABLE;
    can_parameter.local_priority_enable = DISABLE;
    can_parameter.mb_rx_ide_rtr_type = CAN_IDE_RTR_FILTERED;
    can_parameter.mb_remote_frame = CAN_STORE_REMOTE_REQUEST_FRAME;
    can_parameter.rx_private_filter_queue_enable = DISABLE;
    can_parameter.edge_filter_enable = DISABLE;
    can_parameter.protocol_exception_enable = DISABLE;
    can_parameter.rx_filter_order = CAN_RX_FILTER_ORDER_MAILBOX_FIRST;
    can_parameter.memory_size = CAN_MEMSIZE_32_UNIT;
    /* filter configuration */
    can_parameter.mb_public_filter = 0x5FFC0000U;
    /* baud rate 1Mbps, sample point at 80% */
    can_parameter.resync_jump_width = 1U;
    can_parameter.prop_time_segment = 2U;
    can_parameter.time_segment_1 = 5U;
    can_parameter.time_segment_2 = 2U;
    can_parameter.prescaler = 30U;

    /* initialize CAN */
    can_init(CANX, &can_parameter);
    /* enter loopback silent mode */
    can_operation_mode_enter(CANX, CAN_LOOPBACK_SILENT_MODE);
}

/*!
    \brief      function for CAN loopback and silent mode
    \param[in]  none
    \param[out] none
    \retval     ErrStatus
*/
void can_loopback_test(void)
{
    uint32_t timeout = 0xFFFFU;

    can_struct_para_init(CAN_MDSC_STRUCT, &transmit_message);
    can_struct_para_init(CAN_MDSC_STRUCT, &receive_message);
    /* initialize transmit message */
    transmit_message.rtr = 0U;
    transmit_message.ide = 0U;
    transmit_message.code = CAN_MB_TX_STATUS_DATA;
    transmit_message.brs = 0U;
    transmit_message.fdf = 0U;
    transmit_message.prio = 0U;
    transmit_message.data_bytes = BUFFER_SIZE;
    /* tx message content */
    transmit_message.data = (uint32_t *)(tx_data);
    transmit_message.id = 0x55U;

    receive_message.rtr = 0U;
    receive_message.ide = 0U;
    receive_message.code = CAN_MB_RX_STATUS_EMPTY;
    /* rx mailbox */
    receive_message.id = 0x55U;
    receive_message.data = (uint32_t *)(rx_data);
    can_mailbox_config(CANX, 1U, &receive_message);

    /* transmit message */
    can_mailbox_config(CANX, 0U, &transmit_message);
    /* waiting for transmit completed */
    while((RESET == can_flag_get(CANX, CAN_FLAG_MB0)) && (0U != timeout)) {
        timeout--;
    }
    timeout = 0xFFFFU;
    /* waiting for receive completed */
    while((RESET == can_flag_get(CANX, CAN_FLAG_MB1)) && (0U != timeout)) {
        timeout--;
    }

    /* check the receive message */
    can_mailbox_receive_data_read(CANX, 1U, &receive_message);

    /* check the receive message */
    if(0U == memcmp(rx_data, tx_data, receive_message.data_bytes)) {
        /* loopback test is success */
        gd_eval_led_on(LED1);
    } else {
        /* loopback test is failed */
        gd_eval_led_on(LED2);
    }
}

/*!
    \brief      enable the CPU Cache
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
