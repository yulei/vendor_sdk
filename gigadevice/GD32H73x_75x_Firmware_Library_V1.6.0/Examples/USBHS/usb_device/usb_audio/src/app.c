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
#include "audio_core.h"
#include "audio_out_itf.h"

#ifdef USE_USB_AD_MICPHONE
    #include "wave_data.h"
#endif

usb_core_driver usb_audio;

#ifdef USE_USB_AD_MICPHONE
    volatile uint32_t count_data = 0;
#endif

/*!
    \brief      main routine will construct a USB audio device
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
#ifdef USE_ULPI_PHY
    usb_gpio_config();
#endif /* USE_ULPI_PHY */

    usb_rcu_config();

    usb_timer_init();

#ifdef USE_USBHS0

#ifdef USE_USB_FS
    usb_para_init(&usb_audio, USBHS0, USB_SPEED_FULL);
#endif

#ifdef USE_USB_HS
    usb_para_init(&usb_audio, USBHS0, USB_SPEED_HIGH);
#endif

#endif /* USE_USBHS0 */

#ifdef USE_USBHS1

#ifdef USE_USB_FS
    usb_para_init(&usb_audio, USBHS1, USB_SPEED_FULL);
#endif

#ifdef USE_USB_HS
    usb_para_init(&usb_audio, USBHS1, USB_SPEED_HIGH);
#endif

#endif /* USE_USBHS1 */

    usbd_init(&usb_audio, &audio_desc, &usbd_audio_cb);

#ifdef USE_USB_HS
    #ifndef USE_ULPI_PHY
        #ifdef USE_USBHS0
            pllusb_rcu_config(USBHS0);
        #elif defined USE_USBHS1
            pllusb_rcu_config(USBHS1);
        #else
        #endif
    #endif /* !USE_ULPI_PHY */
#endif /* USE_USB_HS */

    usb_intr_config();

    /* check if USB device is enumerated successfully */
    while(USBD_CONFIGURED != usb_audio.dev.cur_status) {
    }

#ifdef USE_USB_AD_MICPHONE
    for(__IO uint32_t i = 0U; i < 2000U; i++) {
        for(__IO uint32_t j = 0; j < 10000U; j++);
    }

    usbd_ep_send(&usb_audio, AD_IN_EP, (uint8_t*)wavetestdata, MIC_IN_PACKET);
    count_data = MIC_IN_PACKET;
#endif /* USE_USB_AD_MICPHONE */

    while (1) {
    }
}
