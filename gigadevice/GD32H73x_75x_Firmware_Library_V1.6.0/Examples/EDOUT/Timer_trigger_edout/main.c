/*!
    \file    main.c
    \brief   timer trigger EDOUT example

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

#include "main.h"

edout_para_struct edout_para;

__IO uint32_t encoder_data_idx = 1;
uint32_t encoder_data[DATA_NUM] = {948862, 997245, 21810, 1028023, 1033476};

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* enable the CPU Cache */
    cache_enable();
    /* configure nvic */
    nvic_config();
    /* configure gpio */
    gpio_config();
    /* configure trigsel */
    trigsel_config();
    /* configure timer */
    timer_config();
    /* configure EDOUT */
    edout_config();

    /* start TIMER1 to trigger EDOUT */
    timer_enable(TIMER1);
    /* calculate the output counter */
    edout_calc_output_counter(&edout_para, encoder_data[encoder_data_idx++], TIMER_PERIOD_NS);
    /* update the phase difference and the number of edges for the next update period  */
    edout_output_counter_update(edout_para.num_edges, edout_para.phase_diff);

    while(1) {
    }
}

/*!
    \brief      enable the CPU Cache
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cache_enable(void)
{
    /* enable I-Cache */
    SCB_EnableICache();

    /* enable D-Cache */
    SCB_EnableDCache();
}

/*!
    \brief      configure interrupt priority
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_config(void)
{
    /* 2 bits for preemption priority, 2 bits for sub-priority */
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);

    /* enable the TIMER1 interrupt */
    nvic_irq_enable(TIMER1_IRQn, 1, 0);
}

/*!
    \brief      configure the GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
  */
void gpio_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOD);

    /* configure PD12(EDOUT Phase-A) as alternate function */
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_12);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_12);
    gpio_af_set(GPIOD, GPIO_AF_6, GPIO_PIN_12);

    /* configure PD13(EDOUT Phase-B) as alternate function */
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_13);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_13);
    gpio_af_set(GPIOD, GPIO_AF_6, GPIO_PIN_13);

    /* configure PD14(EDOUT Phase-Z) as alternate function */
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_14);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_14);
    gpio_af_set(GPIOD, GPIO_AF_6, GPIO_PIN_14);
}

/*!
    \brief      configure TIMER1_TRGO to trigger EDOUT
    \param[in]  none
    \param[out] none
    \retval     none
*/
void trigsel_config(void)
{
    rcu_periph_clock_enable(RCU_TRIGSEL);

    /* reset TRIGSEL */
    trigsel_deinit();
    /* initialize TRIGSEL */
    trigsel_init(TRIGSEL_OUTPUT_EDOUT_TRG, TRIGSEL_INPUT_TIMER1_TRGO0);
}

/*!
    \brief      configure the trigger source TIMER
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer_config(void)
{
    /* TIMER configuration */
    timer_parameter_struct timer_initpara;
    timer_struct_para_init(&timer_initpara);

    rcu_periph_clock_enable(RCU_TIMER1);
    timer_deinit(TIMER1);

    /* the TIMER period is TIMER_PERIOD_NS ns */
    timer_initpara.prescaler         = 299;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = TIMER_PERIOD_NS / 1000 - 1;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER1, &timer_initpara);

    timer_master_output0_trigger_source_select(TIMER1, TIMER_TRI_OUT0_SRC_UPDATE);

    /* clear update interrupt flag */
    timer_interrupt_flag_clear(TIMER1, TIMER_INT_FLAG_UP);
    /* enable update interrupt */
    timer_interrupt_enable(TIMER1, TIMER_INT_UP);
}

/*!
    \brief      configure EDOUT
    \param[in]  none
    \param[out] none
    \retval     none
*/
void edout_config(void)
{
    uint32_t location;
    uint32_t edge_number = EDOUT_MAX_LOC + 1;

    rcu_periph_clock_enable(RCU_EDOUT);

    /* reset EDOUT */
    edout_deinit();

    /* calculate the current location */
    location = (uint32_t)(((uint64_t)encoder_data[0] * (uint64_t)edge_number) / ENCODER_RESOLUTION);

    /* initialize parameter */
    edout_para.pol                = EDOUT_B_PHASE_POL;
    edout_para.max_loc            = EDOUT_MAX_LOC;
    edout_para.cur_loc            = location;
    edout_para.z_output_mode      = EDOUT_Z_OUTPUT_MOD;
    edout_para.z_output_start_loc = EDOUT_Z_OUTPUT_START_LOC;
    edout_para.z_output_width     = EDOUT_Z_OUTPUT_PULSE_WIDTH;

    /* initialize EDOUT */
    edout_init(edout_para.pol, edout_para.max_loc, edout_para.cur_loc);

    /* enable EDOUT */
    edout_enable();

    /* configure Z-phase output */
    edout_z_output_mode_config(edout_para.z_output_mode);
    edout_z_output_start_loc_and_width_config(edout_para.z_output_start_loc, edout_para.z_output_width);
}

/*!
    \brief      calculate the phase difference and the number of edges of the output counter
    \param[in]  para: the EDOUT parameter data
    \param[in]  encoder_cnt: encoder location count
    \param[in]  event_period: the period which the TRIGSEL sends the event signal, in unit of ns
    \param[out] none
    \retval     none
*/
void edout_calc_output_counter(edout_para_struct *para, uint32_t encoder_cnt, uint32_t event_period)
{
    int32_t count;
    uint32_t pre_loc = para->cur_loc;
    uint32_t edge_number = EDOUT_MAX_LOC + 1;
    uint32_t pclk = rcu_clock_freq_get(CK_APB2);

    /* calculate the current location */
    para->cur_loc = (uint32_t)(((uint64_t)encoder_cnt * (uint64_t)edge_number) / ENCODER_RESOLUTION);

    /* calculate location difference */
    count = para->cur_loc - pre_loc;

    /* calculate the number of edges */
    if((edge_number / 2) < ABS(count)) {
        count = (count > 0) ? (count - edge_number) : (count + edge_number);
    }

    para->num_edges = (int16_t)count;

    /* calculate the phase difference */
    if(0 != count) {
        para->phase_diff = (uint16_t)(((uint64_t)event_period * (uint64_t)pclk) / pow(10, 9) / ABS(count));
    } else {
        para->phase_diff = 0xFFFF;
    }
}
