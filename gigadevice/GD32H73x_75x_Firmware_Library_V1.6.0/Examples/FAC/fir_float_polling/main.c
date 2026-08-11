/*!
    \file    main.c
    \brief   FAC float fir filter demo

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
#include <stdio.h>
#include "gd32h759i_eval.h"

void fir_config(void);
void cache_enable(void);

#define fir_coeffb_size         21
#define fir_d0                  105
#define fir_d1                  106
#define fir_gain                0
#define input_array_size        100

/*
    Array of filter (FIR) coefficients B (feed-forward taps) in float format
    FIR filter order is 20 order, the sampling frequency is 16KHz and the cut-off frequency is 800Hz
*/
static float fir_coeffb[fir_coeffb_size] = {
    0.000000000000000f, 0.001327222896911f, 0.004650252250658f, 0.011749802608142f, 0.023779221956107f,
    0.040723451189875f, 0.061156926116235f, 0.082403674188359f, 0.101082211156672f, 0.113897378800498f,
    0.118459717673086f, 0.113897378800498f, 0.101082211156672f, 0.082403674188359f, 0.061156926116235f,
    0.040723451189875f, 0.023779221956107f, 0.011749802608142f, 0.004650252250658f, 0.001327222896911f,
    0.000000000000000f,
};


/*
    Array of input values in float format
    the data is calculated by using 'x[i]=(sin(2*pi*500/16000*i)+0.2*sin(2*pi*5*500/16000*i));' i goes from 1 to 100
    The useful signal frequency is 500Hz, and the noise signal frequency is 2500Hz
*/
static float input_data[input_array_size] = {
    0.361384244476637f, 0.567459338867347f, 0.594588297422828f, 0.565685424949238f, 0.635312556221899f,
    0.847342846038269f, 1.091899327007151f, 1.200000000000000f, 1.091899327007151f, 0.847342846038269f,
    0.635312556221899f, 0.565685424949238f, 0.594588297422828f, 0.567459338867347f, 0.361384244476638f,
    0.000000000000000f, -0.361384244476637f, -0.567459338867347f, -0.594588297422828f, -0.565685424949238f,
    -0.635312556221899f, -0.847342846038269f, -1.091899327007151f, -1.200000000000000f, -1.091899327007151f,
    -0.847342846038269f, -0.635312556221899f, -0.565685424949238f, -0.594588297422827f, -0.567459338867348f,
    -0.361384244476638f, -0.000000000000000f, 0.361384244476637f, 0.567459338867347f, 0.594588297422828f,
    0.565685424949238f, 0.635312556221899f, 0.847342846038269f, 1.091899327007150f, 1.200000000000000f,
    1.091899327007152f, 0.847342846038269f, 0.635312556221900f, 0.565685424949239f, 0.594588297422827f,
    0.567459338867348f, 0.361384244476637f, 0.000000000000001f, -0.361384244476636f, -0.567459338867348f,
    -0.594588297422828f, -0.565685424949239f, -0.635312556221899f, -0.847342846038268f, -1.091899327007151f,
    -1.200000000000000f, -1.091899327007151f, -0.847342846038269f, -0.635312556221900f, -0.565685424949238f,
    -0.594588297422828f, -0.567459338867348f, -0.361384244476637f, -0.000000000000001f, 0.361384244476636f,
    0.567459338867348f, 0.594588297422828f, 0.565685424949238f, 0.635312556221899f, 0.847342846038267f,
    1.091899327007150f, 1.200000000000000f, 1.091899327007151f, 0.847342846038270f, 0.635312556221900f,
    0.565685424949239f, 0.594588297422829f, 0.567459338867348f, 0.361384244476638f, 0.000000000000001f,
    -0.361384244476635f, -0.567459338867346f, -0.594588297422829f, -0.565685424949238f, -0.635312556221899f,
    -0.847342846038267f, -1.091899327007150f, -1.200000000000000f, -1.091899327007152f, -0.847342846038270f,
    -0.635312556221900f, -0.565685424949240f, -0.594588297422828f, -0.567459338867346f, -0.361384244476637f,
    -0.000000000000003f, 0.361384244476635f, 0.567459338867345f, 0.594588297422828f, 0.565685424949240f,
};

/* Array of calculated filtered data by computer */
const float reference_data[input_array_size - fir_coeffb_size + 1] = {
    0.682198598352373f, 0.580077928792056f, 0.455266022467437f, 0.313068628218172f, 0.159361719332136f,
    0.000000000000000f, -0.159361719332136f, -0.313068628218172f, -0.455266022467437f, -0.580077928792056f,
    -0.682198598352373f, -0.757549097447795f, -0.803571381795667f, -0.819026492302533f, -0.803571381795667f,
    -0.757549097447796f, -0.682198598352373f, -0.580077928792056f, -0.455266022467437f, -0.313068628218172f,
    -0.159361719332136f, -0.000000000000000f, 0.159361719332136f, 0.313068628218172f, 0.455266022467437f,
    0.580077928792056f, 0.682198598352373f, 0.757549097447795f, 0.803571381795667f, 0.819026492302532f,
    0.803571381795667f, 0.757549097447796f, 0.682198598352373f, 0.580077928792057f, 0.455266022467437f,
    0.313068628218172f, 0.159361719332136f, 0.000000000000000f, -0.159361719332136f, -0.313068628218171f,
    -0.455266022467437f, -0.580077928792056f, -0.682198598352373f, -0.757549097447795f, -0.803571381795667f,
    -0.819026492302533f, -0.803571381795667f, -0.757549097447796f, -0.682198598352373f, -0.580077928792057f,
    -0.455266022467437f, -0.313068628218172f, -0.159361719332136f, -0.000000000000000f, 0.159361719332136f,
    0.313068628218171f, 0.455266022467437f, 0.580077928792056f, 0.682198598352373f, 0.757549097447795f,
    0.803571381795667f, 0.819026492302532f, 0.803571381795667f, 0.757549097447796f, 0.682198598352373f,
    0.580077928792057f, 0.455266022467438f, 0.313068628218172f, 0.159361719332137f, 0.000000000000001f,
    -0.159361719332135f, -0.313068628218171f, -0.455266022467436f, -0.580077928792056f, -0.682198598352372f,
    -0.757549097447795f, -0.803571381795667f, -0.819026492302533f, -0.803571381795667f, -0.757549097447796f,
};

/* Array of calculated filtered data by mcu */
float calculated_data[input_array_size - fir_coeffb_size + 1] = {0.0f};

/* error between reference_data and calculated_data */
volatile float error[input_array_size - fir_coeffb_size + 1] = {0.0f};

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
        calculated_data[i] = (fac_float_data_read());
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
void fir_config(void)
{
    fac_parameter_struct facconfig;
    fac_float_data_preload_struct faccoeff;

    /* reset the FAC */
    fac_deinit();

    rcu_periph_clock_enable(RCU_FAC);

    /* initialize the FAC filter parameter struct */
    fac_struct_para_init(&facconfig);

    /* Configure Coefficient buffer */
    facconfig.coeff_addr        = 0;
    facconfig.coeff_size        = fir_coeffb_size;

    /* Configure input buffer */
    facconfig.input_addr        = fir_coeffb_size;
    facconfig.input_size        = fir_coeffb_size + fir_d0;
    facconfig.input_threshold   = FAC_THRESHOLD_1;

    /* Configure input buffer */
    facconfig.output_addr       = fir_coeffb_size + fir_coeffb_size + fir_d0;
    facconfig.output_size       = fir_d1;
    facconfig.output_threshold  = FAC_THRESHOLD_1;
    facconfig.clip =  FAC_CP_ENABLE;
    fac_init(&facconfig);

    fac_float_enable();

    faccoeff.coeffa_ctx        = NULL;
    faccoeff.coeffa_size       = 0;
    faccoeff.coeffb_ctx        = fir_coeffb;
    faccoeff.coeffb_size       = fir_coeffb_size;
    faccoeff.input_ctx         = input_data;
    faccoeff.input_size        = input_array_size;
    faccoeff.output_ctx        = NULL;
    faccoeff.output_size       = 0;

    /* preload X0  X1 Y buffer with coefficient */
    fac_float_buffer_preload(&faccoeff);

    facconfig.func             = FUNC_CONVO_FIR;
    facconfig.ipp              = fir_coeffb_size;
    facconfig.ipq              = 0;
    facconfig.ipr              = fir_gain;
    fac_function_config(&facconfig);

    /* start filter calculation */
    fac_start();
}
