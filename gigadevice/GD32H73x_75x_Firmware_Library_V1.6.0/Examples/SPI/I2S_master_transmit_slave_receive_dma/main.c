/*!
    \file    main.c
    \brief   I2S master send and slave receive communication use DMA 
    
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
#include "gd32h759i_eval.h"
#include <stdio.h>

#define ARRAYSIZE         10

uint8_t spi0_send_array[ARRAYSIZE] = {0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA};
uint8_t spi1_receive_array[ARRAYSIZE]; 

ErrStatus memory_compare(uint8_t* src, uint8_t* dst, uint8_t length);
void cache_enable(void);
void rcu_config(void);
void gpio_config(void);
void dma_config(void);
void spi_config(void);

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
    /* initialize the led */
    gd_eval_led_init(LED1);
    /* peripheral clock enable */
    rcu_config();
    /* GPIO config */
    gpio_config();
    /* DMA config */
    dma_config();
    /* SPI config */
    spi_config();

    /* I2S enable */
    i2s_enable(SPI1);
    i2s_enable(SPI0);

    /* SPI DMA enable */
    spi_dma_enable(SPI1, SPI_DMA_RECEIVE);
    spi_dma_enable(SPI0, SPI_DMA_TRANSMIT);

    /* SPI master start transfer */
    spi_master_transfer_start(SPI0, SPI_TRANS_START);

    /* wait DMA transmit complete */
    while(!dma_flag_get(DMA0, DMA_CH0, DMA_FLAG_FTF));
    while(!dma_flag_get(DMA0, DMA_CH3, DMA_FLAG_FTF));

    /* compare receive data with send data */
    if(ERROR != memory_compare(spi1_receive_array, spi0_send_array, ARRAYSIZE)){
        gd_eval_led_on(LED1);
    }else{
        gd_eval_led_off(LED1);
    }

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
    rcu_periph_clock_enable(RCU_DMA0);
    rcu_periph_clock_enable(RCU_DMAMUX);     
}

/*!
    \brief      configure the GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* I2S0 GPIO config: I2S0_WS/PA4, I2S0_CK/PA5, I2S0_SD/PA7 */
    gpio_af_set(GPIOA, GPIO_AF_5, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7);
    
    /* I2S1 GPIO config: I2S1_WS/PB12, I2S1_CK/PB13, I2S1_SD/PG3 */
    gpio_af_set(GPIOB, GPIO_AF_5, GPIO_PIN_12|GPIO_PIN_13);        
    gpio_af_set(GPIOG, GPIO_AF_5, GPIO_PIN_3); 

    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ,GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7);

    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_12 | GPIO_PIN_13);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_12|GPIO_PIN_13);

    gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_3); 
}

/*!
    \brief      configure the DMA peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dma_config(void)
{
    dma_single_data_parameter_struct dma_init_struct;
    
    /* SPI0 transmit DMA config: DMA_CH0 */
    dma_deinit(DMA0, DMA_CH0);
    dma_single_data_para_struct_init(&dma_init_struct);

    dma_init_struct.request = DMA_REQUEST_SPI0_TX;
    dma_init_struct.periph_addr  = (uint32_t)&SPI_TDATA(SPI0);
    dma_init_struct.memory0_addr  = (uint32_t)spi0_send_array;
    dma_init_struct.direction    = DMA_MEMORY_TO_PERIPH; 
    dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
    dma_init_struct.priority     = DMA_PRIORITY_LOW;
    dma_init_struct.number       = ARRAYSIZE;
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_single_data_mode_init(DMA0, DMA_CH0, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH0);

    /* SPI1 receive DMA config: DMA_CH3 */
    dma_deinit(DMA0, DMA_CH3);
    dma_init_struct.request = DMA_REQUEST_SPI1_RX;
    dma_init_struct.periph_addr  = (uint32_t)&SPI_RDATA(SPI1);
    dma_init_struct.memory0_addr  = (uint32_t)spi1_receive_array;
    dma_init_struct.direction    = DMA_PERIPH_TO_MEMORY;
    dma_init_struct.priority     = DMA_PRIORITY_MEDIUM;
    dma_single_data_mode_init(DMA0, DMA_CH3, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH3);

    /* enable DMA channel */
    dma_channel_enable(DMA0, DMA_CH0);
    dma_channel_enable(DMA0, DMA_CH3);
}

/*!
    \brief      configure the SPI peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void spi_config(void)
{
    spi_i2s_deinit(SPI0);
    spi_i2s_deinit(SPI1);

    i2s_init(SPI0, I2S_MODE_MASTERTX, I2S_STD_MSB, I2S_CKPL_LOW);
    i2s_psc_config(SPI0, I2S_AUDIOSAMPLE_96K, I2S_FRAMEFORMAT_DT16B_CH16B, I2S_MCKOUT_DISABLE);
    
    i2s_init(SPI1, I2S_MODE_SLAVERX, I2S_STD_MSB, I2S_CKPL_LOW);
    i2s_psc_config(SPI1, I2S_AUDIOSAMPLE_96K, I2S_FRAMEFORMAT_DT16B_CH16B, I2S_MCKOUT_DISABLE);
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
    while (length--){
        if (*src++ != *dst++)
            return ERROR;
    }
    return SUCCESS;
}
