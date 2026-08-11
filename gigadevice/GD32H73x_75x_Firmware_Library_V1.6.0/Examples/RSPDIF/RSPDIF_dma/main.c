/*!
    \file    main.c
    \brief   RSPDIF receive data using DMA mode
    
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

#define FRAMESIZE         192*2
#define ARRAYSIZE         FRAMESIZE*32

__attribute__((aligned(32))) uint32_t sai_send_array[ARRAYSIZE];
__attribute__((aligned(32))) uint32_t rspdif_receive_array[ARRAYSIZE];

static void fill_array_value(void);
void cache_enable(void);
void rcu_config(void);
void gpio_config(void);
void sai_config(void);
void rspdif_config(void);
void dma_config(void);
ErrStatus memory_compare(uint32_t *src, uint32_t *dst, uint32_t length);

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
    /* initialize LED1 */
    gd_eval_led_init(LED1);
    /* enable peripheral clock */
    rcu_config();
    /* configure GPIO */
    gpio_config();
    /* configure RSPDIF */
    rspdif_config();
    /* configure SAI */
    sai_config();
    /* fill the buffer with specified value */
    fill_array_value();
    SCB_CleanDCache_by_Addr((uint32_t*)sai_send_array, ARRAYSIZE*4);
    SCB_CleanDCache_by_Addr((uint32_t*)rspdif_receive_array, ARRAYSIZE*4);
    /* configure DMA*/
    dma_config();

    /* enable dma channel SAI*/
    dma_channel_enable(DMA0, DMA_CH1);
    /* enable DMA channel 0 */
    dma_channel_enable(DMA0, DMA_CH0);
    
    /* enable SAI DMA */
    sai_dma_enable(SAI0, SAI_BLOCK0);
    /* enable RSPDIF DMA */
    rspdif_dma_enable();

    /* enable RSPDIF receiver */
    rspdif_enable(RSPDIF_STATE_RCV);
    /* enable SAI */
    sai_enable(SAI0, SAI_BLOCK0);
    
    /* wait dma transmit complete */
    while(!dma_flag_get(DMA0, DMA_CH1, DMA_INTF_FTFIF)) {
    }
    while(!dma_flag_get(DMA0, DMA_CH0, DMA_INTF_FTFIF)) {
    }

    /* compare receive data with send data */
    if(ERROR != memory_compare(sai_send_array, rspdif_receive_array, ARRAYSIZE)) {
        gd_eval_led_on(LED1);
    } else {
        gd_eval_led_off(LED1);
    }
    
    while (1){
    }
}

/*!
    \brief      fill the buffer with specified value
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void fill_array_value(void)
{
    uint16_t i;
    for(i = 0U;i < ARRAYSIZE;i++){
        sai_send_array[i] = i;
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
    /* Enable I-Cache */
    SCB_EnableICache();

    /* Enable D-Cache */
    SCB_EnableDCache();
}

/*!
    \brief      configure the system clocks
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOE);
    rcu_periph_clock_enable(RCU_GPIOD);
    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA0);
    rcu_periph_clock_enable(RCU_DMAMUX);

    /* SAI_CK = PLL1_VCO/PLL1P = 271/24 = 11.291 Mhz */
    rcu_pll_input_output_clock_range_config(IDX_PLL1,RCU_PLL1RNG_1M_2M,RCU_PLL1VCO_192M_836M);
    rcu_pll1_config(25,271,24,24,24);
    rcu_sai_clock_config(IDX_SAI0,RCU_SAISRC_PLL1P);
    rcu_pll_clock_output_enable(RCU_PLL1P);
    rcu_osci_on(RCU_PLL1_CK);
    if(ERROR == rcu_osci_stab_wait(RCU_PLL1_CK)){
        while(1){
        }
    }
    /* enable SAI clock */
    rcu_periph_clock_enable(RCU_SAI0);
    
    /* enable RSPDIF clock */
    rcu_periph_clock_enable(RCU_RSPDIF);
}

/*!
    \brief      configure the GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* configure GPIO pins of SAI0: SAI0_SD0(PE6) */
    gpio_af_set(GPIOE, GPIO_AF_6, GPIO_PIN_6);
    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_6);

    /* configure GPIO pins of RSPDIF: RSPDIF_CH0(PD7) */
    gpio_af_set(GPIOD, GPIO_AF_9, GPIO_PIN_7);
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_7);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_7);
} 

/*!
    \brief      configure the RSPDIF peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rspdif_config(void)
{
    rspdif_parameter_struct rspdif_structure;
    
    rspdif_struct_para_init(&rspdif_structure);
    rspdif_structure.input_sel          = RSPDIF_INPUT_IN0;
    rspdif_structure.max_retrie         = RSPDIF_MAXRETRIES_63;
    rspdif_structure.wait_activity      = RSPDIF_WAIT_FOR_ACTIVITY_ON;
    rspdif_structure.channel_sel        = RSPDIF_CHANNEL_A;
    rspdif_structure.sample_format      = RSPDIF_DATAFORMAT_LSB;
    rspdif_structure.sound_mode         = RSPDIF_STEREOMODE_ENABLE;
    rspdif_structure.pre_type           = RSPDIF_PREAMBLE_TYPE_MASK_OFF;
    rspdif_structure.channel_status_bit = RSPDIF_CHANNEL_STATUS_MASK_OFF;
    rspdif_structure.validity_bit       = RSPDIF_VALIDITY_MASK_OFF;
    rspdif_structure.parity_error_bit   = RSPDIF_PERROR_MASK_OFF;
    rspdif_structure.symbol_clk         = RSPDIF_SYMBOL_CLK_ON;
    rspdif_structure.bak_symbol_clk     = RSPDIF_BACKUP_SYMBOL_CLK_ON;
    rspdif_init(&rspdif_structure);
}

/*!
    \brief      configure the SAI peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void sai_config(void)
{
    sai_parameter_struct sai_structure;
    sai_frame_parameter_struct sai_frame_structure;

    sai_struct_para_init(&sai_structure);
    sai_frame_struct_para_init( &sai_frame_structure);

    /* initialize SAI0_B0 frame */
    sai_frame_structure.frame_width            = 64;
    sai_frame_structure.frame_sync_width       = 32;
    sai_frame_structure.frame_sync_function    = SAI_FS_FUNC_START_CHANNEL;
    sai_frame_structure.frame_sync_polarity    = SAI_FS_POLARITY_LOW;
    sai_frame_structure.frame_sync_offset      = SAI_FS_OFFSET_BEGINNING;
    sai_frame_init(SAI0, SAI_BLOCK0, &sai_frame_structure);

    /* initialize SAI0_B0 */
    sai_structure.operating_mode               = SAI_MASTER_TRANSMITTER;
    sai_structure.protocol                     = SAI_PROTOCOL_SPDIF;
    sai_structure.data_width                   = SAI_DATAWIDTH_32BIT;
    sai_structure.shift_dir                    = SAI_SHIFT_MSB;
    sai_structure.sample_edge                  = SAI_SAMPEDGE_RISING;
    sai_structure.sync_mode                    = SAI_SYNCMODE_ASYNC;
    sai_structure.output_drive                 = SAI_OUTPUT_WITH_SAIEN;
    sai_structure.clk_div_bypass               = SAI_CLKDIV_BYPASS_ON;
    sai_structure.mclk_div                     = SAI_MCLKDIV_4;
    sai_structure.mclk_oversampling            = SAI_MCLK_OVERSAMP_256;
    sai_structure.mclk_enable                  = SAI_MCLK_DISABLE;
    sai_structure.fifo_threshold               = SAI_FIFOTH_EMPTY;
    sai_init(SAI0, SAI_BLOCK0, &sai_structure);
}

/*!
    \brief      configure the DMA peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dma_config(void)
{
    dma_single_data_parameter_struct sai_dma_init_struct;
    dma_single_data_parameter_struct rspdif_dma_init_struct;
    dma_single_data_para_struct_init(&sai_dma_init_struct);
    dma_single_data_para_struct_init(&rspdif_dma_init_struct);
    
    /* deinitialize DMA0 channel 1 */
    dma_deinit(DMA0, DMA_CH1);

    /* configure SAI0_B0 transmit DMA: DMA0_CH1 */
    sai_dma_init_struct.request             = DMA_REQUEST_SAI0_B0;
    sai_dma_init_struct.direction           = DMA_MEMORY_TO_PERIPH;
    sai_dma_init_struct.memory0_addr        = (uint32_t)sai_send_array;
    sai_dma_init_struct.memory_inc          = DMA_MEMORY_INCREASE_ENABLE;
    sai_dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_32BIT;
    sai_dma_init_struct.number              = ARRAYSIZE;
    sai_dma_init_struct.periph_addr         = (uint32_t)(&SAI_DATA(SAI0,SAI_BLOCK0));
    sai_dma_init_struct.periph_inc          = DMA_PERIPH_INCREASE_DISABLE;
    sai_dma_init_struct.priority            = DMA_PRIORITY_HIGH;
    dma_single_data_mode_init(DMA0, DMA_CH1, &sai_dma_init_struct);
    dma_circulation_enable(DMA0, DMA_CH1);

    /* deinitialize DMA0 channel 0 */
    dma_deinit(DMA0, DMA_CH0);
    
    /* configure RSPDIF receive DMA: DMA0_CH0 */
    rspdif_dma_init_struct.request             = DMA_REQUEST_RSPDIF_DATA;
    rspdif_dma_init_struct.direction           = DMA_PERIPH_TO_MEMORY;
    rspdif_dma_init_struct.periph_addr         = (uint32_t)(&RSPDIF_DATA);
    rspdif_dma_init_struct.memory0_addr        = (uint32_t)(&rspdif_receive_array);
    rspdif_dma_init_struct.periph_inc          = DMA_PERIPH_INCREASE_DISABLE;
    rspdif_dma_init_struct.memory_inc          = DMA_MEMORY_INCREASE_ENABLE;
    rspdif_dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_32BIT;
    rspdif_dma_init_struct.number              = ARRAYSIZE;
    rspdif_dma_init_struct.priority            = DMA_PRIORITY_ULTRA_HIGH;
    dma_single_data_mode_init(DMA0, DMA_CH0, &rspdif_dma_init_struct);
    /* enable DMA circulation mode */
    dma_circulation_enable(DMA0, DMA_CH0);
}


/*!
    \brief      memory compare function
    \param[in]  src: source data pointer
    \param[in]  dst: destination data pointer
    \param[in]  length: the compare data length
    \param[out] none
    \retval     ErrStatus : ERROR or SUCCESS
*/
ErrStatus memory_compare(uint32_t *src, uint32_t *dst, uint32_t length)
{
    uint32_t i;

    /* skip the first frame of data, as the first frame is used for synchronization */
    for(i = 0; i< (length - FRAMESIZE); i++) {
        *src = *src + FRAMESIZE;
        if(*src++ != ((*dst++) & 0xCFFFFFFF)) {
            return ERROR;
        }
    }

    return SUCCESS;
}
