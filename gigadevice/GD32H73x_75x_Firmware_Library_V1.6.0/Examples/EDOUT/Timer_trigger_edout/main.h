/*!
    \file    main.h
    \brief   the header file of main

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

#ifndef MAIN_H
#define MAIN_H

#include <math.h>
#include "gd32h73x_75x.h"
#include "gd32h759i_eval.h"

#define EDOUT_MAX_LOC                  99
#define EDOUT_B_PHASE_POL              EDOUT_POL_POSITIVE
#define EDOUT_Z_OUTPUT_MOD             EDOUT_Z_OUTPUT_MODE0
#define EDOUT_Z_OUTPUT_START_LOC       92
#define EDOUT_Z_OUTPUT_PULSE_WIDTH     2 /* Z-phase output pulse width is 2 edges */

#define DATA_NUM                       5       /* the size of the encoder data array */
#define TIMER_PERIOD_NS                500000  /* TIMER period, in units of ns */
#define ENCODER_RESOLUTION             1048576 /* the resolution of the encoder location value is 20 bits */

#define ABS(x)                         (((x) < 0) ? -(x) : (x))

/* EDOUT parameter structure definitions */
typedef struct {
    uint32_t pol;                      /* active polarity of the B-phase output */
    uint32_t max_loc;                  /* maximum location value for one rotation */
    int16_t  num_edges;                /* number of edges of the A-phase and the B-phase signal for the next update period */
    uint16_t phase_diff;               /* phase difference between the A-phase and the B-phase signal for the next update period */
    uint32_t cur_loc;                  /* current location value */
    uint32_t z_output_start_loc;       /* Z-phase output start location */
    uint32_t z_output_width;           /* Z-phase output width */
    uint32_t z_output_mode;            /* Z-phase output mode*/
} edout_para_struct;

extern edout_para_struct edout_para;
extern __IO uint32_t encoder_data_idx;
extern uint32_t encoder_data[DATA_NUM];

/* function declarations */
/* enable the CPU Cache */
void cache_enable(void);
/* configure interrupt priority */
void nvic_config(void);
/* configure the GPIO ports */
void gpio_config(void);
/* configure TIMER1_TRGO to trigger EDOUT */
void trigsel_config(void);
/* configure the trigger source TIMER */
void timer_config(void);
/* configure EDOUT */
void edout_config(void);
/* calculate the phase difference and the number of edges of the output counter */
void edout_calc_output_counter(edout_para_struct *para, uint32_t encoder_cnt, uint32_t event_period);

#endif /* MAIN_H */
