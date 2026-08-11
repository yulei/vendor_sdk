/*!
    \file    gd32h73x_75x_usb_test.h
    \brief   definitions for leds, keys and COM ports hardware resources
    
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

#ifndef GD32H73X_75X_USB_TEST_H
#define GD32H73X_75X_USB_TEST_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "gd32h73x_75x.h"

/* exported types */
typedef enum 
{
    LED2 = 0,
    LED3 = 1,
    LED4 = 2,
    LED5 = 3,
}led_typedef_enum;

typedef enum 
{
    KEY2 = 0,
    KEY3 = 1,
    KEY4 = 2,
    KEY5 = 3,
}key_typedef_enum;

typedef enum 
{
    KEY_MODE_GPIO = 0,
    KEY_MODE_EXTI = 1
}keymode_typedef_enum;

/* start board low layer led */
#define LEDn                             4U

#define LED2_PIN                         GPIO_PIN_1
#define LED2_GPIO_PORT                   GPIOE
#define LED2_GPIO_CLK                    RCU_GPIOE

#define LED3_PIN                         GPIO_PIN_2
#define LED3_GPIO_PORT                   GPIOE
#define LED3_GPIO_CLK                    RCU_GPIOE

#define LED4_PIN                         GPIO_PIN_3
#define LED4_GPIO_PORT                   GPIOE
#define LED4_GPIO_CLK                    RCU_GPIOE

#define LED5_PIN                         GPIO_PIN_4
#define LED5_GPIO_PORT                   GPIOE
#define LED5_GPIO_CLK                    RCU_GPIOE

/* start board low layer button */  
#define KEYn                             4U

/* K2 push-button */
#define KEY2_PIN                         GPIO_PIN_9
#define KEY2_GPIO_PORT                   GPIOG
#define KEY2_GPIO_CLK                    RCU_GPIOG
#define KEY2_EXTI_LINE                   EXTI_9
#define KEY2_EXTI_PORT_SOURCE            EXTI_SOURCE_GPIOG
#define KEY2_EXTI_PIN_SOURCE             EXTI_SOURCE_PIN9
#define KEY2_EXTI_IRQn                   EXTI5_9_IRQn

/* K3 push-button */
#define KEY3_PIN                         GPIO_PIN_10
#define KEY3_GPIO_PORT                   GPIOG
#define KEY3_GPIO_CLK                    RCU_GPIOG
#define KEY3_EXTI_LINE                   EXTI_10
#define KEY3_EXTI_PORT_SOURCE            EXTI_SOURCE_GPIOG
#define KEY3_EXTI_PIN_SOURCE             EXTI_SOURCE_PIN10
#define KEY3_EXTI_IRQn                   EXTI10_15_IRQn

/* K4 push-button */
#define KEY4_PIN                         GPIO_PIN_11
#define KEY4_GPIO_PORT                   GPIOG
#define KEY4_GPIO_CLK                    RCU_GPIOG
#define KEY4_EXTI_LINE                   EXTI_11
#define KEY4_EXTI_PORT_SOURCE            EXTI_SOURCE_GPIOG
#define KEY4_EXTI_PIN_SOURCE             EXTI_SOURCE_PIN11
#define KEY4_EXTI_IRQn                   EXTI10_15_IRQn

/* K5 push-button */
#define KEY5_PIN                         GPIO_PIN_13
#define KEY5_GPIO_PORT                   GPIOG
#define KEY5_GPIO_CLK                    RCU_GPIOG
#define KEY5_EXTI_LINE                   EXTI_13
#define KEY5_EXTI_PORT_SOURCE            EXTI_SOURCE_GPIOG
#define KEY5_EXTI_PIN_SOURCE             EXTI_SOURCE_PIN13
#define KEY5_EXTI_IRQn                   EXTI10_15_IRQn

/* eval board low layer COM */
#define COMn                             1U

/* definition for COM, connected to USART0 */
#define EVAL_COM                         USART1
#define EVAL_COM_CLK                     RCU_USART1

#define EVAL_COM_TX_PIN                  GPIO_PIN_5
#define EVAL_COM_RX_PIN                  GPIO_PIN_6

#define EVAL_COM_GPIO_PORT               GPIOD
#define EVAL_COM_GPIO_CLK                RCU_GPIOD
#define EVAL_COM_AF                      GPIO_AF_7

/* function declarations */
/* configure led GPIO */
void gd_eval_led_init(led_typedef_enum lednum);
/* turn on selected led */
void gd_eval_led_on(led_typedef_enum lednum);
/* turn off selected led */
void gd_eval_led_off(led_typedef_enum lednum);
/* toggle the selected led */
void gd_eval_led_toggle(led_typedef_enum lednum);
/* configure key */
void gd_eval_key_init(key_typedef_enum keynum, keymode_typedef_enum keymode);
/* return the selected key state */
uint8_t gd_eval_key_state_get(key_typedef_enum keynum);
/* configure COM port */
void gd_eval_com_init(uint32_t com);

#ifdef __cplusplus
}
#endif

#endif /* GD32H73X_75X_USB_TEST_H */
