/*!
    \file    usbd_conf.h
    \brief   USB device driver basic configuration

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

#ifndef __USBD_CONF_H
#define __USBD_CONF_H

#include "gd32e50x.h"
#include "gd32e503z_eval.h"

#define USBD_CFG_MAX_NUM                   1U
#define USBD_ITF_MAX_NUM                   2U

/* define if low power mode is enabled; it allows entering the device into DEEP_SLEEP mode
   following USB suspend event and wakes up after the USB wakeup event is received. */
//#define USB_DEVICE_LOW_PWR_MODE_SUPPORT

/* USB feature -- Self Powered */
/* #define USBD_SELF_POWERED */

#define CUSTOM_HID_INTERFACE               0x00
#define PRINTER_INTERFACE                  0x01

/* endpoint0, Rx/Tx buffers address offset */
#define EP0_RX_ADDR                        (0x40U)
#define EP0_TX_ADDR                        (0x80U)

/* HID data Tx/Rx buffer address offset */
#define HID_TX_ADDR                        0xC0U
#define HID_RX_ADDR                        0xD0U

/* class layer parameter used by the Custom HID device */
#define CUSTOMHID_IN_EP                    EP_IN(1)
#define CUSTOMHID_OUT_EP                   EP_OUT(1)

#define CUSTOMHID_IN_PACKET                2U
#define CUSTOMHID_OUT_PACKET               2U

/* endpoint count used by the printer device */
#define PRINTER_IN_EP                      EP_IN(2U)
#define PRINTER_OUT_EP                     EP_OUT(2U)

#define BULK_TX_ADDR                       0xE0U
#define BULK_RX_ADDR                       0x120U

#define PRINTER_IN_PACKET                  64
#define PRINTER_OUT_PACKET                 64

/* endpoint count used by the composite device */
#define EP_COUNT                           3U

#define USB_STRING_COUNT                   4U

/* base address offset of the allocation buffer, used for buffer descriptor table and packet memory */
#define BTABLE_OFFSET                      (0x0000U)

/* base address of the allocation buffer, used for buffer descriptor table and packet memory */
#define BUFFER_ADDRESS                     (0x0000U)
#define USB_PULLUP                         GPIOG
#define USB_PULLUP_PIN                     GPIO_PIN_8
#define RCU_AHBPeriph_GPIO_PULLUP          RCU_GPIOG

#endif /* __USBD_CONF_H */
