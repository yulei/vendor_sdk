/*!
    \file    gd32h73x_75x_it.c
    \brief   interrupt service routines

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

#include "gd32h73x_75x_it.h"
#include "main.h"
#include "systick.h"
#include <stdio.h>

/* read data point */
extern int16_t *pOutput;
extern uint16_t pOutputSize;
extern uint16_t curOutputCnt;

/* write data point */
extern int16_t *pInput;
extern uint16_t pInputSize;
extern uint16_t curInputCnt;

/* calculate success flag */
extern __IO uint8_t rd_complete;
extern __IO uint8_t wr_complete;

extern int16_t add_data[50];
extern int16_t calculated_data[80] ;

/*!
    \brief      this function handles NMI exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void NMI_Handler(void)
{
    /* if NMI exception occurs, go to infinite loop */
    while (1){
    }
}

/*!
    \brief      this function handles HardFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void HardFault_Handler(void)
{
    /* if Hard Fault exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles MemManage exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void MemManage_Handler(void)
{
    /* if Memory Manage exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles BusFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void BusFault_Handler(void)
{
    /* if Bus Fault exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles UsageFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void UsageFault_Handler(void)
{
    /* if Usage Fault exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles SVC exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SVC_Handler(void)
{
    /* if SVC exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles DebugMon exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DebugMon_Handler(void)
{
    /* if DebugMon exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles PendSV exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void PendSV_Handler(void)
{
    /* if PendSV exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles FPU exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void FPU_IRQHandler(void)
{
    /* if FPU exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles FAC interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void FAC_IRQHandler(void)
{
    int16_t tmpvalue;
    if(fac_interrupt_flag_get(FAC_INT_FLAG_YBEF) == SET) {
        if(pOutput == NULL) {
            /* the length of the data to be read is input_array_size +add_array_size - iir_coeffb_size +1 */
            pOutputSize = 80;
            /* pOutput pointer to calculated_data array*/
            pOutput = calculated_data;
        }
        /* get calculation result */
        do {
            /* reads the data and passes it to the calculated_data array */
            tmpvalue = (fac_fixed_data_read());
            *pOutput = (int16_t)tmpvalue;
            pOutput++;
            curOutputCnt++;
            /* the data has been fully read */
            if(curOutputCnt == pOutputSize) {
                rd_complete = 1;
                pOutput = NULL;
                fac_interrupt_disable(FAC_CTL_RIE);
                break;
            }
        } while(fac_flag_get(FAC_FLAG_YBEF) == RESET);
    }

    if(fac_interrupt_flag_get(FAC_INT_FLAG_X0BFF)  == SET) {
        if(pInput == NULL) {
            /* the length of the data to be write is add_array_size */
            pInputSize = 50;
            /* pInput pointer to add_data array*/
            pInput = add_data;
        } else {
            do {
                /* write add_data array data to the FAC_WDATA register */
                FAC_WDATA = ((uint32_t)(*pInput) & FAC_WDATA_WDATA);
                pInput++;
                curInputCnt++;
                /* the data has been fully writed */
                if(curInputCnt == pInputSize) {
                    wr_complete = 1;
                    pInput = NULL;
                    fac_interrupt_disable(FAC_CTL_WIE);
                    break;
                }
            } while(fac_flag_get(FAC_FLAG_X0BFF) == RESET);
        }
    }
}
