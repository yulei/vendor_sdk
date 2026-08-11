/*!
    \file    main.c
    \brief   HASH SHA1 MD5 dma example

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

#include "gd32h759i_eval.h"
#include "main.h"
#include <string.h>

/* hash SHA1/MD5 digest computation */
static void hash_sha1_md5_dma(uint32_t algo);
/* printf data in words */
static void data_display(uint32_t datalength);
/* check data in bytes */
static void data_check(uint8_t *src, uint8_t *dst, uint32_t len);

uint32_t i = 0, len = 0;
hau_digest_parameter_struct digest_para;
__IO uint8_t check_falg = 0;

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
    /* enable the CPU cache */
    cache_enable();

    gd_eval_led_init(LED1);
    gd_eval_com_init(EVAL_COM);
    /* enable HAU clock */
    rcu_periph_clock_enable(RCU_HAU);
    /* Enable DMA0 and DMAMUX clock */
    rcu_periph_clock_enable(RCU_DMA0);
    rcu_periph_clock_enable(RCU_DMAMUX);

    len = sizeof(message_input);  
    printf("\r\n message input:\r\n\r\n");
    for(i = 0; i < len ; i++){
        printf("%c", message_input[i]);
    }

    /* SHA-1 digest computation */
    hash_sha1_md5_dma(HAU_ALGO_SHA1);
    printf(" \r\n\r\nSHA1 message digest (160 bits):\r\n\r\n");
    data_display(20U);
    data_check((uint8_t *)&digest_para.out[0], (uint8_t *)&expected_data_sha1[0], 20U);

    /* MD5 digest computation */
    hash_sha1_md5_dma(HAU_ALGO_MD5);
    printf(" \r\n\r\nMD5 message digest (128 bits):\r\n\r\n");
    data_display(16U);
    data_check((uint8_t *)&digest_para.out[0], (uint8_t *)&expected_data_md5[0], 16U);

    if(0U == check_falg){
        gd_eval_led_on(LED1);
    }

    while(1){
    }
}

/*!
    \brief      HASH SHA1/MD5 digest computation
    \param[in]  algo: algorithm direction
      \arg        HAU_ALGO_SHA1
      \arg        HAU_ALGO_MD5
    \param[out] none
    \retval     none
*/
static void hash_sha1_md5_dma(uint32_t algo)
{
    dma_multi_data_parameter_struct dma_init_parameter;
    hau_init_parameter_struct hau_initpara;
    __IO uint16_t num_last_valid = 0;
    __IO uint32_t meg_burst = 0;

    /* HAU configuration */
    hau_deinit();
    hau_init_struct_para_init(&hau_initpara);
    hau_initpara.algo = algo;
    hau_initpara.mode = HAU_MODE_HASH;
    hau_initpara.datatype = HAU_SWAPPING_8BIT;
    hau_init(&hau_initpara);

    if(len % 4){
        meg_burst++;
    }
    meg_burst += (len / 4);
    num_last_valid = 8 * (len % 4);
    /* configure the number of valid bits in last word of the message */
    hau_last_word_validbits_num_config(num_last_valid);

    /* DMA peripheral configure */
    dma_deinit(DMA0, DMA_CH7);
    dma_multi_data_para_struct_init(&dma_init_parameter);

    dma_init_parameter.request            = DMA_REQUEST_HAU_IN;
    dma_init_parameter.periph_addr        = (uint32_t)(&HAU_DI);
    dma_init_parameter.periph_width       = DMA_PERIPH_WIDTH_32BIT;
    dma_init_parameter.periph_inc         = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_parameter.memory0_addr       = (uint32_t)message_input;
    dma_init_parameter.memory_width       = DMA_MEMORY_WIDTH_32BIT;
    dma_init_parameter.memory_inc         = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_parameter.memory_burst_width = DMA_MEMORY_BURST_SINGLE;
    dma_init_parameter.periph_burst_width = DMA_MEMORY_BURST_SINGLE;
    dma_init_parameter.critical_value     = DMA_FIFO_4_WORD;
    dma_init_parameter.circular_mode      = DMA_CIRCULAR_MODE_DISABLE;
    dma_init_parameter.direction          = DMA_MEMORY_TO_PERIPH;
    dma_init_parameter.number             = meg_burst;
    dma_init_parameter.priority           = DMA_PRIORITY_ULTRA_HIGH;
    dma_multi_data_mode_init(DMA0, DMA_CH7, &dma_init_parameter);

    /* disable the DMAMUX_MUXCH7 synchronization mode */
    dmamux_synchronization_disable(DMAMUX_MUXCH7);

    /* enable DMA channel */
    dma_channel_enable(DMA0, DMA_CH7);

    hau_dma_enable(); 
    /* wait until the last transfer from OUT FIFO */
    while(!dma_flag_get(DMA0, DMA_CH7, DMA_FLAG_FTF)){
    }

    /* wait until the busy flag is reset */
    while(hau_flag_get(HAU_FLAG_BUSY)){
    }

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
        check_falg++;
    }
}
