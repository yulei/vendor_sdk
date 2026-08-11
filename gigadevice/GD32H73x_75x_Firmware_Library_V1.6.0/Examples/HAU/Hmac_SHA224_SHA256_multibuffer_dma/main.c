/*!
    \file    main.c
    \brief   SHA224 SHA256 Multibuffer DMA example

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

#include <string.h>
#include "main.h"
#include "gd32h759i_eval.h"

void hmac_start_dma(uint32_t pinbuffer, uint32_t size);
void hmac_sha224_sha256_md5_dma(uint32_t algo);
void step_input(uint8_t *pinbuffer, uint32_t size);
static void data_display(uint32_t datalength);
static void data_check(uint8_t *src, uint8_t *dst, uint32_t len);

hau_digest_parameter_struct   digest_para;
uint32_t meg_len = 0, key_len = 0;
__IO uint8_t check_flag = 0;

/*!
    \brief      enable the CPU chache
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
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    uint32_t i;
    /* enable the CPU cache */
    cache_enable();

    /* initialize the com and LED */
    gd_eval_led_init(LED1);
    gd_eval_com_init(EVAL_COM);
    /* enable HAU clock */
    rcu_periph_clock_enable(RCU_HAU);
    /* Enable DMA0 and DMAMUX clock */
    rcu_periph_clock_enable(RCU_DMA0);
    rcu_periph_clock_enable(RCU_DMAMUX);

    meg_len = sizeof(message_input);
    key_len = sizeof(key);

    printf("\r\n\r\n message input:\r\n\r\n");
    for(i = 0; i < meg_len ; i++){
        printf("%c", message_input[i]);
    }
    
    printf("\r\n\r\n key input:\r\n\r\n");
    for(i = 0; i < key_len ; i++){
        printf("%c", key[i]);
    }

    /* SHA224 digest computation */
    hmac_sha224_sha256_md5_dma(HAU_ALGO_SHA224);
    printf(" \r\n\r\nSHA224 message digest (224 bits):\r\n\r\n");
    data_display(28U);
    data_check((uint8_t *)&digest_para.out[0], (uint8_t *)&expected_data_sha224[0], 28U);

    /* SHA256 digest computation */
    hmac_sha224_sha256_md5_dma(HAU_ALGO_SHA256);
    printf(" \r\n\r\nSHA256 message digest (256 bits):\r\n\r\n");
    data_display(32U);
    data_check((uint8_t *)&digest_para.out[0], (uint8_t *)&expected_data_sha256[0], 32U);

    if(0U == check_flag){
        gd_eval_led_on(LED1);
    }

    while(1) {
    }
}

/*!
    \brief      hmac SHA224/SHA256 digest computation
    \param[in]  algo: algorithm direction
      \arg        HAU_ALGO_SHA224
      \arg        HAU_ALGO_SHA2256
    \param[out] none
    \retval     none
*/
void hmac_sha224_sha256_md5_dma(uint32_t algo)
{
    hau_init_parameter_struct hau_initpara;

    __IO uint16_t num_last_valid = 0;
    __IO uint16_t num_key_valid = 0;

    /* number of valid bits in last word of the message */
    num_last_valid = 8U * (uint16_t)(meg_len  % 4U);
    /* number of valid bits in last word of the key */
    num_key_valid = 8U * (uint16_t)(key_len % 4U);

    /* HAU configuration */
    hau_deinit();
    hau_init_struct_para_init(&hau_initpara);
    hau_initpara.algo = algo;
    hau_initpara.mode = HAU_MODE_HMAC;
    hau_initpara.datatype = HAU_SWAPPING_8BIT;
    if(key_len > 64U) {
        hau_initpara.keytype = HAU_KEY_LONGGER_64;
    } else {
        hau_initpara.keytype = HAU_KEY_SHORTER_64;
    }
    hau_init(&hau_initpara);

    /* configure the number of valid bits in last word of the key */
    hau_last_word_validbits_num_config((uint32_t)num_key_valid);
    step_input((uint8_t *)key, key_len);

    /* configure the number of valid bits in last word of the message */
    hau_last_word_validbits_num_config((uint32_t)num_last_valid);
    step_input((uint8_t *)message_input, meg_len);

    /* configure the number of valid bits in last word of the key */
    hau_last_word_validbits_num_config((uint32_t)num_key_valid);
    step_input((uint8_t *)key, key_len);
    hau_digest_read(&digest_para);

    /* change formatting */
    digest_para.out[0] = __REV(digest_para.out[0]);
    digest_para.out[1] = __REV(digest_para.out[1]);
    digest_para.out[2] = __REV(digest_para.out[2]);
    digest_para.out[3] = __REV(digest_para.out[3]);
    digest_para.out[4] = __REV(digest_para.out[4]);
    digest_para.out[5] = __REV(digest_para.out[5]);
    digest_para.out[6] = __REV(digest_para.out[6]);
    digest_para.out[7] = __REV(digest_para.out[7]);
}

/*!
    \brief      DMA multiple transfer
    \param[in]  pinbuffer: parameter input
      \arg        key
      \arg        message_input
    \param[in]  size: parameter input size
    \param[out] none
    \retval     none
*/
void step_input(uint8_t *pinbuffer, uint32_t size)
{
    uint32_t inputaddr, i;
    uint32_t times = 0, last_size = 0;
    uint32_t dma_m_times = 0, dma_s_size = 0;
    inputaddr = (uint32_t) pinbuffer;

    times = size / 64;
    last_size = size % 64;
    if((0 != times) && (0 != last_size)) {
        dma_m_times = times;
        dma_s_size = last_size;
    } else if((times > 1) && (0 == last_size)) {
        dma_m_times = times - 1;
        dma_s_size = 64;
    } else {
        dma_m_times = 0;
        dma_s_size = 64;
    }

    /* set DMA multiple transfer */
    hau_multiple_single_dma_config(MULTIPLE_DMA_NO_DIGEST);
    for(i = 0; i < dma_m_times; i++) {
        hmac_start_dma(inputaddr + 64 * i, 64);
    }

    /* cancel DMA multiple transfer */
    hau_multiple_single_dma_config(SINGLE_DMA_AUTO_DIGEST);
    hmac_start_dma(inputaddr + 64 * dma_m_times, dma_s_size);
}

/*!
    \brief      DMA transfer
    \param[in]  pinbuffer: parameter input
      \arg        key
      \arg        message_input
    \param[in]  size: length of the data
    \param[out] none
    \retval     none
*/
void hmac_start_dma(uint32_t pinbuffer, uint32_t size)
{
    dma_multi_data_parameter_struct dma_init_parameter;
    uint8_t input_num;

    input_num = ((size % 4U) != 0U) ? ((size + 3U) / 4U) : (size / 4U);

    /* DMA configuration */
    dma_deinit(DMA0, DMA_CH7);
    dma_multi_data_para_struct_init(&dma_init_parameter);
    dma_init_parameter.request            = DMA_REQUEST_HAU_IN;
    dma_init_parameter.periph_addr        = (uint32_t)(&HAU_DI);
    dma_init_parameter.periph_width       = DMA_PERIPH_WIDTH_32BIT;
    dma_init_parameter.periph_inc         = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_parameter.memory0_addr       = (uint32_t)pinbuffer;
    dma_init_parameter.memory_width       = DMA_MEMORY_WIDTH_32BIT;
    dma_init_parameter.memory_inc         = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_parameter.memory_burst_width = DMA_MEMORY_BURST_SINGLE;
    dma_init_parameter.periph_burst_width = DMA_MEMORY_BURST_SINGLE;
    dma_init_parameter.critical_value     = DMA_FIFO_4_WORD;
    dma_init_parameter.circular_mode      = DMA_CIRCULAR_MODE_DISABLE;
    dma_init_parameter.direction          = DMA_MEMORY_TO_PERIPH;
    dma_init_parameter.number             = input_num;
    dma_init_parameter.priority           = DMA_PRIORITY_ULTRA_HIGH;
    dma_multi_data_mode_init(DMA0, DMA_CH7, &dma_init_parameter);

    /* disable the DMAMUX_MUXCH7 synchronization mode */
    dmamux_synchronization_disable(DMAMUX_MUXCH7);

    /* enable DMA channel */
    dma_channel_enable(DMA0, DMA_CH7);

    hau_dma_enable();

    /* wait until the last transfer from OUT FIFO */
    while(!dma_flag_get(DMA0, DMA_CH7, DMA_FLAG_FTF)) {
    }

    /* wait until the busy flag is reset */
    while(hau_flag_get(HAU_FLAG_BUSY)) {
    }
}

/*!
    \brief      printf data in words
    \param[in]  datalength: length of the data to display
    \param[out] none
    \retval     none
*/
static void data_display(uint32_t datalength)
{
    uint32_t i =0U;
    uint32_t count = 0U;
    uint8_t *temp = (uint8_t *)&digest_para.out;

    for(i = 0; i < datalength; i++){
        printf(" 0x%02X", (uint8_t)temp[i]);
        count++;

        if(4U == count){
            count = 0U;
            printf("\r\n");
        }
    }
}

/*!
    \brief      check data in bytes
    \param[in]  src: source
    \param[in]  dst: destination
    \param[in]  len: length of the data to check
    \param[out] none
    \retval     none
*/
static void data_check(uint8_t *src, uint8_t *dst, uint32_t len)
{
    if(memcmp((const void*)src, (const void*)dst, len) != 0){
        check_flag++;
    }
}
