/*!
    \file    main.c
    \brief   CAU gcm, ccm, cfb, ofb example

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
#include <string.h>
#include "main.h"

void cau_aes_encrypt(void);
void cau_aes_decrypt(void);
void cau_parameter_printf(char *name, uint8_t *data, uint32_t data_size);
/* check data in bytes */
void data_check(uint8_t *src, uint8_t *dst, uint32_t len);

uint8_t encrypt_result[TEXT_SIZE];

uint8_t decrypt_result[TEXT_SIZE];

uint8_t ccm_tag[CCM_TAG_SIZE]; /* used for storing computed MAC (TAG) */

uint8_t gcm_tag[GCM_TAG_SIZE];

uint8_t aad_buf[AAD_SIZE + 21]; /* buffer used to append the aad. aad_buf[] size must be equal to AAD_SIZE + 21 */

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
    /* enable the CPU cache */
    cache_enable();

    gd_eval_led_init(LED1);
    gd_eval_com_init(EVAL_COM);

    rcu_periph_clock_enable(RCU_CAU);

    /* print input parameters */
    cau_parameter_printf("plain text", (uint8_t *)plaintext, PLAINTEXT_SIZE);

    /* encryption in GCM, CCM, CFB, OFB mode */
    cau_aes_encrypt();

    /* decryption in GCM, CCM, CFB, OFB mode */
    cau_aes_decrypt();
    
    /* check result */
    if(0U == check_flag){
        gd_eval_led_on(LED1);
    }

    while(1) {
    }
}

/*!
    \brief      encryption in GCM, CCM, CFB, OFB mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cau_aes_encrypt(void)
{
    cau_parameter_struct cau_gcm_parameter;
    cau_parameter_struct cau_ccm_parameter;
    cau_parameter_struct cau_cfb_parameter;
    cau_parameter_struct cau_ofb_parameter;

    /* --------- encryption in GCM mode --------- */
    cau_deinit();
    cau_gcm_parameter.alg_dir     = CAU_ENCRYPT;
    cau_gcm_parameter.key         = (uint8_t *)key_128;
    cau_gcm_parameter.key_size    = KEY_SIZE;
    cau_gcm_parameter.iv          = (uint8_t *)vectors;
    cau_gcm_parameter.iv_size     = IV_SIZE;
    cau_gcm_parameter.input       = (uint8_t *)plaintext;
    cau_gcm_parameter.in_length   = PLAINTEXT_SIZE;
    cau_gcm_parameter.aad         = (uint8_t *)aadmessage;
    cau_gcm_parameter.aad_size    = AAD_SIZE;
    cau_aes_gcm(&cau_gcm_parameter, encrypt_result, gcm_tag);
    printf("\r\nencryption in GCM mode\r\n");
    /* print output parameters */
    cau_parameter_printf("encrypted data", encrypt_result, PLAINTEXT_SIZE);
    data_check((uint8_t *)&encrypt_result[0], (uint8_t *)&ciphertext_gcm[0], PLAINTEXT_SIZE);
    cau_parameter_printf("tag", gcm_tag, GCM_TAG_SIZE);
    data_check((uint8_t *)&gcm_tag[0], (uint8_t *)&exp_gcm_tag[0], GCM_TAG_SIZE);

    /* --------- encryption in CCM mode --------- */
    cau_deinit();
    cau_ccm_parameter.alg_dir     = CAU_ENCRYPT;
    cau_ccm_parameter.key         = (uint8_t *)key_128;
    cau_ccm_parameter.key_size    = KEY_SIZE;
    cau_ccm_parameter.iv          = (uint8_t *)ccm_vectors;
    cau_ccm_parameter.iv_size     = CCM_IV_SIZE;
    cau_ccm_parameter.input       = (uint8_t *)plaintext;
    cau_ccm_parameter.in_length   = PLAINTEXT_SIZE;
    cau_ccm_parameter.aad         = (uint8_t *)aadmessage;
    cau_ccm_parameter.aad_size    = AAD_SIZE;
    cau_aes_ccm(&cau_ccm_parameter, encrypt_result, ccm_tag, CCM_TAG_SIZE, aad_buf);
    printf("\r\nencryption in CCM mode\r\n");
    /* print output parameters */
    cau_parameter_printf("encrypted data", encrypt_result, PLAINTEXT_SIZE);
    data_check((uint8_t *)&encrypt_result[0], (uint8_t *)&ciphertext_ccm[0], PLAINTEXT_SIZE);
    cau_parameter_printf("tag", ccm_tag, CCM_TAG_SIZE);
    data_check((uint8_t *)&ccm_tag[0], (uint8_t *)&exp_ccm_tag[0], CCM_TAG_SIZE);

    /* --------- encryption in CFB mode --------- */
    cau_deinit();
    cau_cfb_parameter.alg_dir   = CAU_ENCRYPT;
    cau_cfb_parameter.key       = (uint8_t *)key_128;
    cau_cfb_parameter.key_size  = KEY_SIZE;
    cau_cfb_parameter.iv        = (uint8_t *)vectors;
    cau_cfb_parameter.iv_size   = IV_SIZE;
    cau_cfb_parameter.input     = (uint8_t *)plaintext;
    cau_cfb_parameter.in_length = PLAINTEXT_SIZE;
    cau_aes_cfb(&cau_cfb_parameter, encrypt_result);
    printf("\r\nencryption in CFB mode\r\n");
    /* print output parameters */
    cau_parameter_printf("encrypted data", encrypt_result, PLAINTEXT_SIZE);
    data_check((uint8_t *)&encrypt_result[0], (uint8_t *)&ciphertext_cfb[0], PLAINTEXT_SIZE);

    /* --------- encryption in OFB mode --------- */
    cau_deinit();
    cau_ofb_parameter.alg_dir   = CAU_ENCRYPT;
    cau_ofb_parameter.key       = (uint8_t *)key_128;
    cau_ofb_parameter.key_size  = KEY_SIZE;
    cau_ofb_parameter.iv        = (uint8_t *)vectors;
    cau_ofb_parameter.iv_size   = IV_SIZE;
    cau_ofb_parameter.input     = (uint8_t *)plaintext;
    cau_ofb_parameter.in_length = PLAINTEXT_SIZE;
    cau_aes_ofb(&cau_ofb_parameter, encrypt_result);
    printf("\r\nencryption in OFB mode\r\n");
    /* print output parameters */
    cau_parameter_printf("encrypted data", encrypt_result, PLAINTEXT_SIZE);
    data_check((uint8_t *)&encrypt_result[0], (uint8_t *)&ciphertext_ofb[0], PLAINTEXT_SIZE);
}

/*!
    \brief      decryption in GCM, CCM, CFB, OFB mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cau_aes_decrypt(void)
{
    cau_parameter_struct cau_gcm_parameter;
    cau_parameter_struct cau_ccm_parameter;
    cau_parameter_struct cau_cfb_parameter;
    cau_parameter_struct cau_ofb_parameter;

    /* --------- decryption in GCM mode --------- */
    cau_deinit();
    cau_gcm_parameter.alg_dir     = CAU_DECRYPT;
    cau_gcm_parameter.key         = (uint8_t *)key_128;
    cau_gcm_parameter.key_size    = KEY_SIZE;
    cau_gcm_parameter.iv          = (uint8_t *)vectors;
    cau_gcm_parameter.iv_size     = IV_SIZE;
    cau_gcm_parameter.input       = (uint8_t *)ciphertext_gcm;
    cau_gcm_parameter.in_length   = CIPHERTEXT_SIZE;
    cau_gcm_parameter.aad         = (uint8_t *)aadmessage;
    cau_gcm_parameter.aad_size    = AAD_SIZE;
    cau_aes_gcm(&cau_gcm_parameter, decrypt_result, gcm_tag);
    printf("\r\ndecryption in GCM mode\r\n");
    /* print output parameters */
    cau_parameter_printf("decrypted data", decrypt_result, CIPHERTEXT_SIZE);
    data_check((uint8_t *)&decrypt_result[0], (uint8_t *)&plaintext[0], PLAINTEXT_SIZE);
    cau_parameter_printf("tag", gcm_tag, GCM_TAG_SIZE);

    /* --------- decryption in CCM mode --------- */
    cau_deinit();
    cau_ccm_parameter.alg_dir     = CAU_DECRYPT;
    cau_ccm_parameter.key         = (uint8_t *)key_128;
    cau_ccm_parameter.key_size    = KEY_SIZE;
    cau_ccm_parameter.iv          = (uint8_t *)ccm_vectors;
    cau_ccm_parameter.iv_size     = CCM_IV_SIZE;
    cau_ccm_parameter.input       = (uint8_t *)ciphertext_ccm;
    cau_ccm_parameter.in_length   = CIPHERTEXT_SIZE;
    cau_ccm_parameter.aad         = (uint8_t *)aadmessage;
    cau_ccm_parameter.aad_size    = AAD_SIZE;
    cau_aes_ccm(&cau_ccm_parameter, decrypt_result, ccm_tag, CCM_TAG_SIZE, aad_buf);
    printf("\r\ndecryption in CCM mode\r\n");
    /* print output parameters */
    cau_parameter_printf("decrypted data", decrypt_result, CIPHERTEXT_SIZE);
    data_check((uint8_t *)&decrypt_result[0], (uint8_t *)&plaintext[0], PLAINTEXT_SIZE);
    cau_parameter_printf("tag", ccm_tag, CCM_TAG_SIZE);
    printf("\r\n");

    /* --------- decryption in CFB mode --------- */
    cau_deinit();
    cau_cfb_parameter.alg_dir   = CAU_DECRYPT;
    cau_cfb_parameter.key       = (uint8_t *)key_128;
    cau_cfb_parameter.key_size  = KEY_SIZE;
    cau_cfb_parameter.iv        = (uint8_t *)vectors;
    cau_cfb_parameter.iv_size   = IV_SIZE;
    cau_cfb_parameter.input     = (uint8_t *)ciphertext_cfb;
    cau_cfb_parameter.in_length = CIPHERTEXT_SIZE;
    cau_aes_cfb(&cau_cfb_parameter, decrypt_result);
    printf("\r\ndecryption in CFB mode\r\n");
    /* print output parameters */
    cau_parameter_printf("decrypted data", decrypt_result, CIPHERTEXT_SIZE);
    data_check((uint8_t *)&decrypt_result[0], (uint8_t *)&plaintext[0], PLAINTEXT_SIZE);

    /* --------- decryption in OFB mode --------- */
    cau_deinit();
    cau_ofb_parameter.alg_dir   = CAU_DECRYPT;
    cau_ofb_parameter.key       = (uint8_t *)key_128;
    cau_ofb_parameter.key_size  = KEY_SIZE;
    cau_ofb_parameter.iv        = (uint8_t *)vectors;
    cau_ofb_parameter.iv_size   = IV_SIZE;
    cau_ofb_parameter.input     = (uint8_t *)ciphertext_ofb;
    cau_ofb_parameter.in_length = CIPHERTEXT_SIZE;
    cau_aes_ofb(&cau_ofb_parameter, decrypt_result);
    printf("\r\ndecryption in OFB mode\r\n");
    cau_parameter_printf("decrypted data", decrypt_result, CIPHERTEXT_SIZE);
    data_check((uint8_t *)&decrypt_result[0], (uint8_t *)&plaintext[0], PLAINTEXT_SIZE);
}

/*!
    \brief      print cau parameter
    \param[in]  name: parameter name
    \param[in]  data: parameter raw data
    \param[in]  data_size: raw data size
    \param[out] none
    \retval     none
*/
void cau_parameter_printf(char *name, uint8_t *data, uint32_t data_size)
{
    uint32_t i = 0, count = 0;

    printf("%s:\n", name);
    for(i = 0; i < data_size; i++) {
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
