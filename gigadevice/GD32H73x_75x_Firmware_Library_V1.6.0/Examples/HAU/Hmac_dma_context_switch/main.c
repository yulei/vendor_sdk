/*!
    \file    main.c
    \brief   HMAC context switch using DMA example

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
#include "gd32h759i_eval.h"
#include "main.h"

#define TRANSFER_STEP_SIZE     152U    /* must be multiple of 4 bytes */
#define HAU_FIFO_DEPTH         16U

hau_context_parameter_struct  context_para;
hau_digest_parameter_struct   digest_para;
uint32_t g_hau_nwif = 0, g_total_size = 0;
__IO uint32_t g_dma_size = 0;
__IO uint8_t check_flag = 0;

/* HMAC using SHA224 or SHA256 */
void hmac_calculate_sha224_sha256(uint32_t algo, uint32_t keysize);
/* DMA multiple transfer */
void multi_input(uint8_t *pinbuffer, uint32_t size);
/* DMA transfer configuration */
void hmac_dma_config(uint32_t input, uint32_t size);

/* task 1 HMAC digest computation process */
void task_1_process(void);
/* task 2 HMAC digest computation insert during task 1 process */
void task_2_insert(void);
/* save context when a higher priority task comes */
void task_context_save(void);
/* task 2 HMAC digest computation process */
void task_2_hmac_sha256_dma(void);
/* restore context when a higher priority task finishes */
void task_context_restore(void);
/* printf data in words */
static void data_display(uint32_t datalength);
/* check data in bytes */
static void data_check(uint8_t *src, uint8_t *dst, uint32_t len);

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

    /* task 1 HMAC digest computation process */
    task_1_process();

    if(0U == check_flag){
        gd_eval_led_on(LED1);
    }

    while(1){
    }
}

/*!
    \brief      task 1 HMAC digest computation process
    \param[in]  none
    \param[out] none
    \retval     none
*/
void task_1_process(void)
{
    uint32_t insert_flag = 1;
    uint32_t t_size = 0, multi_single = 0, step_2_flag = 0;
    uint8_t *t_input = NULL; 
    __IO uint16_t num_last_valid = 0;
    __IO uint16_t num_key_valid = 0;
    uint32_t meg_len = 0, key_len = 0;
    uint32_t i;

    meg_len = sizeof(message_input_task_1);
    key_len = sizeof(key_task_1);

    /* number of valid bits in last word of the message */
    num_last_valid = 8U * (uint16_t)(meg_len  % 4U);
    /* number of valid bits in last word of the key */
    num_key_valid = 8U * (uint16_t)(key_len % 4U);

    hmac_calculate_sha224_sha256(HAU_ALGO_SHA224, sizeof(key_task_1));

    /* ==========step1: enter key for inner hash function========== */
    /* configure the number of valid bits in last word of the key */
    hau_last_word_validbits_num_config((uint32_t)num_key_valid);
    multi_input((uint8_t *)key_task_1, key_len);

    /* ==========step2: enter input message========== */
    multi_single = MULTIPLE_DMA_NO_DIGEST;
    /* multiple DMA */
    while(0 == step_2_flag){
        /* update the input data pointer */
        t_input = (uint8_t *)message_input_task_1 + g_total_size;
        /* multiple DMA transmission size every time */
        t_size = TRANSFER_STEP_SIZE;
        /* pre-calculate the transmitted size after the DMA transmission of this cycle */
        g_total_size += t_size;
        /* judge if this cycle is the end cycle of multiple DMA transmission */
        if(g_total_size >= sizeof(message_input_task_1)){
            step_2_flag = 1;
            /* calculate the exact rest size for transmission */
            t_size = sizeof(message_input_task_1) + TRANSFER_STEP_SIZE - g_total_size;
            /* the end cycle should set MDS=0 to automatically set CALEN=1 */
            multi_single = SINGLE_DMA_AUTO_DIGEST;
        }

        /* DMA transmission configuration */
        hau_multiple_single_dma_config(multi_single);
        /* configure the number of valid bits in last word of the message */
        hau_last_word_validbits_num_config((uint32_t)num_last_valid);
        hmac_dma_config((uint32_t)t_input, t_size);

        /* a high priority task 2 is coming */
        if(1 == insert_flag){
            /* switch context and handle task 2 */
            task_2_insert();
            insert_flag = 0;
            /* re-configure the flag to 0 to avoid the interruption happened in the end cycle */
            step_2_flag = 0;
        }else{
            /* wait until the last transfer from OUT FIFO */
            while(!dma_flag_get(DMA0, DMA_CH7, DMA_FLAG_FTF)){
            }
            /* wait until the busy flag is reset */
            while(hau_flag_get(HAU_FLAG_BUSY)){
            }
        }
    }

    /* ==========step3: enter key for inner hash function========== */
    /* configure the number of valid bits in last word of the key */
    hau_last_word_validbits_num_config((uint32_t)num_key_valid);
    multi_input((uint8_t *)key_task_1, key_len);

    /* read HMAC SHA224 digest */
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

    printf("\r\n\r\n SHA224 message input:\r\n\r\n");
    for(i = 0; i < meg_len ; i++){
        printf("%c", message_input_task_1[i]);
    }
    printf("\r\n\r\n key input:\r\n\r\n");
    for(i = 0; i < key_len ; i++){
        printf("%c", key_task_1[i]);
    }

    printf("\r\n\r\n SHA224 message digest (224 bits):\r\n\r\n");
    data_display(28U);
    data_check((uint8_t *)&digest_para.out[0], (uint8_t *)&expected_data_224[0], 28U);
}

/*!
    \brief      HMAC using SHA224 or SHA256
    \param[in]  algo: algorithm selection
      \arg        HAU_ALGO_SHA224
      \arg        HAU_ALGO_SHA256
    \param[in]  keysize: length of the key used for HMAC in bytes
    \param[out] none
    \retval     none
*/
void hmac_calculate_sha224_sha256(uint32_t algo, uint32_t keysize)
{
    hau_init_parameter_struct hau_initpara;

    /* HAU configuration */
    hau_deinit();
    hau_init_struct_para_init(&hau_initpara);
    hau_initpara.algo = algo;
    hau_initpara.mode = HAU_MODE_HMAC;
    hau_initpara.datatype = HAU_SWAPPING_8BIT;
    if(keysize > 64U){
        hau_initpara.keytype = HAU_KEY_LONGGER_64;
    }else{
        hau_initpara.keytype = HAU_KEY_SHORTER_64;
    }
    hau_init(&hau_initpara);
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
void multi_input(uint8_t *pinbuffer, uint32_t size)
{   
    uint32_t inputaddr, i;
    uint32_t times = 0, last_size = 0;
    uint32_t dma_m_times = 0, dma_s_size = 0;
    inputaddr = (uint32_t) pinbuffer;
    
    times = size / TRANSFER_STEP_SIZE;
    last_size = size % TRANSFER_STEP_SIZE;
    if((0 != times) && (0 != last_size)){
        dma_m_times = times;
        dma_s_size = last_size;
    }else if((times > 1) && (0 == last_size)){
        dma_m_times = times - 1;
        dma_s_size = TRANSFER_STEP_SIZE;
    }else{
        dma_m_times = 0;
        dma_s_size = TRANSFER_STEP_SIZE;
    }

    /* set DMA multiple transfer */
    hau_multiple_single_dma_config(MULTIPLE_DMA_NO_DIGEST);
    for(i = 0; i < dma_m_times; i++){
        hmac_dma_config(inputaddr + TRANSFER_STEP_SIZE * i, TRANSFER_STEP_SIZE);
        /* wait until the last transfer from OUT FIFO */
        while(!dma_flag_get(DMA0, DMA_CH7, DMA_FLAG_FTF)){
        }
        /* wait until the busy flag is reset */
        while(hau_flag_get(HAU_FLAG_BUSY)){
        }
    }

    /* cancel DMA multiple transfer */
    hau_multiple_single_dma_config(SINGLE_DMA_AUTO_DIGEST);
    hmac_dma_config(inputaddr + TRANSFER_STEP_SIZE * dma_m_times, dma_s_size);
    /* wait until the last transfer from OUT FIFO */
    while(!dma_flag_get(DMA0, DMA_CH7, DMA_FLAG_FTF)){
    }
    /* wait until the busy flag is reset */
    while(hau_flag_get(HAU_FLAG_BUSY)){
    }
}

/*!
    \brief      DMA transfer configuration
    \param[in]  input: pointer to data to transmit 
    \param[in]  size: length of the data in bytes
    \param[out] none
    \retval     none
*/
void hmac_dma_config(uint32_t input, uint32_t size)
{   
    dma_multi_data_parameter_struct dma_init_parameter;
    
    g_dma_size = 0;
    if(size % 4){
        g_dma_size++;
    }
    g_dma_size += (size / 4);

    g_hau_nwif = hau_infifo_words_num_get();
    
    /* DMA configuration */
    dma_deinit(DMA0, DMA_CH7);
    dma_multi_data_para_struct_init(&dma_init_parameter);
    dma_init_parameter.request = DMA_REQUEST_HAU_IN;
    dma_init_parameter.periph_addr = (uint32_t)(&HAU_DI);
    dma_init_parameter.periph_width = DMA_PERIPH_WIDTH_32BIT;
    dma_init_parameter.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_parameter.memory0_addr = (uint32_t)input;
    dma_init_parameter.memory_width = DMA_MEMORY_WIDTH_32BIT;
    dma_init_parameter.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_parameter.memory_burst_width = DMA_MEMORY_BURST_SINGLE;
    dma_init_parameter.periph_burst_width = DMA_MEMORY_BURST_SINGLE;
    dma_init_parameter.critical_value = DMA_FIFO_4_WORD;
    dma_init_parameter.circular_mode = DMA_CIRCULAR_MODE_DISABLE;
    dma_init_parameter.direction = DMA_MEMORY_TO_PERIPH;
    dma_init_parameter.number = g_dma_size;
    dma_init_parameter.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_multi_data_mode_init(DMA0, DMA_CH7, &dma_init_parameter);

    /* disable the DMAMUX_MUXCH7 synchronization mode */
    dmamux_synchronization_disable(DMAMUX_MUXCH7);

    /* enable DMA transfer */
    dma_channel_enable(DMA0, DMA_CH7);
    hau_dma_enable();
}

/*!
    \brief      task 2 HMAC digest computation insert during task 1 process
    \param[in]  none
    \param[out] none
    \retval     none
*/
void task_2_insert(void)
{
    task_context_save();
    task_2_hmac_sha256_dma();
    task_context_restore();
}

/*!
    \brief      save context when a higher priority task comes
    \param[in]  none
    \param[out] none
    \retval     none
*/
void task_context_save(void)
{
    uint32_t remaining_size = 0;
    uint32_t new_hau_nwif = 0;

    /* wait for BUSY=0 */
    while(SET == hau_flag_get(HAU_FLAG_BUSY));
    /* if CCF=1, no need to save context */
    if(SET == hau_flag_get(HAU_FLAG_CALCULATION_COMPLETE)){
        return;
    }

    /* disable dma transmission */
    dma_channel_disable(DMA0, DMA_CH7);
    dma_flag_clear(DMA0, DMA_CH7, DMA_FLAG_FTF);
    dma_flag_clear(DMA0, DMA_CH7, DMA_FLAG_HTF);
    dma_flag_clear(DMA0, DMA_CH7, DMA_FLAG_FEE);
    dma_flag_clear(DMA0, DMA_CH7, DMA_FLAG_SDE);
    dma_flag_clear(DMA0, DMA_CH7, DMA_FLAG_TAE);
    hau_dma_disable();

    /* wait for BUSY=0 */
    while(SET == hau_flag_get(HAU_FLAG_BUSY));
    /* if CCF=1, no need to save context */
    if(SET == hau_flag_get(HAU_FLAG_CALCULATION_COMPLETE)){
        return;
    }

    /* save interrupt position */
    remaining_size = dma_transfer_number_get(DMA0, DMA_CH7);
    new_hau_nwif = (hau_infifo_words_num_get()+1)% HAU_FIFO_DEPTH;
    /* synchronous HAU and DMA */
    if(new_hau_nwif != ((g_dma_size - remaining_size + g_hau_nwif) % HAU_FIFO_DEPTH)){
        /* error */
        if(0 != remaining_size){
            while(1);
        }
        remaining_size--;
    }
    g_total_size -= 4*remaining_size;
    /* the length already transferred is multiple of 4 bytes */
    if(0 != (g_total_size%4)){
        g_total_size = (g_total_size/4 + 1) * 4;
    }

    /* save HAU context */
    hau_context_struct_para_init(&context_para);
    hau_context_save(&context_para);
}

/*!
    \brief      task 2 HMAC digest computation process
    \param[in]  none
    \param[out] none
    \retval     none
*/
void task_2_hmac_sha256_dma(void)
{
    __IO uint16_t num_last_valid = 0;
    __IO uint16_t num_key_valid = 0;
    uint32_t meg_len = 0, key_len = 0;
    uint32_t i;
    
    meg_len = sizeof(message_input_task_2);
    key_len = sizeof(key_task_2);

    /* number of valid bits in last word of the message */
    num_last_valid = 8U * (uint16_t)(meg_len  % 4U);
    /* number of valid bits in last word of the key */
    num_key_valid = 8U * (uint16_t)(key_len % 4U);

    hmac_calculate_sha224_sha256(HAU_ALGO_SHA256, sizeof(key_task_2));

    /* configure the number of valid bits in last word of the key */
    hau_last_word_validbits_num_config((uint32_t)num_key_valid);
    multi_input((uint8_t *)key_task_2, key_len);
    
    /* configure the number of valid bits in last word of the message */
    hau_last_word_validbits_num_config((uint32_t)num_last_valid);
    multi_input((uint8_t *)message_input_task_2, meg_len);

    /* configure the number of valid bits in last word of the key */
    hau_last_word_validbits_num_config((uint32_t)num_key_valid);
    multi_input((uint8_t *)key_task_2, key_len);

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

    printf("\r\n\r\n SHA256 message input:\r\n\r\n");
    for(i = 0; i < meg_len ; i++){
        printf("%c", message_input_task_2[i]);
    }
    printf("\r\n\r\n key input:\r\n\r\n");
    for(i = 0; i < key_len ; i++){
        printf("%c", key_task_2[i]);
    }

    printf("\r\n\r\n SHA256 message digest (256 bits):\r\n\r\n");
    data_display(32U);
    data_check((uint8_t *)&digest_para.out[0], (uint8_t *)&expected_data_256[0], 32U);
}

/*!
    \brief      restore context when a higher priority task finishes
    \param[in]  none
    \param[out] none
    \retval     none
*/
void task_context_restore(void)
{
    /* restore HAU context */
    hau_context_restore(&context_para);
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
