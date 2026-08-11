/*!
    \file    audio_sai_codec.h
    \brief   header file of the low layer driver for audio codec

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

#ifndef AUDIO_SAI_CODEC_H
#define AUDIO_SAI_CODEC_H

#include "usb_conf.h"

/* select the interrupt preemption priority and sub-priority for DMA interrupt */
#define AD_IRQ_PREPRIO              0
#define AD_IRQ_SUBRIO               0

/* uncomment the defines below to select if the master clock mode should be enabled or not */
#define SAI1_MCLK_ENABLED
//#define SAI1_MCLK_DISABLED

/* hardware configuration defines parameters */
/* SAI1 peripheral configuration defines (data and control interface of the audio codec) */
#define AD_SAI1                      SAI1
#define AD_SAI1_CLK                  RCU_SAI1
#define AD_SAI1_ADDRESS              ((uint32_t)(&SAI_DATA(SAI1, SAI_BLOCK1)))
#define AD_SAI1_IRQ                  SAI1_IRQn

#define AD_SAI1_FS_PIN               GPIO_PIN_9
#define AD_SAI1_SCK_PIN              GPIO_PIN_2
#define AD_SAI1_MCLK_PIN             GPIO_PIN_3
#define AD_SAI1_SD_PIN               GPIO_PIN_10

#define AD_SAI1_FS_GPIO              GPIOG
#define AD_SAI1_SCK_GPIO             GPIOH
#define AD_SAI1_MCLK_GPIO            GPIOH
#define AD_SAI1_SD_GPIO              GPIOG

#define AD_SAI1_FS_CLK               RCU_GPIOG
#define AD_SAI1_SCK_CLK              RCU_GPIOH
#define AD_SAI1_MCLK_CLK             RCU_GPIOH
#define AD_SAI1_SD_CLK               RCU_GPIOG

/* SAI1 DMA stream definitions */
#define AD_DMA                      DMA1
#define AD_DMA_CLOCK                RCU_DMA1
#define AD_DMA_CHANNEL              DMA_CH3
#define AD_DMA_IRQ                  DMA1_Channel3_IRQn
#define AD_DMA_INT_FLAG_TC          DMA_INT_FLAG_FTF
#define AD_DMA_FLAG_TE              DMA_FLAG_TAE
#define AD_DMA_PERIPH_DATA_SIZE     DMA_PERIPH_WIDTH_16BIT
#define AD_DMA_MEM_DATA_SIZE        DMA_MEMORY_WIDTH_16BIT
#define AD_DMA_CHANNEL_CNT_MASK     DMA_CHXCNT_CNT

#define AD_DMA_IRQHandler           DMA1_Channel3_IRQHandler

/* mask for the bit en of the sai cfgr register */
#define SAI1_ENABLE_MASK             (0x10000)

/* audio state */
typedef enum _audio_status {
    AD_OK = 0,
    AD_FAIL,
} audio_status;

typedef enum _audio_ctl {
    AD_PAUSE = 0,
    AD_RESUME,
} audio_ctl;

/* audio machine states */
typedef enum _audio_state {
    AD_STATE_INACTIVE = 0,
    AD_STATE_ACTIVE,
    AD_STATE_PLAYING,
    AD_STATE_PAUSED,
    AD_STATE_STOPPED,
    AD_STATE_ERROR,
} audio_state_enum;

/* audio commands enumeration */
typedef enum
{
    AD_CMD_PLAY = 1U,
    AD_CMD_PAUSE,
    AD_CMD_STOP,
}audio_cmd_enum;

/* function declarations */
/* initializes the audio codec audio interface (sai) */
void codec_audio_interface_init(uint32_t audio_freq);
/* deinitialize the audio codec audio interface */
void codec_audio_interface_deinit(void);
/* initializes IOs used by the audio codec */
void codec_gpio_init(void);
/* deinitialize IOs used by the audio codec interface */
void codec_gpio_deinit(void);
/* initializes dma to prepare for audio data transfer */
void codec_dma_init(void);
/* starts playing audio stream from the audio media */
void audio_play(uint32_t addr, uint32_t size);
/* pauses or resumes the audio stream playing from the media */
void audio_pause_resume(uint32_t cmd, uint32_t addr, uint32_t size);
/* stops audio stream playing on the used media */
void audio_stop(void);

#endif /* AUDIO_SAI_CODEC_H */
