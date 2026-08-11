/*!
    \file    audio_sai_codec.c
    \brief   the low layer driver for audio codec

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

#include "audio_sai_codec.h"
#include "audio_core.h"

dma_single_data_parameter_struct dma_initstructure;

/*!
    \brief      initializes the audio codec audio interface (i2s)
    \note       this function assumes that the i2s input clock (through pll_r in 
                devices reva/z and through dedicated plli2s_r in devices revb/y)
                is already configured and ready to be used
    \param[in]  audio_freq: audio frequency to be configured for the i2s peripheral
    \param[out] none
    \retval     none
*/
void codec_audio_interface_init(uint32_t audio_freq)
{
    sai_parameter_struct sai_structure;
    sai_frame_parameter_struct sai_frame_structure;
    sai_slot_parameter_struct sai_slot_structure;

    /* enable SAI clock */
    rcu_periph_clock_enable(AD_SAI1_CLK);

    sai_struct_para_init(&sai_structure);
    sai_frame_struct_para_init(&sai_frame_structure);
    sai_slot_struct_para_init(&sai_slot_structure);

    /* AD_SAI1 peripheral configuration */
    sai_deinit(AD_SAI1);

    /* initialize SAI1_B1 frame */
    sai_frame_structure.frame_width            = 32;
    sai_frame_structure.frame_sync_width       = 16;
    sai_frame_structure.frame_sync_function    = SAI_FS_FUNC_START_CHANNEL;
    sai_frame_structure.frame_sync_polarity    = SAI_FS_POLARITY_LOW;
    sai_frame_structure.frame_sync_offset      = SAI_FS_OFFSET_BEGINNING;
    sai_frame_init(SAI1, SAI_BLOCK1, &sai_frame_structure);

    /* initialize SAI1_B1 slot */
    sai_slot_structure.slot_number             = 2;
    sai_slot_structure.slot_width              = SAI_SLOT_WIDTH_DATA;
    sai_slot_structure.data_offset             = 0;
    sai_slot_structure.slot_active             = SAI_SLOT_ACTIVE_ALL;
    sai_slot_init(SAI1, SAI_BLOCK1, &sai_slot_structure);

    /* initialize SAI1_B1  */
    sai_structure.operating_mode               = SAI_MASTER_TRANSMITTER;
    sai_structure.protocol                     = SAI_PROTOCOL_POLYMORPHIC;
    sai_structure.data_width                   = SAI_DATAWIDTH_16BIT;
    sai_structure.shift_dir                    = SAI_SHIFT_MSB;
    sai_structure.sample_edge                  = SAI_SAMPEDGE_RISING;
    sai_structure.sync_mode                    = SAI_SYNCMODE_ASYNC;
    sai_structure.output_drive                 = SAI_OUTPUT_WITH_SAIEN;
    sai_structure.clk_div_bypass               = SAI_CLKDIV_BYPASS_OFF;
    sai_structure.mclk_div                     = SAI_MCLKDIV_2;
    sai_structure.mclk_oversampling            = SAI_MCLK_OVERSAMP_256;
    sai_structure.mclk_enable                  = SAI_MCLK_ENABLE;
    sai_structure.fifo_threshold               = SAI_FIFOTH_FULL;
    sai_init(SAI1, SAI_BLOCK1, &sai_structure);
    
    sai_dma_enable(SAI1, SAI_BLOCK1);
    
    /* sai enable*/
    sai_enable(SAI1, SAI_BLOCK1);
}

/*!
    \brief      deinitialize the audio codec audio interface
    \param[in]  none
    \param[out] none
    \retval     none
*/
void codec_audio_interface_deinit(void)
{
    /* disable the codec_sai peripheral */
    sai_disable(AD_SAI1, SAI_BLOCK1);

    /* deinitialize the codec_sai peripheral */
    sai_deinit(AD_SAI1);

    /* disable the codec_sai peripheral clock */
    rcu_periph_clock_disable(AD_SAI1_CLK);
}

/*!
    \brief      initializes IOs used by the audio codec
    \param[in]  none
    \param[out] none
    \retval     none
*/
void codec_gpio_init(void)
{
    /* enable GPIO clock */
    rcu_periph_clock_enable(AD_SAI1_FS_CLK);
    rcu_periph_clock_enable(AD_SAI1_SCK_CLK);
    /* SAI_CK = PLL2_VCO/PLL2P = 200/25 = 8 Mhz */
    rcu_pll_input_output_clock_range_config(IDX_PLL2, RCU_PLL2RNG_1M_2M, RCU_PLL2VCO_192M_836M);
    rcu_pll2_config(25, 200, 25, 25, 25);
    rcu_sai_clock_config(IDX_SAI1, RCU_SAISRC_PLL2P);
    rcu_pll_clock_output_enable(RCU_PLL2P);
    rcu_osci_on(RCU_PLL2_CK);
    if(ERROR == rcu_osci_stab_wait(RCU_PLL2_CK)) {
        while(1) {
        }
    }

    /* configure GPIO pins of SAI1: SAI1_MCLK1(PH3) SAI1_FS1(PG9) SAI1_SCK1(PH2) SAI1_SD1(PG10) */
    gpio_af_set(AD_SAI1_FS_GPIO, GPIO_AF_10, AD_SAI1_FS_PIN);
    gpio_af_set(AD_SAI1_SCK_GPIO, GPIO_AF_10, AD_SAI1_SCK_PIN);
    gpio_af_set(AD_SAI1_SD_GPIO, GPIO_AF_10, AD_SAI1_SD_PIN);
    gpio_af_set(AD_SAI1_MCLK_GPIO, GPIO_AF_10, AD_SAI1_MCLK_PIN);
    
    gpio_mode_set(AD_SAI1_FS_GPIO, GPIO_MODE_AF, GPIO_PUPD_NONE, AD_SAI1_FS_PIN);
    gpio_mode_set(AD_SAI1_SCK_GPIO, GPIO_MODE_AF, GPIO_PUPD_NONE, AD_SAI1_SCK_PIN);
    gpio_mode_set(AD_SAI1_SD_GPIO, GPIO_MODE_AF, GPIO_PUPD_NONE, AD_SAI1_SD_PIN);
    gpio_mode_set(AD_SAI1_MCLK_GPIO, GPIO_MODE_AF, GPIO_PUPD_NONE, AD_SAI1_MCLK_PIN);

    gpio_output_options_set(AD_SAI1_FS_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, AD_SAI1_FS_PIN);
    gpio_output_options_set(AD_SAI1_SCK_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, AD_SAI1_SCK_PIN);
    gpio_output_options_set(AD_SAI1_SD_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, AD_SAI1_SD_PIN);
    gpio_output_options_set(AD_SAI1_MCLK_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, AD_SAI1_MCLK_PIN);
}

/*!
    \brief      deinitialize IOs used by the audio codec interface
    \param[in]  none
    \param[out] none
    \retval     none
*/
void codec_gpio_deinit(void)
{
    /* deinitialize all the GPIOs used by the driver */
    gpio_mode_set(AD_SAI1_FS_GPIO, GPIO_MODE_INPUT, GPIO_PUPD_NONE, AD_SAI1_FS_PIN);
    gpio_mode_set(AD_SAI1_SCK_GPIO, GPIO_MODE_INPUT, GPIO_PUPD_NONE, AD_SAI1_SCK_PIN);
    gpio_mode_set(AD_SAI1_SD_GPIO, GPIO_MODE_INPUT, GPIO_PUPD_NONE, AD_SAI1_SD_PIN);

#ifdef SAI1_MCLK_ENABLED
    /* AD_SAI1 pins deinitialization: MCLK pin */
    gpio_mode_set(AD_SAI1_MCLK_GPIO, GPIO_MODE_INPUT, GPIO_PUPD_NONE, AD_SAI1_MCLK_PIN);
#endif /* SAI1_MCLK_ENABLED */
}

/*!
    \brief      initializes dma to prepare for audio data transfer
                from Media to the SAI1 peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void codec_dma_init(void)
{
    /* enable DMA1 & DMAMUX clock */
    rcu_periph_clock_enable(AD_DMA_CLOCK);
    rcu_periph_clock_enable(RCU_DMAMUX);

    /* configure the DMA Stream */
    dma_deinit(AD_DMA, AD_DMA_CHANNEL);
    dma_single_data_para_struct_init(&dma_initstructure);

    /* set the parameters to be configured */
    dma_initstructure.request = DMA_REQUEST_SAI1_B1;
    dma_initstructure.periph_addr = AD_SAI1_ADDRESS;
    dma_initstructure.memory0_addr = (uint32_t)0;
    dma_initstructure.direction = DMA_MEMORY_TO_PERIPH;
    dma_initstructure.number = 0xFFFE;
    dma_initstructure.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_initstructure.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_initstructure.periph_memory_width = AD_DMA_PERIPH_DATA_SIZE;
    dma_initstructure.circular_mode = DMA_CIRCULAR_MODE_DISABLE;
    dma_initstructure.priority = DMA_PRIORITY_HIGH;
    dma_single_data_mode_init(AD_DMA, AD_DMA_CHANNEL, &dma_initstructure);

    /* clear the DMA flags */
    dma_flag_clear(AD_DMA, AD_DMA_CHANNEL, DMA_FLAG_FEE);
    dma_flag_clear(AD_DMA, AD_DMA_CHANNEL, DMA_FLAG_SDE);
    dma_flag_clear(AD_DMA, AD_DMA_CHANNEL, DMA_FLAG_TAE);
    dma_flag_clear(AD_DMA, AD_DMA_CHANNEL, DMA_FLAG_HTF);
    dma_flag_clear(AD_DMA, AD_DMA_CHANNEL, DMA_FLAG_FTF);

    /* SAI1 DMA IRQ channel configuration */
    nvic_irq_enable(AD_DMA_IRQ, AD_IRQ_PREPRIO, AD_IRQ_SUBRIO);
    
    /* enable the selected DMA interrupts */
    dma_interrupt_enable(AD_DMA, AD_DMA_CHANNEL, DMA_CHXCTL_FTFIE);
}

/*!
    \brief      restore default state of the used media
    \param[in]  none
    \param[out] none
    \retval     none
*/
void audio_mal_deinit(void)  
{
    /* deinitialize the NVIC interrupt for the I2S DMA Stream */
    nvic_irq_disable(AD_DMA_IRQ);

    /* disable the DMA channel before the deinitialize */
    dma_channel_disable(AD_DMA, AD_DMA_CHANNEL);

    /* deinitialize the DMA channel */
    dma_deinit(AD_DMA, AD_DMA_CHANNEL);
}

/*!
    \brief      starts playing audio stream from the audio media
    \param[in]  addr: pointer to the audio stream buffer
    \param[in]  size: number of data in the audio stream buffer
    \param[out] none
    \retval     none
*/
void audio_play(uint32_t addr, uint32_t size)
{
    /* disable the SAI DMA Stream*/
    dma_channel_disable(AD_DMA, AD_DMA_CHANNEL);
    
    sai_disable(SAI1, SAI_BLOCK1);

    /* clear the DMA flags */
    dma_flag_clear(AD_DMA, AD_DMA_CHANNEL, DMA_FLAG_FEE);
    dma_flag_clear(AD_DMA, AD_DMA_CHANNEL, DMA_FLAG_SDE);
    dma_flag_clear(AD_DMA, AD_DMA_CHANNEL, DMA_FLAG_TAE);
    dma_flag_clear(AD_DMA, AD_DMA_CHANNEL, DMA_FLAG_HTF);
    dma_flag_clear(AD_DMA, AD_DMA_CHANNEL, DMA_FLAG_FTF);

    /* configure the buffer address and size */
    dma_initstructure.memory0_addr = (uint32_t)addr;
    dma_initstructure.number = (uint32_t)(size);

    /* configure the DMA Stream with the new parameters */
    dma_single_data_mode_init(AD_DMA, AD_DMA_CHANNEL, &dma_initstructure);
    
    if((SAI_B1CFG0(AD_SAI1) & SAI1_ENABLE_MASK) == 0) {
        sai_enable(SAI1, SAI_BLOCK1);
    }

    /* enable the SAI1 DMA Stream*/
    dma_channel_enable(AD_DMA, AD_DMA_CHANNEL);
}

/*!
    \brief      pauses or resumes the audio stream playing from the media
    \param[in]  cmd: AD_PAUSE (or 0) to pause, AD_RESUME (or any value different
                from 0) to resume
    \param[in]  addr: address from/at which the audio stream should resume/pause
    \param[in]  size: number of data to be configured for next resume
    \param[out] none
    \retval     none
*/
void audio_pause_resume(uint32_t cmd, uint32_t addr, uint32_t size)
{
    /* pause the audio file playing */
    if(cmd == AD_PAUSE) {
        /* stop the current DMA request by resetting the I2S peripherals */
        codec_audio_interface_deinit();

        /* re-configure the SAI1 interface for the next resume operation */
        codec_audio_interface_init(0);

        /* disable the DMA Stream */
        dma_channel_disable(AD_DMA, AD_DMA_CHANNEL);

        /* clear all the DMA flags for the next transfer */
        dma_flag_clear(AD_DMA, AD_DMA_CHANNEL, DMA_FLAG_FEE);
        dma_flag_clear(AD_DMA, AD_DMA_CHANNEL, DMA_FLAG_SDE);
        dma_flag_clear(AD_DMA, AD_DMA_CHANNEL, DMA_FLAG_TAE);
        dma_flag_clear(AD_DMA, AD_DMA_CHANNEL, DMA_FLAG_HTF);
        dma_flag_clear(AD_DMA, AD_DMA_CHANNEL, DMA_FLAG_FTF);
    } else {
        /* configure the buffer address and size */
        dma_initstructure.memory0_addr = (uint32_t)addr;
        dma_initstructure.number = (uint32_t)size;

        /* configure the DMA Stream with the new parameters */
        dma_single_data_mode_init(AD_DMA, AD_DMA_CHANNEL, &dma_initstructure);
        
        /* if the sai peripheral is still not enabled, enable it */
        if((SAI_B1CFG0(AD_SAI1) & SAI1_ENABLE_MASK) == 0) {
            sai_enable(SAI1, SAI_BLOCK1);
        }
        
        /* enable the SAI1 DMA Stream*/
        dma_channel_enable(AD_DMA, AD_DMA_CHANNEL);
    }
}

/*!
    \brief      stops audio stream playing on the used media
    \param[in]  none
    \param[out] none
    \retval     none
*/
void audio_stop(void)
{
    /* stop the transfer on the SAI side: Stop and disable the DMA stream */
    dma_channel_disable(AD_DMA, AD_DMA_CHANNEL);

    /* clear all the DMA flags for the next transfer */
    dma_flag_clear(AD_DMA, AD_DMA_CHANNEL, DMA_FLAG_FEE);
    dma_flag_clear(AD_DMA, AD_DMA_CHANNEL, DMA_FLAG_SDE);
    dma_flag_clear(AD_DMA, AD_DMA_CHANNEL, DMA_FLAG_TAE);
    dma_flag_clear(AD_DMA, AD_DMA_CHANNEL, DMA_FLAG_HTF);
    dma_flag_clear(AD_DMA, AD_DMA_CHANNEL, DMA_FLAG_FTF);

    /* stop the current DMA request by resetting the I2S cell */
    codec_audio_interface_deinit();

    /* re-configure the SAI1 interface for the next play operation */
    codec_audio_interface_init(0);
}

/*!
    \brief      this function handles main media layer interrupt
    \param[in]  none
    \param[out] none
    \retval     0 if correct communication, else wrong communication
*/
void AD_DMA_IRQHandler(void)
{
    uint16_t remain_size = 0;

    /* transfer complete interrupt */
    if(dma_interrupt_flag_get(AD_DMA, AD_DMA_CHANNEL, AD_DMA_INT_FLAG_TC) != RESET) {
        /* increment to the next sub-buffer */
        audio_handler.isoc_out_rdptr += audio_handler.dam_tx_len;

        if(audio_handler.isoc_out_rdptr >= (audio_handler.isoc_out_buff + TOTAL_OUT_BUF_SIZE)) {
            /* roll back to the start of buffer */
            audio_handler.isoc_out_rdptr = audio_handler.isoc_out_buff;
        }

        if(audio_handler.isoc_out_wrptr >= audio_handler.isoc_out_rdptr) {
            remain_size = audio_handler.isoc_out_wrptr - audio_handler.isoc_out_rdptr;
        } else {
            remain_size = TOTAL_OUT_BUF_SIZE + audio_handler.isoc_out_buff - audio_handler.isoc_out_rdptr;
        }

        /* check if the end of file has been reached */
        if(remain_size > 0) {
            dma_channel_disable(AD_DMA, AD_DMA_CHANNEL);

            /* clear the interrupt flag */
            dma_interrupt_flag_clear(AD_DMA, AD_DMA_CHANNEL, AD_DMA_INT_FLAG_TC);

            /* clear the DMA flag */
            dma_flag_clear(AD_DMA, AD_DMA_CHANNEL, AD_DMA_FLAG_TE);

            DMA_CHM0ADDR(AD_DMA, AD_DMA_CHANNEL) = (uint32_t)audio_handler.isoc_out_rdptr;

            DMA_CHCNT(AD_DMA, AD_DMA_CHANNEL) = (uint32_t)(remain_size / 2);

            /* update the current DMA tx data length */
            audio_handler.dam_tx_len = (remain_size / 2) * 2;

            dma_channel_enable(AD_DMA, AD_DMA_CHANNEL);
        } else {
            /* disable the SAI DMA Stream*/
            dma_channel_disable(AD_DMA, AD_DMA_CHANNEL);

            /* clear the interrupt flag */
            dma_interrupt_flag_clear(AD_DMA, AD_DMA_CHANNEL, AD_DMA_INT_FLAG_TC);

            /* clear flag */
            audio_handler.dam_tx_len = 0;
            audio_handler.play_flag = 0U;
        }
    }
}
