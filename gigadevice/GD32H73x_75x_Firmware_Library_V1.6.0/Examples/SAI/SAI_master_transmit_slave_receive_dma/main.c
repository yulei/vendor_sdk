/*!
    \file    main.c
    \brief   SAI master send and slave receive communication use DMA

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

#define ARRAYSIZE         128

__attribute__((aligned(32))) uint32_t sai_send_array[ARRAYSIZE];
__attribute__((aligned(32))) uint32_t sai_receive_array[ARRAYSIZE];

void rcu_config(void);
void gpio_config(void);
void sai_config(void);
void dma_config(void);
void cache_enable(void);
ErrStatus memory_compare(uint32_t *src, uint32_t *dst, uint32_t length);

static void fill_array_value(void);

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
    /* initialized led2 */
    gd_eval_led_init(LED2);
    /* initialized the array value */
    fill_array_value();

    /* Clean tx buffer */
    SCB_CleanDCache_by_Addr((uint32_t *)sai_send_array, ARRAYSIZE * 4);

    /* configure RCU */
    rcu_config();
    /* configure GPIO */
    gpio_config();
    /* configure SAI */
    sai_config();
    /* configure DMA */
    dma_config();

    /* enable sai dma */
    sai_dma_enable(SAI0, SAI_BLOCK0);
    sai_dma_enable(SAI1, SAI_BLOCK1);

    /* enable dma channel */
    dma_channel_enable(DMA0, DMA_CH0);
    dma_channel_enable(DMA1, DMA_CH1);

    /* enable sai */
    sai_enable(SAI1, SAI_BLOCK1);
    sai_enable(SAI0, SAI_BLOCK0);

    /* wait DMA transmit complete */
    while(!dma_flag_get(DMA0, DMA_CH0, DMA_INTF_FTFIF));
    while(!dma_flag_get(DMA1, DMA_CH1, DMA_INTF_FTFIF));

    sai_disable(SAI0, SAI_BLOCK0);
    sai_disable(SAI1, SAI_BLOCK1);
    sai_flag_clear(SAI0, SAI_BLOCK0, SAI_FLAG_OUERR);
    sai_flag_clear(SAI1, SAI_BLOCK1, SAI_FLAG_OUERR);

    /* Invalidate rx buffer */
    SCB_InvalidateDCache_by_Addr((uint32_t *)sai_receive_array, ARRAYSIZE * 4);
    /* compare receive data with send data */
    if(memory_compare(sai_receive_array, sai_send_array, ARRAYSIZE)) {
        gd_eval_led_on(LED2);
    } else {
        gd_eval_led_off(LED2);
    }

    /* Infinite loop */
    while(1) {
    }
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

    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA0);
    rcu_periph_clock_enable(RCU_DMA1);
    rcu_periph_clock_enable(RCU_DMAMUX);

    /* SAI_CK = PLL1_VCO/PLL1P = 271/24 = 11.291 Mhz */
    rcu_pll_input_output_clock_range_config(IDX_PLL1, RCU_PLL1RNG_1M_2M, RCU_PLL1VCO_192M_836M);
    rcu_pll1_config(25, 271, 24, 24, 24);
    rcu_sai_clock_config(IDX_SAI0, RCU_SAISRC_PLL1P);
    rcu_pll_clock_output_enable(RCU_PLL1P);
    rcu_osci_on(RCU_PLL1_CK);
    if(ERROR == rcu_osci_stab_wait(RCU_PLL1_CK)) {
        while(1) {
        }
    }

    /* enable SAI clock */
    rcu_periph_clock_enable(RCU_SAI0);
    rcu_periph_clock_enable(RCU_SAI1);
}

/*!
    \brief      configure the GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* configure GPIO pins of SAI0: SAI0_MCLK0(PE2) SAI0_FS0(PE4) SAI0_SCK0(PE5) SAI0_SD0(PE6) */
    gpio_af_set(GPIOE, GPIO_AF_6, GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6);
    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE,
                  GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ,
                            GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6);

    /* configure GPIO pins of SAI1: SAI1_MCLK1(PE14) SAI1_FS1(PE13) SAI1_SCK1(PE12) SAI1_SD1(PE11) */
    gpio_af_set(GPIOE, GPIO_AF_10, GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14);
    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE,
                  GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ,
                            GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14);
}

/*!
    \brief      configure the DMA channel
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dma_config(void)
{
    dma_single_data_parameter_struct dma_init_struct;
    /* initialize DMA0 channel 0 */
    dma_deinit(DMA0, DMA_CH0);
    /* initialize DMA1 channel 1 */
    dma_deinit(DMA1, DMA_CH1);

    dma_single_data_para_struct_init(&dma_init_struct);
    /* configure SAI0_B0 transmit dma: DMA0_CH0 */
    dma_init_struct.request             = DMA_REQUEST_SAI0_B0;
    dma_init_struct.direction           = DMA_MEMORY_TO_PERIPH;
    dma_init_struct.memory0_addr        = (uint32_t)sai_send_array;
    dma_init_struct.memory_inc          = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_32BIT;
    dma_init_struct.number              = ARRAYSIZE;
    dma_init_struct.periph_addr         = (uint32_t)(&SAI_DATA(SAI0, SAI_BLOCK0));
    dma_init_struct.periph_inc          = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.priority            = DMA_PRIORITY_ULTRA_HIGH;
    dma_single_data_mode_init(DMA0, DMA_CH0, &dma_init_struct);

    /* configure SAI1_B1 recevie dma: DMA1_CH1 */
    dma_init_struct.request             = DMA_REQUEST_SAI1_B1;
    dma_init_struct.direction           = DMA_PERIPH_TO_MEMORY;
    dma_init_struct.memory0_addr        = (uint32_t)sai_receive_array;
    dma_init_struct.memory_inc          = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_32BIT;
    dma_init_struct.number              = ARRAYSIZE;
    dma_init_struct.periph_addr         = (uint32_t)(&SAI_DATA(SAI1, SAI_BLOCK1));
    dma_init_struct.periph_inc          = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.priority            = DMA_PRIORITY_LOW;
    dma_single_data_mode_init(DMA1, DMA_CH1, &dma_init_struct);
}

/*!
    \brief      configure the sai peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void sai_config(void)
{
    sai_parameter_struct sai_structure;
    sai_frame_parameter_struct sai_frame_structure;
    sai_slot_parameter_struct sai_slot_structure;

    sai_struct_para_init(&sai_structure);
    sai_frame_struct_para_init(&sai_frame_structure);
    sai_slot_struct_para_init(&sai_slot_structure);

    /* initialize SAI0_B0 frame */
    sai_frame_structure.frame_width            = 64;
    sai_frame_structure.frame_sync_width       = 32;
    sai_frame_structure.frame_sync_function    = SAI_FS_FUNC_START_CHANNEL;
    sai_frame_structure.frame_sync_polarity    = SAI_FS_POLARITY_LOW;
    sai_frame_structure.frame_sync_offset      = SAI_FS_OFFSET_BEGINNING;
    sai_frame_init(SAI0, SAI_BLOCK0, &sai_frame_structure);

    /* initialize SAI0_B0 slot */
    sai_slot_structure.slot_number             = 2;
    sai_slot_structure.slot_width              = SAI_SLOT_WIDTH_32BIT;
    sai_slot_structure.data_offset             = 0;
    sai_slot_structure.slot_active             = SAI_SLOT_ACTIVE_ALL;
    sai_slot_init(SAI0, SAI_BLOCK0, &sai_slot_structure);

    /* initialize SAI0_B0  */
    sai_structure.operating_mode               = SAI_MASTER_TRANSMITTER;
    sai_structure.protocol                     = SAI_PROTOCOL_POLYMORPHIC;
    sai_structure.data_width                   = SAI_DATAWIDTH_32BIT;
    sai_structure.shift_dir                    = SAI_SHIFT_MSB;
    sai_structure.sample_edge                  = SAI_SAMPEDGE_RISING;
    sai_structure.sync_mode                    = SAI_SYNCMODE_ASYNC;
    sai_structure.output_drive                 = SAI_OUTPUT_WITH_SAIEN;
    sai_structure.clk_div_bypass               = SAI_CLKDIV_BYPASS_OFF;
    sai_structure.mclk_div                     = SAI_MCLKDIV_1;
    sai_structure.mclk_oversampling            = SAI_MCLK_OVERSAMP_256;
    sai_structure.mclk_enable                  = SAI_MCLK_ENABLE;
    sai_structure.fifo_threshold               = SAI_FIFOTH_EMPTY;
    sai_init(SAI0, SAI_BLOCK0, &sai_structure);

    /* initialize SAI1_B1 frame */
    sai_frame_init(SAI1, SAI_BLOCK1, &sai_frame_structure);
    /* initialize SAI1_B1 slot */
    sai_slot_init(SAI1, SAI_BLOCK1, &sai_slot_structure);

    /* initialize SAI1_B1 */
    sai_structure.operating_mode               = SAI_SLAVE_RECEIVER;
    sai_init(SAI1, SAI_BLOCK1, &sai_structure);
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
    while(length--) {
        if(*src++ != *dst++) {
            return ERROR;
        }
    }
    return SUCCESS;
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
    for(i = 0U; i < ARRAYSIZE; i++) {
        sai_send_array[i] = i;
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
