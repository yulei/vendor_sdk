/*!
    \file    i2s_codec.c
    \brief   I2S codec driver

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

#include <stdio.h>
#include "i2s_codec.h"
#include "wave_data.h"

wave_file_struct wave_struct;
uint16_t i2saudiofreq = 0;
__IO uint8_t headertab_index = 0;
uint32_t datastartaddr = 0;
__IO uint32_t audiodataindex = 0;

/*!
    \brief      read uint data according to endianness
    \param[in]  nbrofbytes: number of read bytes
    \param[in]  bytesformat: littleendian or bigendian
    \param[out] none
    \retval     the uint data
*/
uint32_t read_unit(uint8_t nbrofbytes, endianness_enum bytesformat)
{
    uint32_t index = 0;
    uint32_t temp = 0;
    if(littleendian == bytesformat) {
        for(index = 0; index < nbrofbytes; index++) {
            temp |= AUDIOFILEADDRESS[headertab_index++] << (index * 8);
        }
    } else {
        for(index = nbrofbytes; index != 0; index--) {
            temp |= AUDIOFILEADDRESS[headertab_index++] << ((index - 1) * 8);
        }
    }
    return temp;
}

/*!
    \brief      wave audio file parsing function
    \param[in]  none
    \param[out] none
    \retval     errorcode_enum
*/
errorcode_enum codec_wave_parsing(void)
{
    uint32_t temp = 0;
    uint32_t extraformatbytes = 0;
    /* initialize the headertab index variable */
    headertab_index = 0;
    /* read chunkid, must be 'riff' */
    if(CHUNKID != read_unit(4, bigendian)) {
        return(UNVALID_RIFF_ID);
    }
    /* read the file length */
    wave_struct.riffchunksize = read_unit(4, littleendian);
    /* read the file format, must be 'wave' */
    if(FILEFORMAT != read_unit(4, bigendian)) {
        return(UNVALID_WAVE_FORMAT);
    }
    /* read the format chunk, must be 'fmt' */
    if(FORMATID != read_unit(4, bigendian)) {
        return(UNVALID_FORMATCHUNK_ID);
    }
    /* read the size of the 'fmt' data, must be 0x10 */
    if(FORMATCHUNKSIZE != read_unit(4, littleendian)) {
        extraformatbytes = 1;
    }
    /* read the audio format, must be 0x01 (pcm) */
    wave_struct.formattag = read_unit(2, littleendian);
    if(WAVE_FORMAT_PCM != wave_struct.formattag) {
        return(UNSUPPORETD_FORMATTAG);
    }
    /* read the number of channels: 0x02->stereo 0x01->mono */
    wave_struct.numchannels = read_unit(2, littleendian);
    /* read the sample rate */
    wave_struct.samplerate = read_unit(4, littleendian);
    /* update the i2s_audiofreq value according to the .wav file sample rate */
    if((wave_struct.samplerate < 8000) || (wave_struct.samplerate > 192000)) {
        return(UNSUPPORETD_SAMPLE_RATE);
    } else {
        i2saudiofreq = wave_struct.samplerate;
    }
    /* read the byte rate */
    wave_struct.byterate = read_unit(4, littleendian);
    /* read the block alignment */
    wave_struct.blockalign = read_unit(2, littleendian);
    /* read the number of bits per sample */
    wave_struct.bitspersample = read_unit(2, littleendian);
    if(BITS_PER_SAMPLE_16 != wave_struct.bitspersample) {
        return(UNSUPPORETD_BITS_PER_SAMPLE);
    }
    /* if there are extra format bytes, these bytes will be defined in "fact chunk" */
    if(1 == extraformatbytes) {
        /* read th extra format bytes, must be 0x00 */
        if(0x00 != read_unit(2, littleendian)) {
            return(UNSUPPORETD_EXTRAFORMATBYTES);
        }
        /* read the fact chunk, must be 'fact' */
        if(FACTID != read_unit(4, bigendian)) {
            return(UNVALID_FACTCHUNK_ID);
        }
        /* read fact chunk data size */
        temp = read_unit(4, littleendian);
        /* set the index to start reading just after the header end */
        headertab_index += temp;
    }
    /* read the data chunk, must be 'data' */
    if(DATAID != read_unit(4, bigendian)) {
        return(UNVALID_DATACHUNK_ID);
    }
    /* read the number of sample data */
    wave_struct.datasize = read_unit(4, littleendian);
    /* set the data pointer at the beginning of the effective audio data */
    datastartaddr += headertab_index;

    return(VALID_WAVE_FILE);
}

/*!
    \brief      configure the SAI peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void sai_config(void)
{
    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOH);
    rcu_periph_clock_enable(RCU_GPIOG);
    /* SAI_CK = PLL1_VCO/PLL1P = 200/25 = 8 Mhz */
    rcu_pll_input_output_clock_range_config(IDX_PLL1, RCU_PLL1RNG_1M_2M, RCU_PLL1VCO_192M_836M);
    rcu_pll1_config(25, 200, 25, 25, 25);
    rcu_sai_clock_config(IDX_SAI1, RCU_SAISRC_PLL1P);
    rcu_pll_clock_output_enable(RCU_PLL1P);
    rcu_osci_on(RCU_PLL1_CK);
    if(ERROR == rcu_osci_stab_wait(RCU_PLL1_CK)) {
        while(1) {
        }
    }

    /* enable SAI clock */
    rcu_periph_clock_enable(RCU_SAI1);
    /* configure GPIO pins of SAI1: SAI1_MCLK1(PH3) SAI1_FS1(PG9) SAI1_SCK1(PH2) SAI1_SD1(PG10) */
    gpio_af_set(GPIOG, GPIO_AF_10, GPIO_PIN_9 | GPIO_PIN_10);
    gpio_af_set(GPIOH, GPIO_AF_10, GPIO_PIN_2 | GPIO_PIN_3);
    gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE,
                  GPIO_PIN_9 | GPIO_PIN_10);
    gpio_mode_set(GPIOH, GPIO_MODE_AF, GPIO_PUPD_NONE,
                  GPIO_PIN_2 | GPIO_PIN_3);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ,
                            GPIO_PIN_9 | GPIO_PIN_10);
    gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ,
                            GPIO_PIN_2 | GPIO_PIN_3);

    sai_parameter_struct sai_structure;
    sai_frame_parameter_struct sai_frame_structure;
    sai_slot_parameter_struct sai_slot_structure;

    sai_struct_para_init(&sai_structure);
    sai_frame_struct_para_init(&sai_frame_structure);
    sai_slot_struct_para_init(&sai_slot_structure);

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
    sai_structure.mclk_div                     = SAI_MCLKDIV_4;
    sai_structure.mclk_oversampling            = SAI_MCLK_OVERSAMP_256;
    sai_structure.mclk_enable                  = SAI_MCLK_ENABLE;
    sai_structure.fifo_threshold               = SAI_FIFOTH_HALF;
    sai_init(SAI1, SAI_BLOCK1, &sai_structure);
    /* sai enable*/
    sai_enable(SAI1, SAI_BLOCK1);
}

/*!
    \brief      send audio data
    \param[in]  none
    \param[out] none
    \retval     none
*/
void sai_audio_data_send(void)
{
    /* send the data read from the memory */
    sai_data_transmit(SAI1, SAI_BLOCK1, read_half_word(audiodataindex + datastartaddr));
    /* increment the index */
    audiodataindex += (uint32_t)wave_struct.numchannels ;
}

/*!
    \brief      start audio play
    \param[in]  none
    \param[out] none
    \retval     errorcode_enum
*/
errorcode_enum sai_audio_play(void)
{
    errorcode_enum errorcode = UNVALID_RIFF_ID;
    /* read the audio file to extract the audio frequency */
    errorcode = codec_wave_parsing();
    if(VALID_WAVE_FILE == errorcode) {
        /* configure SAI */
        sai_config();
        /* NVIC configuration */
        nvic_irq_enable(SAI1_IRQn, 1, 1);
        /* enable the SAI interrupt */
        sai_interrupt_enable(SAI1, SAI_BLOCK1, SAI_INT_FFREQ);
    }
    return errorcode;
}

/*!
    \brief      read half word
    \param[in]  offset : audio data index
    \param[out] none
    \retval     audio data
*/
uint16_t read_half_word(uint32_t offset)
{
    static  uint32_t monovar = 0, tmpvar = 0;
    uint32_t test;
    tmpvar = (*(__IO uint16_t *)(AUDIOFILEADDRESS));
    if((AUDIOFILEADDRESS + offset) >= AUDIOFILEADDRESSEND) {
        audiodataindex = 0;
    }
    /* test if the left channel is to be sent */
    if(0 == monovar) {
        test = AUDIOFILEADDRESS + offset;
        tmpvar = (*(__IO uint16_t *)(test));
        /* increment the mono variable only if the file is in mono format */
        if(CHANNEL_MONO == wave_struct.numchannels)
            /* increment the monovar variable */
        {
            monovar++;
        }
        /* return the read value */
        return tmpvar;
        /* right channel to be sent in mono format */
    } else {
        /* reset the monovar variable */
        monovar = 0;
        /* return the previous read data in mono format */
        return tmpvar;
    }
}
