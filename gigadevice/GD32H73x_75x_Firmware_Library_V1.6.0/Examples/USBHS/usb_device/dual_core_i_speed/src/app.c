/*!
    \file    app.c
    \brief   main routine

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

#include "drv_usb_hw.h"
#include "cdc_acm_core.h"
#include "custom_hid_core.h"

usb_core_driver usbhs0_cdc_acm;
usb_core_driver usbhs1_custom_hid;

extern hid_fop_handler fop_handler;

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
    /** note:
      * if the USB DMA is enabled, the d-cache should be disabled!
      */
#ifndef USB_INTERNAL_DMA_ENABLED
    SCB_EnableDCache();
#endif /* USB_INTERNAL_DMA_ENABLED */
}

/*!
    \brief      main routine will construct a USB dual device
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    cache_enable();

#ifdef USE_ULPI_PHY
    usb_gpio_config();
#endif /* USE_ULPI_PHY */

    usb_rcu_config();

    usb_timer_init();

    custom_hid_itfop_register(&usbhs1_custom_hid, &fop_handler);

#ifdef USE_USBHS0

#ifdef USE_USB_FS
    usb_para_init(&usbhs0_cdc_acm, USBHS0, USB_SPEED_FULL);
#endif /* USE_USB_FS */

#ifdef USE_USB_HS
    usb_para_init(&usbhs0_cdc_acm, USBHS0, USB_SPEED_HIGH);
#endif /* USE_USB_HS */

#endif /* USE_USBHS0 */

#ifdef USE_USBHS1

#ifdef USE_USB_FS
    usb_para_init(&usbhs1_custom_hid, USBHS1, USB_SPEED_FULL);
#endif /* USE_USB_FS */

#ifdef USE_USB_HS
    usb_para_init(&usbhs1_custom_hid, USBHS1, USB_SPEED_HIGH);
#endif /* USE_USB_HS */

#endif /* USE_USBHS1 */

    usbd_init(&usbhs0_cdc_acm, &cdc_desc, &cdc_class);
    usbd_init(&usbhs1_custom_hid, &custom_hid_desc, &usbd_custom_hid_cb);

#ifdef USE_USB_HS
    #ifndef USE_ULPI_PHY
        pllusb_rcu_config(USBHS0);
        pllusb_rcu_config(USBHS1);
    #endif /* !USE_ULPI_PHY */
#endif /* USE_USB_HS */

    usb_intr_config();

    while(1) {
        if(USBD_CONFIGURED == usbhs0_cdc_acm.dev.cur_status) {
            if(0U == cdc_acm_check_ready(&usbhs0_cdc_acm)) {
                cdc_acm_data_receive(&usbhs0_cdc_acm);
            } else {
                cdc_acm_data_send(&usbhs0_cdc_acm);
            }
        }
    }
}
