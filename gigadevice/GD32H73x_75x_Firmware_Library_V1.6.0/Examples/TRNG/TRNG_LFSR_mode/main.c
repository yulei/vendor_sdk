/*!
    \file    main.c
    \brief   TRNG in LFSR mode example
    
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
#include "gd32h759i_eval.h"

__IO FlagStatus g_tamperbutton_flag = RESET;
void trng_configuration(void);
ErrStatus trng_ready_check(void);

/*!
    \brief      enable the CPU Chache
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cache_enable(void)
{
    /* Enable I-Cache */
    SCB_EnableICache();

    /* Enable D-Cache */
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
    uint32_t random_data = 0, random_lastdata = 0;

    /* enable the CPU cache */
    cache_enable();
    /* initialize COM port */
    gd_eval_com_init(EVAL_COM);

    /* configure the tamper key */
    gd_eval_key_init(KEY_TAMPER, KEY_MODE_EXTI);

    /* show example introduce */
    printf("============ Gigadevice TRNG LFSR mode demo ============ \r\n");

    /* configure TRNG module */
    trng_configuration();

    printf("TRNG init successful! \r\n");

    while (1){
        /* get random data if TEMPER key is pressed */
        if(SET == g_tamperbutton_flag){
            g_tamperbutton_flag = RESET;
            
            /* enable TRNG module */
            trng_enable();
            /* check whether the random data is valid and get it */
            if(SUCCESS == trng_ready_check()){
                random_data = trng_get_true_random_data();
                if(random_data != random_lastdata){
                    random_lastdata = random_data;
                    printf("Get random number: 0x%08x \r\n", random_data);
                }else{
                    /* the random data is invalid */
                    printf("Error: Get the the same random number! \r\n");
                }
            }
            /* disable TRNG module */
            trng_disable();
        }

    }
}

/*!
    \brief      check whether the TRNG module is ready
    \param[in]  none
    \param[out] none
    \retval     ErrStatus: SUCCESS or ERROR
*/
ErrStatus trng_ready_check(void)
{
    uint32_t timeout = 0;
    FlagStatus trng_flag = RESET;

    /* check wherther the random data is valid */
    do{
        timeout++;
        trng_flag = trng_flag_get(TRNG_FLAG_DRDY);
    }while((RESET == trng_flag) &&(0xFFFF > timeout));
    
    if(SET == trng_flag){
        if(RESET == trng_interrupt_flag_get(TRNG_INT_FLAG_CEIF)){
            if(RESET == trng_interrupt_flag_get(TRNG_INT_FLAG_SEIF)){
                if(RESET == trng_flag_get(TRNG_FLAG_CECS)){
                    if(RESET == trng_flag_get(TRNG_FLAG_SECS)){
                        return SUCCESS;
                    }
                }
            }
        }
    }
    printf("error occurred! : %x \r\n", TRNG_STAT);
    /* return check status */
    return ERROR;
}

/*!
    \brief      configure TRNG module
    \param[in]  none
    \param[out] none
    \retval     ErrStatus: SUCCESS or ERROR
*/
void trng_configuration(void)
{
    /* TRNG clock configuration */
    rcu_osci_on(RCU_IRC48M);
    while(rcu_flag_get(RCU_FLAG_IRC48MSTB) != SET);
    rcu_ck48m_clock_config(RCU_CK48MSRC_IRC48M);

    /* TRNG module clock enable */
    rcu_periph_clock_enable(RCU_TRNG);

    /* TRNG registers reset */
    trng_deinit();
    /* configure as LFSR mode */
    trng_conditioning_reset_enable();
    trng_mode_config(TRNG_MODSEL_LFSR);
    trng_conditioning_disable();

}
