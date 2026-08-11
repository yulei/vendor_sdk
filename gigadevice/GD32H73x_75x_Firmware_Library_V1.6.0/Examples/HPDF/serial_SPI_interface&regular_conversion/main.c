/*!
    \file    main.c
    \brief   serial SPI interface and regular conversion

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

#define FLT_OVER_SAMPLE_32       32
#define BUFFER_SIZE              25
#define HPDF_OUTPUT_BUFFER       2

/* limit the range of data */
#define DATA_LIMIT(N, L, H) (((N)<(L))?(L):(((N)>(H))?(H):(N)))
#define SET_SPI0_NSS_HIGH          gpio_bit_set(GPIOA,GPIO_PIN_4);
#define SET_SPI0_NSS_LOW           gpio_bit_reset(GPIOA,GPIO_PIN_4);

int32_t tm_high_val = 32769;
int32_t tm_low_val = -32769;
__attribute__((aligned(32))) volatile uint8_t spi0_send_array[BUFFER_SIZE] = {0};
__attribute__((aligned(32))) volatile int32_t final_data[HPDF_OUTPUT_BUFFER] = {0};
__attribute__((aligned(32))) volatile int32_t conversion_data[HPDF_OUTPUT_BUFFER] = {0};

void cache_enable(void);
void rcu_config(void);
void gpio_config(void);
void hpdf_config(void);
void spi_config(void);
void dma_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    uint16_t i = 0;
    uint8_t channel_error_flag = 0;
    spi0_send_array[12] = 0x01;

    for(i = (BUFFER_SIZE / 2) + 1; i < BUFFER_SIZE; i++) {
        spi0_send_array[i] = 0xFF;
    }
    /* clean D-Cache to flush spi0_send_array to memory before HPDF reads it */
    SCB_CleanDCache_by_Addr((uint32_t *)spi0_send_array, sizeof(spi0_send_array));
    __DSB();
    __ISB();

    /* enable the CPU cache */
    cache_enable();

    /* enable the peripheral clock */
    rcu_config();
    /* GPIO configure */
    gpio_config();
    /* configure DMA */
    dma_config();
    /* configure SPI0 */
    spi_config();

    /* configure USART */
    gd_eval_com_init(EVAL_COM);
    printf(" The HPDF test:\r\n ");
    /* configure HPDF */
    hpdf_config();

    /* enable SPI0 */
    spi_enable(SPI0);
    /* enable SPI0 DMA */
    spi_dma_enable(SPI0, SPI_DMA_TRANSMIT);

    /* enable DMA1 channel1 and channel3 */
    dma_channel_enable(DMA1, DMA_CH3);
    dma_channel_enable(DMA1, DMA_CH1);

    /* SPI master start transfer */
    spi_master_transfer_start(SPI0, SPI_TRANS_START);

    /* wait for the DMA transmit complete */
    while(RESET == dma_flag_get(DMA1, DMA_CH3, DMA_FLAG_FTF));
    while(RESET == dma_flag_get(DMA1, DMA_CH1, DMA_FLAG_FTF));

    /* verify that the HPDF output data exceed the high threshold */
    if(SET == hpdf_flag_get(FLT0, HPDF_FLAG_FLTY_HTF0)) {
        channel_error_flag = SET;
        printf(" The HPDF output exceed the high threshold !\r\n ");
    }
    /* verify that the HPDF output data exceed the low threshold */
    if(SET == hpdf_flag_get(FLT0, HPDF_FLAG_FLTY_LTF0)) {
        channel_error_flag = SET;
        printf(" The HPDF output exceed the low threshold !\r\n ");
    }
    /* verify that the HPDF malfunction event */
    if(SET == hpdf_flag_get(FLT0, HPDF_FLAG_FLT0_MMF0)) {
        channel_error_flag = SET;
        printf(" The channel0 input malfunction monitor occurred !\r\n ");
    }

    /* invalidate D-Cache to ensure reading latest data from memory */
    SCB_InvalidateDCache_by_Addr((uint32_t *)conversion_data, sizeof(conversion_data));
    __DSB();
    __ISB();
    
    if(RESET == channel_error_flag) {
        printf(" The range of maximum output resolution:\n ");
        /* get the PCM stereo data */
        for(i = 0; i < HPDF_OUTPUT_BUFFER; i++) {
            final_data[i] = DATA_LIMIT((conversion_data[i] >> 8), -32768, 32767);
            printf("%d ", final_data[i]);
        }
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
    /* enable i-cache d-cache */
    SCB_EnableICache();
    SCB_EnableDCache();
}

/*!
    \brief      configure the peripheral clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable HPDF clock */
    rcu_periph_clock_enable(RCU_HPDF);
    rcu_hpdf_clock_config(RCU_HPDFSRC_APB2);
    /* enable GPIOA clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* enable GPIOC clock */
    rcu_periph_clock_enable(RCU_GPIOC);
    /* enable SPI0 clock */
    rcu_per_clock_config(RCU_PERSRC_IRC64MDIV);
    rcu_periph_clock_enable(RCU_SPI0);
    rcu_spi_clock_config(IDX_SPI0, RCU_SPISRC_PER);
    /* enable DMA1 clock */
    rcu_periph_clock_enable(RCU_DMA1);
    rcu_periph_clock_enable(RCU_DMAMUX);
}

/*!
    \brief      configure the ports
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* connect PC0 to HPDF_CKIN0 */
    gpio_af_set(GPIOC, GPIO_AF_3, GPIO_PIN_0);
    /* connect PC3 to HPDF_DATAIN0 */
    gpio_af_set(GPIOC, GPIO_AF_3, GPIO_PIN_1);
    /* configure GPIO pins of HPDF */
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_0);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_0);
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_1);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_1);

    /* SPI0 GPIO config:SCK/PA5, MOSI/PA7 */
    gpio_af_set(GPIOA, GPIO_AF_5, GPIO_PIN_5 | GPIO_PIN_7);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5 | GPIO_PIN_7);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_5 | GPIO_PIN_7);
}

/*!
    \brief      configure the HPDF
    \param[in]  none
    \param[out] none
    \retval     none
*/
void hpdf_config(void)
{
    hpdf_channel_parameter_struct hpdf_channel_init_struct;
    hpdf_filter_parameter_struct hpdf_filter_init_struct;
    hpdf_rc_parameter_struct hpdf_rc_init_struct;

    /* reset HPDF */
    hpdf_deinit();

    /*  initialize the parameters */
    hpdf_channel_struct_para_init(&hpdf_channel_init_struct);
    hpdf_filter_struct_para_init(&hpdf_filter_init_struct);
    hpdf_rc_struct_para_init(&hpdf_rc_init_struct);

    /* initialize HPDF channel0 */
    hpdf_channel_init_struct.data_packing_mode      = DPM_STANDARD_MODE;
    hpdf_channel_init_struct.malfunction_monitor    = MM_ENABLE;
    hpdf_channel_init_struct.spi_ck_source          = EXTERNAL_CKIN;
    hpdf_channel_init_struct.channel_multiplexer    = SERIAL_INPUT;
    hpdf_channel_init_struct.serial_interface       = SPI_RISING_EDGE;
    hpdf_channel_init_struct.calibration_offset     = 0;
    hpdf_channel_init_struct.right_bit_shift        = 0;
    hpdf_channel_init_struct.mm_counter_threshold   = 110;
    hpdf_channel_init_struct.plsk_value             = 0;
    hpdf_channel_init(CHANNEL0, &hpdf_channel_init_struct);

    /* initialize HPDF fliter0 */
    hpdf_filter_init_struct.tm_fast_mode             = TMFM_DISABLE;
    hpdf_filter_init_struct.tm_channel               = TMCHEN_CHANNEL0;
    hpdf_filter_init_struct.tm_high_threshold        = tm_high_val;
    hpdf_filter_init_struct.tm_low_threshold         = tm_low_val;
    hpdf_filter_init_struct.extreme_monitor_channel  = EM_CHANNEL0;
    hpdf_filter_init_struct.sinc_filter              = FLT_SINC3;
    hpdf_filter_init_struct.sinc_oversample          = FLT_OVER_SAMPLE_32;
    hpdf_filter_init_struct.integrator_oversample    = INTEGRATOR_BYPASS;
    hpdf_filter_init(FLT0, &hpdf_filter_init_struct);

    /* initialize HPDF regular conversions */
    hpdf_rc_init_struct.fast_mode       = FAST_DISABLE;
    hpdf_rc_init_struct.rcs_channel     = RCS_CHANNEL0;
    hpdf_rc_init_struct.rcdmaen         = RCDMAEN_ENABLE;
    hpdf_rc_init_struct.continuous_mode = RCCM_ENABLE;
    hpdf_rc_init(FLT0, &hpdf_rc_init_struct);

    /* enable channel */
    hpdf_channel_enable(CHANNEL0);

    /* enable filter */
    hpdf_filter_enable(FLT0);

    /* enable the HPDF module globally */
    hpdf_enable();

    /* enable regular channel conversion by software */
    hpdf_rc_start_by_software(FLT0);
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
    spi_i2s_deinit(SPI0);

    /* SPI0 parameter config */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.data_size            = SPI_DATASIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_64;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI0, &spi_init_struct);

    /* enable SPI byte access */
    spi_byte_access_enable(SPI0);

    /* enable SPI NSS output */
    spi_nss_output_enable(SPI0);
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
    dma_single_data_para_struct_init(&dma_init_struct);

    /* SPI0 transmit dma config: DMA1,DMA_CH3 */
    dma_deinit(DMA1, DMA_CH3);
    dma_init_struct.request             = DMA_REQUEST_SPI0_TX;
    dma_init_struct.periph_addr         = (uint32_t)&SPI_TDATA(SPI0);
    dma_init_struct.memory0_addr        = (uint32_t)spi0_send_array;
    dma_init_struct.direction           = DMA_MEMORY_TO_PERIPH;
    dma_init_struct.periph_memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.priority            = DMA_PRIORITY_LOW;
    dma_init_struct.number              = BUFFER_SIZE;
    dma_init_struct.periph_inc          = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.memory_inc          = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.circular_mode       = DMA_CIRCULAR_MODE_DISABLE;
    dma_single_data_mode_init(DMA1, DMA_CH3, &dma_init_struct);

    /* HPDF dma config: DMA1,DMA_CH1 */
    dma_deinit(DMA1, DMA_CH1);
    /* configure DMA1_CH1 */
    dma_init_struct.request             =  DMA_REQUEST_HPDF_FLT0;
    dma_init_struct.periph_addr         = (int32_t)&HPDF_FLTYRDATA(FLT0);
    dma_init_struct.periph_inc          = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.memory0_addr        = (uint32_t)conversion_data;
    dma_init_struct.memory_inc          = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_32BIT;
    dma_init_struct.circular_mode       = DMA_CIRCULAR_MODE_DISABLE;
    dma_init_struct.direction           = DMA_PERIPH_TO_MEMORY;
    dma_init_struct.number              = HPDF_OUTPUT_BUFFER;
    dma_init_struct.priority            = DMA_PRIORITY_ULTRA_HIGH;
    dma_single_data_mode_init(DMA1, DMA_CH1, &dma_init_struct);
}
