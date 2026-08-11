/*!
    \file    main.c
    \brief   Hmac SHA1 MD5 example

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

static uint8_t md5_output[16];
static uint8_t sha1_output[20];
__IO uint8_t check_flag = 0;

/* printf data in bytes */
static void data_display(uint32_t datalength, uint8_t *data);
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
    uint32_t i = 0, meg_len = 0, key_len = 0;

    /* enable the CPU cache */
    cache_enable();

    gd_eval_led_init(LED1);
    gd_eval_com_init(EVAL_COM);
    /* enable HAU clock */
    rcu_periph_clock_enable(RCU_HAU);

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

    /* HMAC SHA-1 digest computation */
    hau_hmac_sha_1((uint8_t*)key, key_len,(uint8_t *)message_input, meg_len, sha1_output);
    printf(" \r\n\r\nSHA1 message digest (160 bits):\r\n\r\n");
    data_display(20, sha1_output);
    data_check((uint8_t *)&sha1_output[0], (uint8_t *)&expected_data_sha1[0], 20U);

    /* HMAC MD5 digest computation */
    hau_hmac_md5((uint8_t*)key, key_len,(uint8_t *)message_input, meg_len, md5_output);
    printf(" \r\n\r\nMD5 Message Digest (128 bits):\r\n\r\n");
    data_display(16, md5_output);
    data_check((uint8_t *)&md5_output[0], (uint8_t *)&expected_data_md5[0], 16U);

    if(0U == check_flag){
        gd_eval_led_on(LED1);
    }

    while(1) {
    }
}

/*!
    \brief      printf data in bytes
    \param[in]  datalength: length of the data to display
    \param[in]  data: pointer to the data to display
    \param[out] none
    \retval     none
*/
static void data_display(uint32_t datalength, uint8_t *data)
{
    uint32_t i =0, count = 0;

    for(i = 0; i < datalength; i++){
        printf(" 0x%02X", data[i]);
        count++;

        if(4U == count){
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
static void data_check(uint8_t *src, uint8_t *dst, uint32_t len)
{
    if(memcmp((const void*)src, (const void*)dst, len) != 0){
        check_flag++;
    }
}
