/*!
    \file    main.c
    \brief   SPI fullduplex communication use polling mode

    \version 2026-03-24, V1.6.0, firmware for GD32H73x_75x
*/

/*
    Copyright (c) 2026, GigaDevice Semiconductor Inc.

    All rights reserved.

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
#include "gd32h759i_eval.h"
#include <stdio.h>

#define ARRAYSIZE         10
#define SET_SPI0_NSS_HIGH          gpio_bit_set(GPIOA,GPIO_PIN_4);
#define SET_SPI0_NSS_LOW           gpio_bit_reset(GPIOA,GPIO_PIN_4);

uint32_t send_n = 0, receive_n = 0;
uint8_t spi0_send_array[ARRAYSIZE] = {0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA};
uint8_t spi1_send_array[ARRAYSIZE] = {0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA};
uint8_t spi0_receive_array[ARRAYSIZE] = {0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA};
uint8_t spi1_receive_array[ARRAYSIZE] = {0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA};

void cache_enable(void);
void rcu_config(void);
void gpio_config(void);
void spi_config(void);
ErrStatus memory_compare(uint8_t* src, uint8_t* dst, uint8_t length);

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

    /* init led1 and led2 */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);

    /* peripheral clock enable */
    rcu_config();
    /* GPIO config */
    gpio_config();
    /* SPI config */
    spi_config();

    /* configure SPI current data number  */
    spi_current_data_num_config(SPI0, ARRAYSIZE);
    spi_current_data_num_config(SPI1, ARRAYSIZE);  

    SET_SPI0_NSS_HIGH

    /* SPI enable */
    spi_enable(SPI1);
    spi_enable(SPI0);

    SET_SPI0_NSS_LOW

    /* SPI master start transfer */
    spi_master_transfer_start(SPI0, SPI_TRANS_START);

    /* wait for transmit complete */
    while(send_n < ARRAYSIZE){
        while(RESET == spi_i2s_flag_get(SPI1, SPI_FLAG_TP));
        spi_i2s_data_transmit(SPI1, spi1_send_array[send_n]);
        while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TP));
        spi_i2s_data_transmit(SPI0, spi0_send_array[send_n++]);
        while(RESET == spi_i2s_flag_get(SPI1, SPI_FLAG_RP));
        spi1_receive_array[receive_n] = spi_i2s_data_receive(SPI1);
        while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_RP));
        spi0_receive_array[receive_n++] = spi_i2s_data_receive(SPI0);
    }

    SET_SPI0_NSS_HIGH

    /* compare receive data with send data */
    if(memory_compare(spi1_receive_array, spi0_send_array, ARRAYSIZE))
        gd_eval_led_on(LED1);
    else
        gd_eval_led_off(LED1);

    /* compare receive data with send data */
    if(memory_compare(spi0_receive_array, spi1_send_array, ARRAYSIZE))
        gd_eval_led_on(LED2);
    else
        gd_eval_led_off(LED2);

    while(1);
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

/*!
    \brief      configure different peripheral clocks
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable the peripherals clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOG);    
    rcu_periph_clock_enable(RCU_SPI0);
    rcu_periph_clock_enable(RCU_SPI1);      
    rcu_spi_clock_config(IDX_SPI0, RCU_SPISRC_PLL0Q);
    rcu_spi_clock_config(IDX_SPI1, RCU_SPISRC_PLL0Q);  
}

/*!
    \brief      configure the GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* connect port to SPI0_NSS->PA4
                       SPI0_SCK->PA5
                       SPI0_MISO->PB4
                       SPI0_MOSI->PA7 */
    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_4);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_4);

    gpio_af_set(GPIOA, GPIO_AF_5, GPIO_PIN_5|GPIO_PIN_7);
    
    /* connect port to SPI1_NSS->PB12
                       SPI1_SCK->PB13
                       SPI1_MISO->PG2
                       SPI1_MOSI->PG3 */
    gpio_af_set(GPIOB, GPIO_AF_5, GPIO_PIN_4|GPIO_PIN_12|GPIO_PIN_13);
    gpio_af_set(GPIOG, GPIO_AF_5, GPIO_PIN_2|GPIO_PIN_3);  
    
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5|GPIO_PIN_7);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_5|GPIO_PIN_7);
    
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4|GPIO_PIN_12|GPIO_PIN_13);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_4|GPIO_PIN_12|GPIO_PIN_13);
    
    gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_2|GPIO_PIN_3);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_2|GPIO_PIN_3);
}

/*!
    \brief      configure the SPI peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void spi_config(void)
{
    spi_parameter_struct spi_init_struct;
    /* deinitialize SPI and the parameters */
    spi_i2s_deinit(SPI0);
    spi_i2s_deinit(SPI1);
    spi_struct_para_init(&spi_init_struct);

    /* SPI0 parameter configuration */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.data_size            = SPI_DATASIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_16;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI0, &spi_init_struct);

    /* SPI1 parameter configuration */
    spi_init_struct.device_mode          = SPI_SLAVE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init(SPI1, &spi_init_struct);

    /* enable SPI byte access */
    spi_byte_access_enable(SPI0);
    spi_byte_access_enable(SPI1);

    /* SPI NSS pin high level in software mode */
    spi_nss_internal_high(SPI0);
}

/*!
    \brief      memory compare function
    \param[in]  src: source data pointer
    \param[in]  dst: destination data pointer
    \param[in]  length: the compare data length
    \param[out] none
    \retval     ErrStatus: ERROR or SUCCESS
*/
ErrStatus memory_compare(uint8_t* src, uint8_t* dst, uint8_t length) 
{
    while(length--){
        if(*src++ != *dst++)
            return ERROR;
    }
    return SUCCESS;
}
