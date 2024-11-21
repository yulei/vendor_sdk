/*!
    \file    main.c
    \brief   I2S master and slave fullduplex communication using DMA

    \version 2023-12-31, V1.4.0, firmware for GD32E50x
*/

/*
    Copyright (c) 2023, GigaDevice Semiconductor Inc.

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

#include "gd32e50x.h"
#include "gd32e507z_eval.h"

#define ARRAYSIZE         8

uint16_t i2s1_txbuffer[ARRAYSIZE] = {0x8121, 0x7342, 0x9563, 0x6784, 0xA9A5, 0x5BC6, 0xBDE7, 0x4F05};
uint16_t i2s1_rxbuffer[ARRAYSIZE] = {0};
uint16_t i2s2_txbuffer[ARRAYSIZE] = {0x1234, 0x2335, 0x4564, 0x5675, 0x9854, 0x7551, 0x0503, 0x1CBE};
uint16_t i2s2_rxbuffer[ARRAYSIZE] = {0};


void rcu_config(void);
void gpio_config(void);
void dma_config(void);
void spi_config(void);
ErrStatus memory_compare(uint16_t* src, uint16_t* dst, uint16_t length);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* initialize LED1 and LED2 */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);

    /* enable peripheral clock */
    rcu_config();
    /* configure GPIO */
    gpio_config();
    /* configure DMA */
    dma_config();
    /* configure I2S */
    spi_config();

    /* enbale I2S and I2S_ADD*/
    i2s_enable(SPI2);
    i2s_enable(I2S2_ADD);
    i2s_enable(I2S1_ADD);
    i2s_enable(SPI1);

    /* enable DMA channel */
    dma_channel_enable(DMA1,DMA_CH3);
    dma_channel_enable(DMA1,DMA_CH0);
    dma_channel_enable(DMA0,DMA_CH4);
    dma_channel_enable(DMA0,DMA_CH5);

    /* enable SPI DMA */
    spi_dma_enable(I2S1_ADD, SPI_DMA_RECEIVE);
    spi_dma_enable(SPI2, SPI_DMA_RECEIVE);
    spi_dma_enable(I2S2_ADD, SPI_DMA_TRANSMIT);
    spi_dma_enable(SPI1, SPI_DMA_TRANSMIT);

    /* wait DMA transmit completed */
    while(!dma_flag_get(DMA0,DMA_CH4,DMA_INTF_FTFIF)){
    }
    while(!dma_flag_get(DMA0,DMA_CH5,DMA_INTF_FTFIF)){
    }
    while(!dma_flag_get(DMA1,DMA_CH0,DMA_INTF_FTFIF)){
    }
    while(!dma_flag_get(DMA1,DMA_CH3,DMA_INTF_FTFIF)){
    }

    /* compare receive data with send data */
    if(ERROR != memory_compare(i2s2_txbuffer, i2s1_rxbuffer, ARRAYSIZE)) {
        gd_eval_led_on(LED1);
    } else {
        gd_eval_led_off(LED1);
    }

    /* compare receive data with send data */
    if(ERROR != memory_compare(i2s1_txbuffer, i2s2_rxbuffer, ARRAYSIZE)) {
        gd_eval_led_on(LED2);
    } else {
        gd_eval_led_off(LED2);
    }
    while(1) {
    }
}

/*!
    \brief      configure different peripheral clocks
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
#ifdef GD32E50X_CL
    rcu_pll2_config(RCU_PLL2_MUL8);
    rcu_osci_on(RCU_PLL2_CK);
    while((RCU_CTL & RCU_CTL_PLL2STB) == 0)
    {
    }
    rcu_i2s1_clock_config(RCU_I2S1SRC_CKPLL2_MUL2);
    rcu_i2s2_clock_config(RCU_I2S2SRC_CKPLL2_MUL2);
#endif

    rcu_periph_clock_enable(RCU_SPI1);
    rcu_periph_clock_enable(RCU_SPI2);
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_DMA0);
    rcu_periph_clock_enable(RCU_DMA1);
    rcu_periph_clock_enable(RCU_AF);
}

/*!
    \brief      configure the GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    gpio_afio_port_config(AFIO_PB14_I2S1_CFG,ENABLE);
    gpio_afio_port_config(AFIO_PC11_I2S2_CFG,ENABLE);

    /* I2S1 GPIO config: I2S1_WS/PB12, I2S1_CK/PB13,I2S_ADD_SD/PB14 I2S_SD/PB15 */
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12 | GPIO_PIN_13 |GPIO_PIN_15);
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_14);
    
    /* I2S2 GPIO config: I2S2_WS/PA4, I2S2_CK/PC10, I2S2_ADD_SD/PC11 I2S2_SD/PC12 */
    gpio_pin_remap_config(GPIO_SPI2_REMAP, ENABLE);

    gpio_init(GPIOC, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_10 | GPIO_PIN_12);
    gpio_init(GPIOC, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_4);
}

/*!
    \brief      configure the DMA peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dma_config(void)
{
    dma_parameter_struct dma_init_struct;

    /* I2S1_SD transmit  */
    dma_deinit(DMA0, DMA_CH4);
    dma_init_struct.periph_addr  = (uint32_t)&SPI_DATA(SPI1);
    dma_init_struct.memory_addr  = (uint32_t)i2s1_txbuffer;
    dma_init_struct.direction    = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_16BIT;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_init_struct.priority     = DMA_PRIORITY_LOW;
    dma_init_struct.number       = 8;
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_init(DMA0, DMA_CH4, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH4);
    dma_memory_to_memory_disable(DMA0, DMA_CH4);
    
    /* I2S1_ADD receive dma config */
    dma_deinit(DMA0, DMA_CH5);
    dma_init_struct.periph_addr  = (uint32_t)&SPI_DATA(I2S1_ADD);
    dma_init_struct.memory_addr  = (uint32_t)i2s1_rxbuffer;
    dma_init_struct.direction    = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_16BIT;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_init_struct.priority     = DMA_PRIORITY_HIGH;
    dma_init_struct.number       = 8;
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_init(DMA0, DMA_CH5, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH5);
    dma_memory_to_memory_disable(DMA0, DMA_CH5);

    /* I2S2_ADD transmit dma config */
    dma_deinit(DMA1, DMA_CH3);
    dma_init_struct.periph_addr  = (uint32_t)&SPI_DATA(I2S2_ADD);
    dma_init_struct.memory_addr  = (uint32_t)i2s2_txbuffer;
    dma_init_struct.direction    = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_16BIT;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_init_struct.priority     = DMA_PRIORITY_MEDIUM;
    dma_init_struct.number       = 8;
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_init(DMA1, DMA_CH3, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA1, DMA_CH3);
    dma_memory_to_memory_disable(DMA1, DMA_CH3);

    /* I2S2_SD receive dma config */
    dma_deinit(DMA1, DMA_CH0);
    dma_init_struct.periph_addr  = (uint32_t)&SPI_DATA(SPI2);
    dma_init_struct.memory_addr  = (uint32_t)i2s2_rxbuffer;
    dma_init_struct.direction    = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_16BIT;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
    dma_init_struct.number       = 8;
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_init(DMA1, DMA_CH0, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA1, DMA_CH0);
    dma_memory_to_memory_disable(DMA1, DMA_CH0);

}

/*!
    \brief      configure the SPI peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void spi_config(void)
{

    spi_i2s_deinit(SPI2);
    spi_i2s_deinit(SPI1);

    i2s_init(SPI1, I2S_MODE_MASTERTX, I2S_STD_PHILLIPS, I2S_CKPL_LOW);
    i2s_psc_config(SPI1, I2S_AUDIOSAMPLE_11K, I2S_FRAMEFORMAT_DT16B_CH16B, I2S_MCKOUT_DISABLE);
    i2s_full_duplex_mode_config(I2S1_ADD, I2S_MODE_MASTERTX, I2S_STD_PHILLIPS, I2S_CKPL_LOW, I2S_FRAMEFORMAT_DT16B_CH16B);

    i2s_init(SPI2, I2S_MODE_SLAVERX, I2S_STD_PHILLIPS, I2S_CKPL_LOW);
    i2s_psc_config(SPI2, I2S_AUDIOSAMPLE_11K, I2S_FRAMEFORMAT_DT16B_CH16B, I2S_MCKOUT_DISABLE);
    i2s_full_duplex_mode_config(I2S2_ADD, I2S_MODE_SLAVERX, I2S_STD_PHILLIPS, I2S_CKPL_LOW, I2S_FRAMEFORMAT_DT16B_CH16B);

}

/*!
    \brief      memory compare function
    \param[in]  src: source data pointer
    \param[in]  dst: destination data pointer
    \param[in]  length: the compare data length
    \param[out] none
    \retval     ErrStatus : ERROR or SUCCESS
*/
ErrStatus memory_compare(uint16_t* src, uint16_t* dst, uint16_t length) 
{
    while(length--){
        if(*src++ != *dst++)
            return ERROR;
    }
    return SUCCESS;
}
