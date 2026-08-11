/*!
    \file    main.c
    \brief   MDMA bufffer transfer example

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

#define DTCM_RAM_ADDR             ((uint32_t)0x20000000U)
#define BUFFER_SIZE               (32U)
#define USART0_RDATA_ADDRESS      ((uint32_t)&USART_RDATA(USART0))
#define DMA1_INTC_ADDRESS         ((uint32_t)&DMA_INTC0(DMA1))
#define DMA1_INTC_FTFIFC2         BIT(21)

__attribute__((aligned(32))) uint8_t usart_rxbuffer[BUFFER_SIZE];
uint32_t mdma_rxbuffer;
ErrStatus status;

void cache_enable(void);
void dma_config(void);
void mdma_config(void);
static ErrStatus memory_compare(uint8_t *src, uint8_t *dst, uint16_t length);

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
    /* enable DMA1 clock */
    rcu_periph_clock_enable(RCU_DMA1);
    /* enable MDMA clock */
    rcu_periph_clock_enable(RCU_MDMA);
    /* enable DMAMUX clock */
    rcu_periph_clock_enable(RCU_DMAMUX);

    /* initialize USART */
    gd_eval_com_init(EVAL_COM);
    /* USART DMA enable*/
    usart_dma_receive_config(USART0, USART_RECEIVE_DMA_ENABLE);

    /* initialize LED */
    gd_eval_led_init(LED1);

    /* MDMA rx buffer located in DTCM */
    mdma_rxbuffer = DTCM_RAM_ADDR;
    /* invalidate the cache line */
    SCB_InvalidateDCache_by_Addr((uint32_t *)usart_rxbuffer, BUFFER_SIZE);
    /* configure MDMA */
    mdma_config();
    /* configure DMA */
    dma_config();
    /* wait MDMA channel 0 transfer done */
    while(!mdma_flag_get(MDMA_CH0, MDMA_FLAG_TCF));
    mdma_flag_clear(MDMA_CH0, MDMA_FLAG_TCF);
    /* check DMA1 channel2 full transfer flag status */
    while(dma_flag_get(DMA1, DMA_CH2, DMA_FLAG_FTF));

    /* compare mdma_rxbuffer with usart_rxbuffer */
    status = memory_compare((uint8_t *)usart_rxbuffer, (uint8_t *)mdma_rxbuffer, BUFFER_SIZE);

    if(SUCCESS == status) {
        gd_eval_led_on(LED1);
    } else {
        gd_eval_led_off(LED1);
    }

    while(1) {
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

/*!
    \brief      configure DMA
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dma_config(void)
{
    dma_single_data_parameter_struct dma_init_struct;
    /* deinitialize DMA1 channel2 */
    dma_deinit(DMA1, DMA_CH2);
    dma_single_data_para_struct_init(&dma_init_struct);
    /* config DMA1 channel2 for USART0 Rx request */
    dma_init_struct.request             = DMA_REQUEST_USART0_RX;
    dma_init_struct.periph_addr         = USART0_RDATA_ADDRESS;
    dma_init_struct.periph_inc          = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.memory0_addr        = (uint32_t)&usart_rxbuffer;
    dma_init_struct.memory_inc          = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
    dma_init_struct.circular_mode       = DMA_CIRCULAR_MODE_DISABLE;
    dma_init_struct.direction           = DMA_PERIPH_TO_MEMORY;
    dma_init_struct.number              = BUFFER_SIZE;
    dma_init_struct.priority            = DMA_PRIORITY_ULTRA_HIGH;
    dma_single_data_mode_init(DMA1, DMA_CH2, &dma_init_struct);
    /* enable DMA1 channel2 */
    dma_channel_enable(DMA1, DMA_CH2);
}

/*!
    \brief      configure MDMA
    \param[in]  none
    \param[out] none
    \retval     none
*/
void mdma_config(void)
{
    mdma_parameter_struct mdma_init_struct;
    /* deinitialize MDMA */
    mdma_deinit();
    /* initialize MDMA parameter structure */
    mdma_para_struct_init(&mdma_init_struct);
    /* configure full transfer finish event of DMA1 channel2 as MDMA trigger source */
    mdma_init_struct.request             = MDMA_REQUEST_DMA1_CH2_FTFIF;
    mdma_init_struct.trans_trig_mode     = MDMA_BUFFER_TRANSFER;
    mdma_init_struct.priority            = MDMA_PRIORITY_HIGH;
    mdma_init_struct.endianness          = MDMA_LITTLE_ENDIANNESS;
    /* configure start address, address increment mode, data width and burst mode for MDMA source and destination */
    mdma_init_struct.source_addr         = (uint32_t)&usart_rxbuffer;
    mdma_init_struct.destination_addr    = mdma_rxbuffer;
    mdma_init_struct.source_inc          = MDMA_SOURCE_INCREASE_8BIT;
    mdma_init_struct.dest_inc            = MDMA_DESTINATION_INCREASE_8BIT;
    mdma_init_struct.source_data_size    = MDMA_SOURCE_DATASIZE_8BIT;
    mdma_init_struct.dest_data_size      = MDMA_DESTINATION_DATASIZE_8BIT;
    mdma_init_struct.source_burst        = MDMA_SOURCE_BURST_SINGLE;
    mdma_init_struct.dest_burst          = MDMA_DESTINATION_BURST_SINGLE;
    /* configure bus type for source and destination */
    mdma_init_struct.source_bus          = MDMA_SOURCE_AXI;
    mdma_init_struct.destination_bus     = MDMA_DESTINATION_AHB_TCM;
    /* configure data alignment and pack mode */
    mdma_init_struct.data_alignment      = MDMA_DATAALIGN_PKEN;
    /* configure buffer transfer length and transfer byte count */
    mdma_init_struct.buff_trans_len      = BUFFER_SIZE;
    mdma_init_struct.tbytes_num_in_block = BUFFER_SIZE;
    /* write mask addr with mask data to clear DMA1 channel2's full transfer finish flag after MDMA transfer done */
    mdma_init_struct.mask_addr           = DMA1_INTC_ADDRESS;
    mdma_init_struct.mask_data           = DMA1_INTC_FTFIFC2;

    /* configure MDMA channel 0 */
    mdma_init(MDMA_CH0, &mdma_init_struct);
    /* enable the MDMA channel0 */
    mdma_channel_enable(MDMA_CH0);
}

/*!
    \brief      memory compare function
    \param[in]  src : source data
    \param[in]  dst : destination data
    \param[in]  length : the compare data length
    \param[out] none
    \retval     ErrStatus : ERROR or SUCCESS
*/
ErrStatus memory_compare(uint8_t *src, uint8_t *dst, uint16_t length)
{
    while(length--) {
        if(*src++ != *dst++) {
            return ERROR;
        }
    }
    return SUCCESS;
}
