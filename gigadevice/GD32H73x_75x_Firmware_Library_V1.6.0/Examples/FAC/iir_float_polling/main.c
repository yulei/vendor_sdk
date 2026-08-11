/*!
    \file    main.c
    \brief   FAC iir float filter demo

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

void iir_config(void);
void cache_enable(void);

#define iir_coeffa_size   2
#define iir_coeffb_size   3
#define iir_d0            105
#define iir_d1            106
#define iir_gain          0
#define input_array_size  100

/*
    array of filter (IIR) coefficients A (feedback coefficients) in float format
    IIR filter order is 2 order
    the sampling frequency is 16KHz and the cut-off frequency is 500Hz
*/
float aFilterCoeffA_IIR[iir_coeffa_size] = {
    1.723776172762509f, -0.757546944478829f,
};

/*
    array of filter (IIR) coefficients B (feed-forward taps) in float format
    IIR filter order is 2 order
    the sampling frequency is 16KHz and the cut-off frequency is 500Hz
*/
float aFilterCoeffB_IIR[iir_coeffb_size] = {
    0.008442692929080f, 0.016885385858160f, 0.008442692929080f,
};

/*
    array of input values
    the data is calculated by using 'x[i]=(sin(2*pi*500/16000*i)+0.2*sin(2*pi*5*500/16000*i));' i goes from 1 to 100
    the useful signal frequency is 500Hz, and the noise signal frequency is 2500Hz
*/
static float aInputValues[input_array_size] = {
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

/* array of calculated filtered data by computer */
const float reference_data[input_array_size - iir_coeffb_size + 1] = {
    0.043184445016361f, 0.081810811178169f, 0.128244774527283f, 0.181747018022472f, 0.245029772512324f,
    0.320416813727530f, 0.405404917867519f, 0.491818787966269f, 0.569562281669030f, 0.631879331428592f,
    0.677683861324088f, 0.709103682126929f, 0.726611445315364f, 0.726229165936056f, 0.701414251983763f,
    0.648035491800538f, 0.568061163950519f, 0.468686346248479f, 0.357641865219304f, 0.238785354683498f,
    0.111791972130398f, -0.023909006386698f, -0.164601046913034f, -0.301345396371116f, -0.423649024352286f,
    -0.524650070983251f, -0.603380759384717f, -0.662252964050590f, -0.702139381658027f, -0.719536424030579f,
    -0.708416200005361f, -0.665175349143770f, -0.592302137314169f, -0.497488111978141f, -0.388925987414758f,
    -0.270893489485712f, -0.143440018674432f, -0.006321722748056f, 0.136464917303466f, 0.275746203041783f,
    0.400836183864430f, 0.504718430805013f, 0.586304870567852f, 0.647917106894812f, 0.690363360074246f,
    0.710097283399365f, 0.701066123462830f, 0.659656054475456f, 0.588356136700954f, 0.494867214955035f,
    0.387397428282516f, 0.270244048206542f, 0.143478482571876f, 0.006880008154635f, -0.135531696429985f,
    -0.274560466540054f, -0.399499198156804f, -0.503312017762724f, -0.584893358714116f, -0.646549400296632f,
    -0.689075026520866f, -0.708912586671874f, -0.699999944618863f, -0.658715664174368f, -0.587542794832356f,
    -0.494177585420843f, -0.386824805970815f, -0.269779402256090f, -0.143111325236285f, -0.006599102207918f,
    0.135737776490003f, 0.274702902995573f, 0.399588611405176f, 0.503358243888136f, 0.584905307474561f,
    0.646534978825119f, 0.689041115384931f, 0.708865056405437f, 0.699943702355506f, 0.658654721509000f,
    0.587480349472647f, 0.494116110527615f, 0.386766142306090f, 0.269724849346156f, 0.143061728709951f,
    0.006554935287805f, -0.135776338677538f, -0.274735917060236f, -0.399616307595869f, -0.503380976217918f,
    -0.584923511758358f, -0.646549138128803f, -0.689051732255678f, -0.708872631177018f, -0.699948716788277f,
    -0.658657627023664f, -0.587481559261370f, -0.494115994868835f,

};
/* array of output data to preload in float format */
static float output_data[iir_coeffa_size] = {
    0.000000000000000f, 0.000000000000000f,
};

/* array of calculated filtered data by mcu */
static float calculated_data[input_array_size - iir_coeffb_size + 1] = {0};
/* error between reference_data and calculated_data */
volatile float error[input_array_size - iir_coeffb_size + 1] = {0};
/*!
    \brief      enable the CPU Chache
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
    uint16_t i;
    /* enable the CPU cache */
    cache_enable();

    iir_config();

    /* get calculation result */
    for(i = 0; i < input_array_size - iir_coeffb_size + 1; i++) {
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
void iir_config(void)
{
    fac_parameter_struct facconfig;
    fac_float_data_preload_struct faccoeff;

    /* reset the FAC */
    fac_deinit();

    rcu_periph_clock_enable(RCU_FAC);

    /* initialize the FAC filter parameter struct */
    fac_struct_para_init(&facconfig);

    /* configure Coefficient buffer */
    facconfig.coeff_addr        = 0;
    facconfig.coeff_size        = iir_coeffa_size + iir_coeffb_size;

    /* configure input buffer */
    facconfig.input_addr        = iir_coeffa_size + iir_coeffb_size;
    facconfig.input_size        = iir_coeffb_size + iir_d0;
    facconfig.input_threshold   = FAC_THRESHOLD_1;

    /* configure input buffer */
    facconfig.output_addr       = iir_coeffa_size + iir_coeffb_size + iir_coeffb_size + iir_d0;
    facconfig.output_size       = iir_coeffa_size + iir_d1;
    facconfig.output_threshold  = FAC_THRESHOLD_1;
    facconfig.clip =  FAC_CP_ENABLE;
    fac_init(&facconfig);

    fac_float_enable();

    faccoeff.coeffa_ctx        = aFilterCoeffA_IIR;
    faccoeff.coeffa_size       = iir_coeffa_size;
    faccoeff.coeffb_ctx        = aFilterCoeffB_IIR;
    faccoeff.coeffb_size       = iir_coeffb_size;
    faccoeff.input_ctx         = aInputValues;
    faccoeff.input_size        = iir_coeffb_size + iir_d0;
    faccoeff.output_ctx        = output_data;
    faccoeff.output_size       = iir_coeffa_size;
    /* preload X0 X1 Y buffer with coefficient */
    fac_float_buffer_preload(&faccoeff);

    facconfig.func             = FUNC_IIR_DIRECT_FORM_1;
    facconfig.ipp              = iir_coeffb_size;
    facconfig.ipq              = iir_coeffa_size;
    facconfig.ipr              = iir_gain;
    fac_function_config(&facconfig);

    /* start filter calculation */
    fac_start();
}
