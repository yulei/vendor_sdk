/*!
    \file    gd32e50x_it.c
    \brief   main interrupt service routines

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

#include "gd32e50x_it.h"
#include "drv_usbd_int.h"
#include "usb_conf.h"
#include "custom_hid_core.h"

uint8_t send_buffer1[4] = {0x00, 0x01, 0x00, 0x00};
uint8_t send_buffer2[4] = {0x00, 0x01, 0x00, 0x00};

extern usb_core_driver custom_hid;
extern uint32_t usb_prescaler;

void usb_timer_irq (void);

/* local function prototypes ('static') */
static void resume_mcu_clk(void);

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
    \brief      this function handles SysTick exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SysTick_Handler(void)
{
}

/*!
    \brief      this function handles USBHS interrupt
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBHS_IRQHandler (void)
{
    usbd_isr (&custom_hid);
}

/*!
    \brief      this function handles USBHS wakeup interrupt request.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBHS_WKUP_IRQHandler(void)
{
    if (custom_hid.bp.low_power) {
        resume_mcu_clk();

        rcu_usb_clock_config(usb_prescaler);

        rcu_periph_clock_enable(RCU_USBHS);

        usb_clock_active(&custom_hid);
    }

    exti_interrupt_flag_clear(EXTI_18);
}

#ifdef USB_DEDICATED_EP1_ENABLED

/*!
    \brief      this function handles USB dedicated endpoint 1 OUT interrupt request.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBHS_EP1_OUT_IRQHandler (void)
{
    usbd_int_dedicated_ep1out(&custom_hid);
}

/*!
    \brief      this function handles USB dedicated endpoint 1 IN interrupt request.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBHS_EP1_IN_IRQHandler (void)
{
    usbd_int_dedicated_ep1in(&custom_hid);
}

#endif /* USB_DEDICATED_EP1_ENABLED */

/*!
    \brief      this function handles Timer2 update interrupt request.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void TIMER2_IRQHandler(void)
{
    usb_timer_irq();
}

/*!
    \brief      this function handles EXTI0_IRQ Handler.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI0_IRQHandler (void)
{
    if (RESET != exti_interrupt_flag_get(KEY_A_EXTI_LINE)) {
        if (USBD_CONFIGURED == custom_hid.dev.cur_status) {
            send_buffer1[0] = 0x15U;

            if (RESET == gd_eval_key_state_get(KEY_A)) {
                if(send_buffer1[1]) {
                    send_buffer1[1] = 0x00U;
                } else {
                    send_buffer1[1] = 0x01U;
                }
            }
            
            custom_hid_report_send (&custom_hid, send_buffer1, 2U);
        }

        /* clear the EXTI line interrupt flag */
        exti_interrupt_flag_clear(KEY_A_EXTI_LINE);
    }
}

/*!
    \brief      this function handles EXTI10_15_IRQ Handler.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void  EXTI10_15_IRQHandler (void)
{
    if (RESET != exti_interrupt_flag_get(KEY_B_EXTI_LINE)) {
        if (USBD_CONFIGURED == custom_hid.dev.cur_status) {
            send_buffer2[0] = 0x16U;

            if (RESET == gd_eval_key_state_get(KEY_B)) {
                if(send_buffer2[1]) {
                    send_buffer2[1] = 0x00U;
                } else {
                    send_buffer2[1] = 0x01U;
                }
            }
            
            custom_hid_report_send (&custom_hid, send_buffer2, 2U);
        }

        /* clear the EXTI line interrupt flag */
        exti_interrupt_flag_clear(KEY_B_EXTI_LINE);
    }
}

/*!
    \brief      resume MCU clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void resume_mcu_clk(void)
{
    /* enable HXTAL */
    rcu_osci_on(RCU_HXTAL);

    /* wait till HXTAL is ready */
    while(RESET == rcu_flag_get(RCU_FLAG_HXTALSTB)){
    }

    /* enable PLL1 */
    rcu_osci_on(RCU_PLL1_CK);

    /* wait till PLL1 is ready */
    while(RESET == rcu_flag_get(RCU_FLAG_PLL1STB)){
    }

    /* enable PLL */
    rcu_osci_on(RCU_PLL_CK);

    /* wait till PLL is ready */
    while(RESET == rcu_flag_get(RCU_FLAG_PLLSTB)){
    }

    /* select PLL as system clock source */
    rcu_system_clock_source_config(RCU_CKSYSSRC_PLL);

    /* wait till PLL is used as system clock source */
    while(RCU_SCSS_PLL != rcu_system_clock_source_get()){
    }
}
