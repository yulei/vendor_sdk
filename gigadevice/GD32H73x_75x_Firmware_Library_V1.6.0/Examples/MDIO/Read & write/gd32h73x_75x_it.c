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

extern volatile uint32_t mdio_message, mdio_addr;
extern volatile uint16_t data_array[8];

/*!
    \brief      this function handles NMI exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void NMI_Handler(void)
{
    /* if NMI exception occurs, go to infinite loop */
    while(1) {
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
    \brief      this function handles MDIO exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void MDIO_IRQHandler(void)
{
    mdio_message = MDIO_STAT;

    /* an address frame */
    if(MDIO_FLAG_ADDRFRM == (mdio_message & MDIO_FLAG_ADDRFRM)) {
        /* read the received address */
        mdio_addr = mdio_address_receive();

        /* preload the data into MDIO_TDATA register in case the next frame is a read frame */
        if(mdio_addr < 8) {
            mdio_data_transmit(data_array[mdio_addr]);
        } else {
            mdio_data_transmit(0xAAAA);
        }
    }

    /* a write data frame */
    if(MDIO_FLAG_WRFRM == (mdio_message & MDIO_FLAG_WRFRM)) {
        /* write the received data to array */
        if(mdio_addr < 8) {
            data_array[mdio_addr] = mdio_data_receive();
        }
    }

    /* a post read increment address frame */
    if(MDIO_FLAG_RDINCFRM == (mdio_message & MDIO_FLAG_RDINCFRM)) {
        /* increment the address for the next frame */
        mdio_addr++;

        if(mdio_addr < 8) {
            /* preload the next address data into MDIO_TDATA register */
            mdio_data_transmit(data_array[mdio_addr]);
        } else {
            mdio_data_transmit(0xAAAA);
        }
    }

    /* a read data frame */
    if(MDIO_FLAG_RDFRM == (mdio_message & MDIO_FLAG_RDFRM)) {
        /* fill the MDIO_TDATA register to avoid the next read operation failure */
        mdio_data_transmit(data_array[mdio_addr]);
    }
}
