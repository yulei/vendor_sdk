/*!
    \file    main.c
    \brief   USART synchronous

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

#include "gd32e50x.h"
#include "gd32e507z_eval.h"
#include <stdio.h>

#define txbuffer_size1   (countof(txbuffer1) - 1)
#define txbuffer_size2   (countof(txbuffer2) - 1)
#define DYMMY_BYTE       0x00
#define countof(a)       (sizeof(a) / sizeof(*(a)))

uint8_t txbuffer1[] = "USART synchronous example: USART5 -> SPI0 using TXE and RXNE Flags";
uint8_t txbuffer2[] = "USART synchronous example: SPI0 -> USART5 using TXE and RXNE Flags";
uint8_t rxbuffer1[txbuffer_size2];
uint8_t rxbuffer2[txbuffer_size1];
__IO uint8_t data_read1 = txbuffer_size2;
__IO uint8_t data_read2 = txbuffer_size1;
__IO uint8_t tx_counter1 = 0, rx_counter1 = 0;
__IO uint8_t tx_counter2 = 0, rx_counter2 = 0;
ErrStatus state1 = ERROR;
ErrStatus state2 = ERROR;

void usart5_config(void);
void spi_config(void);
void led_init(void);
ErrStatus memory_compare(uint8_t* src, uint8_t* dst, uint16_t length);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* initialize leds */
    led_init();
    /* turn off LED1~2 */
    gd_eval_led_off(LED1);
    gd_eval_led_off(LED2);

    /* configure USART */
    usart5_config();

    /* configure SPI */
    spi_config();

    while(data_read2--){
        while(RESET == usart5_flag_get(USART5, USART5_FLAG_TBE)){
        }
        /* write one byte in the USART5 data register */
        usart_data_transmit(USART5, txbuffer1[tx_counter1++]);
        /* wait until end of transmit */
        while(RESET == usart5_flag_get(USART5, USART5_FLAG_TC)){
        }
        /* wait the byte is entirely received by SPI0 */  
        while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE)){
        }
        /* store the received byte in the rxbuffer2 */
        rxbuffer2[rx_counter2++] = spi_i2s_data_receive(SPI0);
    }

    /* clear the USART0 data register */
    usart_data_receive(USART5);

    while(data_read1--){
        /* wait until end of transmit */
        while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE)){
        }
        /* write one byte in the SPI0 transmit data register */
        spi_i2s_data_transmit(SPI0, txbuffer2[tx_counter2++]);

        /* send a dummy byte to generate clock to slave */ 
        usart_data_transmit(USART5, DYMMY_BYTE);
        /* wait until end of transmit */
        while(RESET == usart5_flag_get(USART5, USART5_FLAG_TC)){
        }
        /* wait the byte is entirely received by USART0 */
        while(RESET == usart5_flag_get(USART5, USART5_FLAG_RBNE)){
        }
        /* store the received byte in the rxbuffer1 */
        rxbuffer1[rx_counter1++] = usart_data_receive(USART5);
    }

    /* check the received data with the send ones */
    state1 = memory_compare(txbuffer1, rxbuffer2, txbuffer_size1);
    state2 = memory_compare(txbuffer2, rxbuffer1, txbuffer_size2);

    if(SUCCESS == state1){
        /* if the data transmitted from USART5 and received by SPI0 are the same */
        gd_eval_led_on(LED1);
    }else{
        /* if the data transmitted from USART5 and received by SPI0 are not the same */
        gd_eval_led_off(LED1);
    }
    if(SUCCESS == state2){
        /* if the data transmitted from SPI0 and received by USART5 are the same */
        gd_eval_led_on(LED2);
    }else{
        /* if the data transmitted from SPI0 and received by USART5 are not the same */
        gd_eval_led_off(LED2);
    }
    while(1){
    }
}
/*!
    \brief      configure USART
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usart5_config(void)
{
    /* enable USART5 GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOC);
    /* enable AF clock */
    rcu_periph_clock_enable(RCU_AF);
    /* enable USART5 clock */
    rcu_periph_clock_enable(RCU_USART5);

    /* configure USART CK Tx Rx as alternate function push-pull */
    gpio_init(GPIOC, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
    gpio_init(GPIOC, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
    gpio_init(GPIOC, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_7);

    /* configure USART5 CK Tx Rx as alternate function */
    gpio_afio_port_config(AFIO_PC8_USART5_CFG, ENABLE);
    gpio_afio_port_config(AFIO_PC6_USART5_CFG, ENABLE);
    gpio_afio_port_config(AFIO_PC7_USART5_CFG, ENABLE);

    /* configure USART synchronous mode */
    usart_synchronous_clock_enable(USART5);
    usart_synchronous_clock_config(USART5, USART_CLEN_EN, USART_CPH_2CK, USART_CPL_HIGH);

    usart_baudrate_set(USART5, 115200);
    /* configure USART transmitter */
    usart_transmit_config(USART5, USART_TRANSMIT_ENABLE);
    /* configure USART receiver */
    usart_receive_config(USART5, USART_RECEIVE_ENABLE);
    /* enable USART */
    usart_enable(USART5);
}

/*!
    \brief      configure SPI
    \param[in]  none
    \param[out] none
    \retval     none
*/
void spi_config(void)
{
    spi_parameter_struct spi_init_parameter;
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_AF);
    rcu_periph_clock_enable(RCU_SPI0);

    spi_i2s_deinit(SPI0);

    /* configure SPI SCK MISO MOSI as alternate function push-pull */
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_5);
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_6);
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_10MHZ, GPIO_PIN_7);

    /* configure SPI0 */
    spi_init_parameter.device_mode = SPI_SLAVE;
    spi_init_parameter.trans_mode = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_parameter.frame_size = SPI_FRAMESIZE_8BIT;
    spi_init_parameter.nss = SPI_NSS_SOFT;
    spi_init_parameter.endian = SPI_ENDIAN_LSB;
    spi_init_parameter.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
    spi_init_parameter.prescale = SPI_PSC_32;
    spi_init(SPI0, &spi_init_parameter);

    /* SPI0 enable */
    spi_enable(SPI0);
}

/*!
    \brief      initialize leds
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_init(void)
{
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
}

/*!
    \brief      memory compare function
    \param[in]  src: source data
    \param[in]  dst: destination data
    \param[in]  length: the compare data length
    \param[out] none
    \retval     ErrStatus: ERROR or SUCCESS
*/
ErrStatus memory_compare(uint8_t* src, uint8_t* dst, uint16_t length)
{
    while(length--){
        if(*src++ != *dst++){
            return ERROR;
        }
    }
    return SUCCESS;
}
