/*!
    \file    main.c
    \brief   CAU DES/TDES modes example

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
    cau_parameter_struct text;

    /* enable the CPU cache */
    cache_enable();

    gd_eval_led_init(LED1);
    gd_eval_com_init(EVAL_COM);
    /* enable CAU clock */
    rcu_periph_clock_enable(RCU_CAU);

    printf("\r\nplain text :\r\n");
    data_display(DATA_SIZE, plaintext);

    /* reset the CAU peripheral */
    cau_deinit();

    cau_struct_para_init(&text);
    /* encryption in DES ECB mode */
    text.alg_dir   = CAU_ENCRYPT;
    text.key       = des_key;
    text.input     = plaintext;
    text.in_length = DATA_SIZE;

    if(SUCCESS == cau_des_ecb(&text, encrypt_result)) {
        printf(" \r\n encrypted data with DES  Mode  ECB :\r\n\r\n");
        data_display(DATA_SIZE, encrypt_result);
        data_check((uint8_t *)&encrypt_result[0], (uint8_t *)&exp_des_ecb[0], DATA_SIZE);
    }

    /* decryption in DES ECB mode */
    text.alg_dir   = CAU_DECRYPT;
    text.key       = des_key;
    text.input     = encrypt_result;
    text.in_length = DATA_SIZE;
    if(SUCCESS == cau_des_ecb(&text, decrypt_result)) {
        printf(" \r\n decrypted data with DES  Mode  ECB :\r\n\r\n");
        data_display(DATA_SIZE, decrypt_result);
        data_check((uint8_t *)&decrypt_result[0], (uint8_t *)&plaintext[0], DATA_SIZE);
    }

    /* encryption in DES CBC mode */
    text.alg_dir   = CAU_ENCRYPT;
    text.key       = des_key;
    text.iv        = vectors;
    text.input     = plaintext;
    text.in_length = DATA_SIZE;
    if(SUCCESS == cau_des_cbc(&text, encrypt_result)) {
        printf(" \r\n encrypted data with DES  Mode  CBC :\r\n\r\n");
        data_display(DATA_SIZE, encrypt_result);
        data_check((uint8_t *)&encrypt_result[0], (uint8_t *)&exp_des_cbc[0], DATA_SIZE);
    }

    /* decryption in DES CBC mode */
    text.alg_dir   = CAU_DECRYPT;
    text.key       = des_key;
    text.iv        = vectors;
    text.input     = encrypt_result;
    text.in_length = DATA_SIZE;
    if(SUCCESS == cau_des_cbc(&text, decrypt_result)) {
        printf(" \r\n decrypted data with DES  Mode  CBC :\r\n\r\n");
        data_display(DATA_SIZE, decrypt_result);
        data_check((uint8_t *)&decrypt_result[0], (uint8_t *)&plaintext[0], DATA_SIZE);
    }

    /* encryption in TDES ECB mode */
    text.alg_dir   = CAU_ENCRYPT;
    text.key       = tdes_key;
    text.input     = plaintext;
    text.in_length = DATA_SIZE;
    if(SUCCESS == cau_tdes_ecb(&text, encrypt_result)) {
        printf(" \r\n encrypted data with TDES  Mode  ECB :\r\n\r\n");
        data_display(DATA_SIZE, encrypt_result);
        data_check((uint8_t *)&encrypt_result[0], (uint8_t *)&exp_tdes_ecb[0], DATA_SIZE);
    }

    /* decryption in TDES ECB mode */
    text.alg_dir   = CAU_DECRYPT;
    text.key       = tdes_key;
    text.input     = encrypt_result;
    text.in_length = DATA_SIZE;
    if(SUCCESS == cau_tdes_ecb(&text, decrypt_result)) {
        printf(" \r\n decrypted data with TDES  Mode  ECB :\r\n\r\n");
        data_display(DATA_SIZE, decrypt_result);
        data_check((uint8_t *)&decrypt_result[0], (uint8_t *)&plaintext[0], DATA_SIZE);
    }

    /* encryption in TDES CBC mode */
    text.alg_dir   = CAU_ENCRYPT;
    text.key       = tdes_key;
    text.iv        = vectors;
    text.input     = plaintext;
    text.in_length = DATA_SIZE;
    if(SUCCESS == cau_tdes_cbc(&text, encrypt_result)) {
        printf(" \r\n encrypted data with TDES  Mode  CBC :\r\n\r\n");
        data_display(DATA_SIZE, encrypt_result);
        data_check((uint8_t *)&encrypt_result[0], (uint8_t *)&exp_tdes_cbc[0], DATA_SIZE);
    }

    /* decryption in TDES CBC mode */
    text.alg_dir   = CAU_DECRYPT;
    text.key       = tdes_key;
    text.iv        = vectors;
    text.input     = encrypt_result;
    text.in_length = DATA_SIZE;
    if(SUCCESS == cau_tdes_cbc(&text, decrypt_result)) {
        printf(" \r\n decrypted data with TDES  Mode  CBC :\r\n\r\n");
        data_display(DATA_SIZE, decrypt_result);
        data_check((uint8_t *)&decrypt_result[0], (uint8_t *)&plaintext[0], DATA_SIZE);
    }

    /* check result */
    if(0U == check_flag){
        gd_eval_led_on(LED1);
    }

    while(1) {
    }
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
