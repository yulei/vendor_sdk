/*!
    \file    main.c
    \brief   TRNG in NIST mode example

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

ErrStatus trng_configuration(void);
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
    uint32_t random_data[8] = {0};
    uint8_t retry = 0, i = 0;

    /* enable the CPU cache */
    cache_enable();
    /* initialize COM port */
    gd_eval_com_init(EVAL_COM);

    /* show example introduce */
    printf("============ Gigadevice TRNG NIST mode demo ============ \r\n");

/* configure TRNG module */
    while((ERROR == trng_configuration()) && retry < 3){
        printf("TRNG init fail and retry %d\r\n", retry + 1);
        retry++;
    }
    
    if(retry >= 3U){
        printf("TRNG init fail! Please reboot!\r\n");
        while(1){
        }
    }

    printf("TRNG init successful \r\n");

    /* disable TRNG module */
    trng_disable();

    if(SUCCESS == trng_ready_check()) {
        printf("Get 8 random data:  \r\n");
        for(i = 0; i < 8; i++) {
            random_data[i] = trng_get_true_random_data();
            printf("random data [%d]: 0x%08x \r\n", i, random_data[i]);
        }
    } else {
        printf("Error! \r\n");
    }

    while(1) {
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
ErrStatus trng_configuration(void)
{
    ErrStatus reval = SUCCESS;

    /* TRNG clock configuration */
    rcu_osci_on(RCU_IRC48M);
    while(rcu_flag_get(RCU_FLAG_IRC48MSTB) != SET);
    rcu_ck48m_clock_config(RCU_CK48MSRC_IRC48M);

    /* TRNG module clock enable */
    rcu_periph_clock_enable(RCU_TRNG);

    /* TRNG registers reset */
    trng_deinit();
    /* configure as NIST mode */
    trng_conditioning_reset_enable();
    trng_mode_config(TRNG_MODSEL_NIST);
    /* enable TRNG module clock error detection */
    trng_clockerror_detection_enable();
    trng_conditioning_input_bitwidth(TRNG_INMOD_440BIT);
    /* configured to output 8 32-bit random numbers */
    trng_conditioning_output_bitwidth(TRNG_OUTMOD_256BIT);
    /* the sha256 algorithm is configured */
    trng_conditioning_algo_config(TRNG_ALGO_SHA256);
    trng_conditioning_enable();
    trng_postprocessing_enable();

    /* enable TRNG module */
    trng_enable();
    /* check TRNG work status */
    reval = trng_ready_check();

    return reval;
}
