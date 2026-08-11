/*!
    \file    usbd_conf.h
    \brief   the header file of USB device configuration

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

#ifndef USBD_CONF_H
#define USBD_CONF_H

#include "usb_conf.h"

#define USBD_CFG_MAX_NUM                       1U
#define USBD_ITF_MAX_NUM                       1U
#define USB_STR_DESC_MAX_SIZE                  64U

#define USBD_IAP_INTERFACE                     0U

/* USB user string supported */
/* #define USB_SUPPORT_USER_STRING_DESC */

//#define USBD_DYNAMIC_DESCRIPTOR_CHANGE_ENABLED

#define USB_STRING_COUNT                       4U

#define IAP_IN_EP                              EP1_IN
#define IAP_OUT_EP                             EP1_OUT

#ifdef USE_USB_HS
    #define IAP_IN_PACKET                      1018U
    #define IAP_OUT_PACKET                     1018U
    #define TRANSFER_SIZE                      1012U  /* IAP maximum data packet size */
#elif defined(USE_USB_FS)
    #define IAP_IN_PACKET                      62U
    #define IAP_OUT_PACKET                     62U
    #define TRANSFER_SIZE                      56U
#else
    #error "Please select USE_USB_HS or USE_USB_FS"
#endif

/* maximum number of supported memory media (Flash, RAM or EEPROM and so on) */
#define MAX_USED_MEMORY_MEDIA                  1U

/* MCU page size */

#define PAGE_SIZE                              4096U

#define OPT_BYTE_ADDR                          0x5200201CU
#define OPT_BYTE_ADDR2                         0x52002054U
#define AES_IV_ADDR                            0x52002068U
#define EFUSE_OB_ADDR                          0x40022814U

#define OPT_BYTE_SIZE                          60
#define AES_IV_SIZE                            24
#define EFUSE_OB_SIZE                          48

#define REPORT_IN_COUNT                        ((TRANSFER_SIZE) + 5U)
#define REPORT_OUT_COUNT                       ((TRANSFER_SIZE) + 5U)

/* memory address from where user application will be loaded, which represents 
   the DFU code protected against write and erase operations.*/
#define APP_LOADED_ADDR                        0x08008000U

/* make sure the corresponding memory where the DFU code should not be loaded
   cannot be erased or overwritten by DFU application. */
#define IS_PROTECTED_AREA(addr)                (uint8_t)(((addr >= 0x08000000U) && \
                                               (addr < (APP_LOADED_ADDR)))? 1U : 0U)

#endif /* USBD_CONF_H */
