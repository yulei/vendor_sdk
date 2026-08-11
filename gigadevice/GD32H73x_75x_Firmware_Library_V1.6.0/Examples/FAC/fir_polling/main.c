/*!
    \file    main.c
    \brief   FAC polling fir filter demo

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

#include "gd32h73x_75x.h"
#include "gd32h759i_eval.h"
#include <stdio.h>

void fir_config(void);
void cache_enable(void);

#define fir_coeffb_size          21
#define fir_d0                   107
#define fir_d1                   108
#define fir_gain                 0
#define input_array_size         100

/*
    Array of filter (FIR) coefficients B (feed-forward taps) in Q1.15 format
    FIR filter order is 20 order, the output data is multiply by 32767
    the sampling frequency is 16KHz and the cut-off frequency is 800Hz
*/
static int16_t fir_coeffb[fir_coeffb_size] = {
    0, 43, 152, 385, 779, 1334, 2004, 2700, 3312, 3732,
    3882, 3732, 3312, 2700, 2004, 1334, 779, 385, 152, 43,
    0
};

/*
    Array of input values in -32767~32768
    the data is calculated by using 'x[i]=25000*(sin(2*pi*500/16000*i)+0.2*sin(2*pi*5*500/16000*i));' i goes from 1 to 100
    The useful signal frequency is 500Hz, and the noise signal frequency is 2500Hz
*/
static int16_t input_data[input_array_size] = {
    9035, 14186, 14865, 14142, 15883, 21184, 27297, 30000, 27297, 21184,
    15883, 14142, 14865, 14186, 9035, 0, -9035, -14186, -14865, -14142,
    -15883, -21184, -27297, -30000, -27297, -21184, -15883, -14142, -14865, -14186,
    -9035, 0, 9035, 14186, 14865, 14142, 15883, 21184, 27297, 30000,
    27297, 21184, 15883, 14142, 14865, 14186, 9035, 0, -9035, -14186,
    -14865, -14142, -15883, -21184, -27297, -30000, -27297, -21184, -15883, -14142,
    -14865, -14186, -9035, 0, 9035, 14186, 14865, 14142, 15883, 21184,
    27297, 30000, 27297, 21184, 15883, 14142, 14865, 14186, 9035, 0,
    -9035, -14186, -14865, -14142, -15883, -21184, -27297, -30000, -27297, -21184,
    -15883, -14142, -14865, -14186, -9035, 0, 9035, 14186, 14865, 14142,
};

/* Array of calculated filtered data by mcu */
int16_t calculated_data[input_array_size - fir_coeffb_size + 1] = {0};

/* Array of calculated filtered data by computer */
int16_t reference_data[input_array_size - fir_coeffb_size + 1] = {
    17055, 14502, 11381, 7827, 3984, 0, -3984, -7827, -11381, -14502,
    -17055, -18938, -20089, -20475, -20089, -18938, -17055, -14502, -11381, -7827,
    -3984, 0, 3984, 7827, 11381, 14502, 17055, 18938, 20089, 20475,
    20089, 18938, 17055, 14502, 11381, 7827, 3984, 0, -3984, -7827,
    -11381, -14502, -17055, -18938, -20089, -20475, -20089, -18938, -17055, -14502,
    -11381, -7827, -3984, 0, 3984, 7827, 11381, 14502, 17055, 18938,
    20089, 20475, 20089, 18938, 17055, 14502, 11381, 7827, 3984, 0,
    -3984, -7827, -11381, -14502, -17055, -18938, -20089, -20475, -20089, -18938,
};

/* error between reference_data and calculated_data */
volatile int16_t error[input_array_size - fir_coeffb_size + 1];

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
    uint16_t i;
    /* enable the CPU Chache */
    cache_enable();
    fir_config();

    /* get calculation result */
    for(i = 0; i < input_array_size - fir_coeffb_size + 1; i++) {
        while(fac_flag_get(FAC_FLAG_YBEF) != RESET);
        calculated_data[i] = (fac_fixed_data_read());
        error[i] = calculated_data[i] - reference_data[i];
    }
    /* finish calculation */
    fac_finish_calculate();

    while(1) {
    }
}

/*!
    \brief      configure the fir peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fir_config(void)
{
    fac_parameter_struct facconfig;
    fac_fixed_data_preload_struct faccoeff;

    /* reset the FAC */
    fac_deinit();

    rcu_periph_clock_enable(RCU_FAC);

    /* initialize the FAC filter parameter struct */
    fac_struct_para_init(&facconfig);

    /* Configure Coefficient buffer,X2 buffer */
    facconfig.coeff_addr        = 0;
    facconfig.coeff_size        = fir_coeffb_size;

    /* Configure input buffer, X1 buffer */
    facconfig.input_addr        = fir_coeffb_size;
    facconfig.input_size        = fir_coeffb_size + fir_d0;
    facconfig.input_threshold   = FAC_THRESHOLD_1;

    /* Configure input buffer, Y buffer */
    facconfig.output_addr       = fir_coeffb_size + fir_coeffb_size + fir_d0;
    facconfig.output_size       = fir_d1;
    facconfig.output_threshold  = FAC_THRESHOLD_1;
    facconfig.clip =  FAC_CP_ENABLE;
    fac_init(&facconfig);

    faccoeff.coeffa_ctx        = NULL;
    faccoeff.coeffa_size       = 0;
    faccoeff.coeffb_ctx        = fir_coeffb;
    faccoeff.coeffb_size       = fir_coeffb_size;
    faccoeff.input_ctx         = input_data;
    faccoeff.input_size        = input_array_size;
    faccoeff.output_ctx        = NULL;
    faccoeff.output_size       = 0;
    /* preload X0  X1 Y buffer with coefficient */
    fac_fixed_buffer_preload(&faccoeff);

    facconfig.func             = FUNC_CONVO_FIR;
    facconfig.ipp              = fir_coeffb_size;
    facconfig.ipq              = 0;
    facconfig.ipr              = fir_gain;
    fac_function_config(&facconfig);

    /* start filter calculation */
    fac_start();
}
