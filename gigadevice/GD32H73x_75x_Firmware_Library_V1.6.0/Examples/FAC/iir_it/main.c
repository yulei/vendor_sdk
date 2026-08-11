/*!
    \file    main.c
    \brief   FAC interrupt iir filter demo

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
#include "gd32h73x_75x_it.h"
#include "systick.h"
#include <stdio.h>

void iir_config(void);
void cache_enable(void);
void nvic_config(void);

#define iir_coeffa_size          2
#define iir_coeffb_size          3
#define iir_d0                   105
#define iir_d1                   106
#define iir_gain                 1
#define input_array_size         50
#define add_array_size           50

/* read data point */
int16_t *pOutput = NULL;
uint16_t pOutputSize = 0;
uint16_t curOutputCnt = 0;

/* write data point */
int16_t *pInput = NULL;
uint16_t pInputSize = 0;
uint16_t curInputCnt = 0;

/* calculate success flag */
__IO uint8_t rd_complete = 0;
__IO uint8_t wr_complete = 0;

/*
    aray of filter (IIR) coefficients A (feedback coefficients) in Q1.15 format
    IIR filter order is 2 order, the output data is multiply by 32767
    the sampling frequency is 16KHz and the cut-off frequency is 500Hz
*/
static int16_t iir_coeffa[iir_coeffa_size] = {
    28242, -12412,
};

/*
    array of filter (FIR) coefficients B (feed-forward taps) in Q1.15 format
    FIR filter order is 20 order, the output data is multiply by 32767
    the sampling frequency is 16KHz and the cut-off frequency is 800Hz
*/
static int16_t iir_coeffb[iir_coeffb_size] = {
    138, 277, 138,
};

/*
    array of input values in -32767~32768
    the data is calculated by using 'x[i]=25000*(sin(2*pi*500/16000*i)+0.2*sin(2*pi*5*500/16000*i));' i goes from 1 to 50
    the useful signal frequency is 500Hz, and the noise signal frequency is 2500Hz
*/
static int16_t input_data[input_array_size] = {
    9035, 14186, 14865, 14142, 15883, 21184, 27297, 30000, 27297, 21184,
    15883, 14142, 14865, 14186, 9035, 0, -9035, -14186, -14865, -14142,
    -15883, -21184, -27297, -30000, -27297, -21184, -15883, -14142, -14865, -14186,
    -9035, 0, 9035, 14186, 14865, 14142, 15883, 21184, 27297, 30000,
    27297, 21184, 15883, 14142, 14865, 14186, 9035, 0, -9035, -14186,
};

/*
    array of input values in -32767~32768
    the data is calculated by using 'x[i]=25000*(sin(2*pi*500/16000*i)+0.2*sin(2*pi*5*500/16000*i));' i goes from 51 to 100
    the useful signal frequency is 500Hz, and the noise signal frequency is 2500Hz
*/
int16_t add_data[add_array_size] = {
    -14865, -14142, -15883, -21184, -27297, -30000, -27297, -21184, -15883, -14142,
        -14865, -14186, -9035, 0, 9035, 14186, 14865, 14142, 15883, 21184,
        27297, 30000, 27297, 21184, 15883, 14142, 14865, 14186, 9035, 0,
        -9035, -14186, -14865, -14142, -15883, -21184, -27297, -30000, -27297, -21184,
        -15883, -14142, -14865, -14186, -9035, 0, 9035, 14186, 14865, 14142,
    };

/* array of output data to preload in Q1.15 format */
static int16_t output_data[iir_coeffa_size] = {
    0, 0
};

/* array of calculated filtered data by computer */
const int16_t reference_data[input_array_size + add_array_size - iir_coeffb_size + 1] = {
    1079, 2044, 3204, 4541, 6122, 8005, 10128, 12287, 14228, 15784,
    16927, 17711, 18146, 18135, 17514, 16178, 14178, 11694, 8918, 5948,
    2775, -615, -4130, -7546, -10600, -13121, -15086, -16554, -17548, -17979,
    -17698, -16615, -14791, -12419, -9704, -6754, -3568, -141, 3426, 6906,
    10030, 12624, 14660, 16197, 17255, 17745, 17516, 16478, 14694, 12355,
    9667, 6738, 3570, 156, -3402, -6876, -9996, -12589, -14625, -16163,
    -17222, -17715, -17489, -16455, -14673, -12338, -9653, -6727, -3561, -149,
    3407, 6879, 9999, 12590, 14625, 16162, 17222, 17714, 17488, 16453,
    14672, 12336, 9652, 6726, 3560, 148, -3408, -6880, -9999, -12590,
    -14625, -16163, -17222, -17714, -17488, -16453, -14672, -12336,
};

/* array of calculated filtered data in Q1.15 format */
int16_t calculated_data[input_array_size + add_array_size - iir_coeffb_size + 1] = {0};
volatile int16_t error[input_array_size + add_array_size - iir_coeffb_size + 1] = {0};
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
    \brief      enable the CPU Chache
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_config(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    nvic_irq_enable(FAC_IRQn, 1, 0);
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    uint16_t i = 0;
    /* enable the CPU Chache */
    cache_enable();
    nvic_config();
    iir_config();

    while((wr_complete == 0) || (rd_complete == 0));
    for(i = 0; i < input_array_size + add_array_size - iir_coeffb_size + 1; i++) {
        error[i] = reference_data[i] - calculated_data[i];
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
void iir_config(void)
{
    fac_parameter_struct facconfig;
    fac_fixed_data_preload_struct faccoeff;

    /* reset the FAC */
    fac_deinit();

    rcu_periph_clock_enable(RCU_FAC);

    /* initialize the FAC filter parameter struct */
    fac_struct_para_init(&facconfig);

    /* Configure Coefficient */
    facconfig.coeff_addr        = 0;
    facconfig.coeff_size        = iir_coeffa_size + iir_coeffb_size;

    /* Configure input buffer */
    facconfig.input_addr        = iir_coeffa_size + iir_coeffb_size;
    facconfig.input_size        = iir_coeffb_size + iir_d0;
    facconfig.input_threshold   = FAC_THRESHOLD_1;

    /* Configure input buffer */
    facconfig.output_addr       = iir_coeffa_size + iir_coeffb_size + iir_coeffb_size + iir_d0;
    facconfig.output_size       = iir_coeffa_size + iir_d1;
    facconfig.output_threshold  = FAC_THRESHOLD_1;
    facconfig.clip =  FAC_CP_ENABLE;
    fac_init(&facconfig);

    faccoeff.coeffa_ctx        = iir_coeffa;
    faccoeff.coeffa_size       = iir_coeffa_size;
    faccoeff.coeffb_ctx        = iir_coeffb;
    faccoeff.coeffb_size       = iir_coeffb_size;
    faccoeff.input_ctx         = input_data;
    faccoeff.input_size        = input_array_size;
    faccoeff.output_ctx        = output_data;
    faccoeff.output_size       = iir_coeffa_size;
    /* preload X0  X1 Y buffer with coefficient */
    fac_fixed_buffer_preload(&faccoeff);

    facconfig.func             = FUNC_IIR_DIRECT_FORM_1;
    facconfig.ipp              = iir_coeffb_size;
    facconfig.ipq              = iir_coeffa_size;
    facconfig.ipr              = iir_gain;
    fac_function_config(&facconfig);

    fac_interrupt_enable(FAC_CTL_WIE | FAC_CTL_RIE);
    /* start filter calculation */
    fac_start();
}
