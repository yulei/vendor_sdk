/*!
    \file    i2c_ie.c
    \brief   I2C interrupt program

    \version 2023-12-31, V1.4.0, firmware for GD32E50x
*/

/*
    Copyright (c) 2023, GigaDevice Semiconductor Inc.

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

#include "i2c_ie.h"
#include "i2c.h"

/*!
    \brief      handle I2CX event interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void i2c_event_irq_handler(void)
{
    if(i2c2_interrupt_flag_get(I2CX, I2C2_INT_FLAG_STPDET)) {
        i2c2_interrupt_flag_clear(I2CX, I2C2_INT_FLAG_STPDET);
    } else if((i2c2_interrupt_flag_get(I2CX, I2C2_INT_FLAG_RBNE)) && (rx_nBytes)) {
        /* read a data byte from I2C2_RDATA*/
        *prbuffer++ = i2c_data_receive(I2CX);
        rx_nBytes--;
    } else if(i2c2_interrupt_flag_get(I2CX, I2C2_INT_FLAG_TI)) {
        if(1 == address_byte_flag) {
            /* send the EEPROM's internal address to write to */
            i2c_data_transmit(I2CX, gaddress);
        }
        if(tx_nBytes > 0) {
            /* send a data byte */
            i2c_data_transmit(I2CX, *pwbuffer++);
            tx_nBytes--;
        }
    } else if(i2c2_interrupt_flag_get(I2CX, I2C2_INT_FLAG_TCR)) {
        if(nbytes_read >= 255) {
            rx_nBytes = 255;
            nbytes_read = nbytes_read - 255;
        } else {
            rx_nBytes = nbytes_read;
            nbytes_read = 0;
        }
        i2c_transfer_byte_number_config(I2CX, rx_nBytes);
        /* disable I2C reload mode */
        if(0 == nbytes_read) {
            i2c_reload_disable(I2CX);
        }
    } else if(i2c2_interrupt_flag_get(I2CX, I2C2_INT_FLAG_TC)) {
        if(1 == address_byte_flag) {
            i2c2_master_addressing(I2CX, eeprom_address, I2C2_MASTER_RECEIVE);
            i2c_reload_enable(I2CX);
            /* configure number of bytes to be transferred */
            if(nbytes_read >= 255) {
                rx_nBytes = 255;
                nbytes_read = nbytes_read - 255;
            } else {
                rx_nBytes = nbytes_read;
                nbytes_read = 0;
            }
            i2c_transfer_byte_number_config(I2CX, rx_nBytes);
            /* disable I2C reload mode */
            if(0 == nbytes_read) {
                i2c_reload_disable(I2CX);
            }
            address_byte_flag = 0;
            i2c_start_on_bus(I2CX);
        } else {
            i2c_stop_on_bus(I2CX);
            status = SUCCESS;
        }
    }
}

/*!
    \brief      handle I2CX error interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void i2c_error_irq_handler(void)
{
    /* bus error */
    if(i2c2_interrupt_flag_get(I2CX, I2C2_INT_FLAG_BERR)) {
        i2c2_interrupt_flag_clear(I2CX, I2C2_INT_FLAG_BERR);
    }

    /* arbitration lost */
    if(i2c2_interrupt_flag_get(I2CX, I2C2_INT_FLAG_LOSTARB)) {
        i2c2_interrupt_flag_clear(I2CX, I2C2_INT_FLAG_LOSTARB);
    }

    /* over-run or under-run when SCL stretch is disabled */
    if(i2c2_interrupt_flag_get(I2CX, I2C2_INT_FLAG_OUERR)) {
        i2c2_interrupt_flag_clear(I2CX, I2C2_INT_FLAG_OUERR);
    }

    /* PEC error */
    if(i2c2_interrupt_flag_get(I2CX, I2C2_INT_FLAG_PECERR)) {
        i2c2_interrupt_flag_clear(I2CX, I2C2_INT_FLAG_PECERR);
    }

    /* timeout error */
    if(i2c2_interrupt_flag_get(I2CX, I2C2_INT_FLAG_TIMEOUT)) {
        i2c2_interrupt_flag_clear(I2CX, I2C2_INT_FLAG_TIMEOUT);
    }

    /* SMBus alert */
    if(i2c2_interrupt_flag_get(I2CX, I2C2_INT_FLAG_SMBALT)) {
        i2c2_interrupt_flag_clear(I2CX, I2C2_INT_FLAG_SMBALT);
    }

    /* disable the I2CX interrupt */
    i2c2_interrupt_disable(I2CX, I2C2_INT_ERR | I2C2_INT_STPDET | I2C2_INT_RBNE | I2C2_INT_TC | I2C2_INT_TI);
}
