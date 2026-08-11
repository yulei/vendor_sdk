/*!
    \file    main.c
    \brief   calculate sin with zero overhead example

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
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "gd32h759i_eval.h"

/* the value of pi */
#define DEMO_PI             (3.14159265f)

/* the first input data: angle */
float theta = DEMO_PI / 2.0f;
/* the second inputdata: modulus */
float m = 100.0f;
/* the calculation results: result=m*sin(theta)*/
float result = 0;

/* TMU input data in q31 */
uint32_t in_data[2] = {0};
/* TMU output data in q31 */
uint32_t out_data[2] = {0};

/* function declaration */
/* enable the CPU cache */
void cache_enable(void);
/* configure RCU */
void rcu_config(void);
/* configure TMU in q31 */
void tmu_config_q31(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/

int main(void)
{
    uint16_t scaling_factor = 128U;

    /* enable the CPU cache */
    cache_enable();
    /* configure RCU */
    rcu_config();
    /* configure systick */
    systick_config();
    /* configure COM port */
    gd_eval_com_init(EVAL_COM);

    /* software processes the input data */
    in_data[0] = (uint32_t)((int32_t)(theta / DEMO_PI * 0x80000000U));
    in_data[1] = (uint32_t)((int32_t)(m / scaling_factor * 0x80000000U));

    /* confiugre TMU and DMA in q31 */
    tmu_config_q31();

    /* write data to start TMU */
    tmu_two_q31_write(in_data[0], in_data[1]);

    /* read two output data */
    tmu_two_q31_read(&out_data[0], &out_data[1]);

    /* software processes the output data */
    result = scaling_factor * ((float)(int32_t)out_data[0]) / 0x80000000U;

    while(1) {
        delay_1ms(100);
        printf("\n%3.2f*sin(%3.2f))=%3.2f\n", m, theta, result);
    }
}

/*!
    \brief      enable the CPU cache
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
    \brief      configure RCU
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    rcu_periph_clock_enable(RCU_TMU);
}

/*!
    \brief      configure TMU
    \param[in]  none
    \param[out] none
    \retval     none
*/
void tmu_config_q31(void)
{
    tmu_parameter_struct tmu_init_struct;

    tmu_struct_para_init(&tmu_init_struct);
    /* reset the TMU */
    tmu_deinit();
    /* configure TMU peripheral */
    tmu_init_struct.mode = TMU_MODE_COS;
    tmu_init_struct.scale = TMU_SCALING_FACTOR_1;
    tmu_init_struct.dma_read = TMU_READ_DMA_DISABLE;
    tmu_init_struct.dma_write = TMU_WRITE_DMA_DISABLE;
    tmu_init_struct.read_times = TMU_READ_TIMES_2;
    tmu_init_struct.write_times = TMU_WRITE_TIMES_2;
    tmu_init_struct.output_width = TMU_OUTPUT_WIDTH_32;
    tmu_init_struct.input_width = TMU_INPUT_WIDTH_32;
    tmu_init(&tmu_init_struct);
}

