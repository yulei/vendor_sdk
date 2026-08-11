/*!
    \file    main.c
    \brief   CAU TDESECB mode example

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

__attribute__((aligned(32)))
uint8_t encrypt_result[DATA_SIZE];
__attribute__((aligned(32)))
uint8_t decrypt_result[DATA_SIZE];
__IO uint8_t check_flag = 0;

/* encrypt/decrypt data using TDES */
static void tdes_dma(uint32_t mode, uint32_t *source, uint32_t *dest);
/* printf data by a block of 8 bytes */
static void data_display(uint32_t datalength, uint8_t *data);
/* check data in bytes */
void data_check(uint8_t *src, uint8_t *dst, uint32_t len);

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

    printf(" plain text :\r\n");
    data_display(DATA_SIZE, plaintext);

    /* encrypt */
    tdes_dma(CAU_ENCRYPT, (uint32_t *)&plaintext[0], (uint32_t *)&encrypt_result[0]);
    printf(" \r\nTDES encrypted data:\r\n\r\n");
    data_display(DATA_SIZE, encrypt_result);
    data_check((uint8_t *)&encrypt_result[0], (uint8_t *)&exp_tdes_ecb[0], DATA_SIZE);

    /* decrypt */
    tdes_dma(CAU_DECRYPT, (uint32_t *)&encrypt_result[0], (uint32_t *)&decrypt_result[0]);
    printf(" \r\nTDES decrypted data:\r\n\r\n");
    data_display(DATA_SIZE, decrypt_result);
    data_check((uint8_t *)&decrypt_result[0], (uint8_t *)&plaintext[0], DATA_SIZE);

    /* check result */
    if(0U == check_flag){
        gd_eval_led_on(LED1);
    }

    while(1) {
    }
}

/*!
    \brief      encrypt/decrypt data using TDES
    \param[in]  mode: algorithm direction
      \arg        CAU_ENCRYPT: encrypt
      \arg        CAU_DECRYPT: decrypt
    \param[in]  source: pointer to the source address
    \param[in]  dest: pointer to the dest address
    \param[out] none
    \retval     none
*/
static void tdes_dma(uint32_t mode, uint32_t *source, uint32_t *dest)
{
    cau_key_parameter_struct key_initpara;
    dma_single_data_parameter_struct dma_initpara;
    uint32_t *key_temp = (uint32_t *)&tdes_key[0];

    /* enable CAU clock */
    rcu_periph_clock_enable(RCU_CAU);
    /* enable DMA and DMAMUX clock */
    rcu_periph_clock_enable(RCU_DMA0);
    rcu_periph_clock_enable(RCU_DMAMUX);

    cau_fifo_flush();

    /* key structure initialization */
    cau_deinit();
    cau_key_struct_para_init(&key_initpara);
    key_initpara.key_1_high = __REV(*key_temp);
    key_initpara.key_1_low  = __REV(*(++key_temp));
    key_initpara.key_2_high = __REV(*(++key_temp));
    key_initpara.key_2_low  = __REV(*(++key_temp));
    key_initpara.key_3_high = __REV(*(++key_temp));
    key_initpara.key_3_low  = __REV(*(++key_temp));
    /* key initialization */
    cau_key_init(&key_initpara);

    /* initialize the CAU peripheral */
    cau_init(mode, CAU_MODE_TDES_ECB, CAU_SWAPPING_8BIT);
    /* flush the IN and OUT FIFOs */
    cau_fifo_flush();
    cau_enable();

    /* enable the CAU DMA interface */
    cau_dma_enable(CAU_DMA_INFIFO | CAU_DMA_OUTFIFO);

    /* DMA configuration */
    dma_deinit(DMA0, DMA_CH5);
    dma_deinit(DMA0, DMA_CH6);
    dma_single_data_para_struct_init(&dma_initpara);

    /* DMA channel5 for CAU_IN configuration */
    dma_initpara.direction           = DMA_MEMORY_TO_PERIPH;
    dma_initpara.memory0_addr        = (uint32_t)source;
    dma_initpara.memory_inc          = DMA_MEMORY_INCREASE_ENABLE;
    dma_initpara.periph_memory_width = DMA_PERIPH_WIDTH_32BIT;
    dma_initpara.number              = DATA_SIZE / 4U;
    dma_initpara.periph_addr         = (uint32_t)(&CAU_DI);
    dma_initpara.periph_inc          = DMA_PERIPH_INCREASE_DISABLE;
    dma_initpara.priority            = DMA_PRIORITY_HIGH;
    dma_initpara.request             = DMA_REQUEST_CAU_IN;
    dma_initpara.circular_mode       = DMA_CIRCULAR_MODE_DISABLE;
    dma_single_data_mode_init(DMA0, DMA_CH5, &dma_initpara);

    /* disable the DMAMUX_MUXCH5 synchronization mode */
    dmamux_synchronization_disable(DMAMUX_MUXCH5);

    /* DMA channel6 for CAU_OUT configuration */
    dma_initpara.direction           = DMA_PERIPH_TO_MEMORY;
    dma_initpara.memory0_addr        = (uint32_t)dest;
    dma_initpara.memory_inc          = DMA_MEMORY_INCREASE_ENABLE;
    dma_initpara.periph_memory_width = DMA_PERIPH_WIDTH_32BIT;
    dma_initpara.number              = DATA_SIZE / 4U;
    dma_initpara.periph_addr         = (uint32_t)(&CAU_DO);
    dma_initpara.periph_inc          = DMA_PERIPH_INCREASE_DISABLE;
    dma_initpara.priority            = DMA_PRIORITY_HIGH;
    dma_initpara.request             = DMA_REQUEST_CAU_OUT;
    dma_single_data_mode_init(DMA0, DMA_CH6, &dma_initpara);

    /* disable the DMAMUX_MUXCH6 synchronization mode */
    dmamux_synchronization_disable(DMAMUX_MUXCH6);

    /* enable DMA transfer */
    dma_channel_enable(DMA0, DMA_CH5);
    dma_channel_enable(DMA0, DMA_CH6);

    /* wait until the last transfer from OUT FIFO */
    while(!dma_flag_get(DMA0, DMA_CH6, DMA_FLAG_FTF)) {
    }

    dma_flag_clear(DMA0, DMA_CH5, DMA_FLAG_FTF);
    dma_flag_clear(DMA0, DMA_CH6, DMA_FLAG_FTF);

    cau_disable();
    cau_dma_disable(CAU_DMA_INFIFO | CAU_DMA_OUTFIFO);
    dma_channel_disable(DMA0, DMA_CH5);
    dma_channel_disable(DMA0, DMA_CH6);
}

/*!
    \brief      printf data by a block of 8 bytes
    \param[in]  datalength: length of the data to display
    \param[in]  data: pointer to the data to display
    \param[out] none
    \retval     none
*/
static void data_display(uint32_t datalength, uint8_t *data)
{
    uint32_t i = 0, count = 0;

    for(i = 0; i < datalength; i++) {
        printf("0x%02X ", data[i]);
        count++;

        if(8 == count) {
            count = 0;
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
void data_check(uint8_t *src, uint8_t *dst, uint32_t len)
{
    if(memcmp((const void*)src, (const void*)dst, len) != 0){
        check_flag++;
    }
}
