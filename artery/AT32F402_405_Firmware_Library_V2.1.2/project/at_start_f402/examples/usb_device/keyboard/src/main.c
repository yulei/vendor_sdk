/**
  **************************************************************************
  * @file     main.c
  * @brief    main program
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) that is made available to
  * download from Artery official website is the copyrighted work of Artery.
  * Artery authorizes customers to use, copy, and distribute the BSP
  * software and its related documentation for the purpose of design and
  * development in conjunction with Artery microcontrollers. Use of the
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */

#include "at32f402_405_board.h"
#include "at32f402_405_clock.h"
#include "usb_conf.h"
#include "usb_core.h"
#include "usbd_int.h"
#include "keyboard_class.h"
#include "keyboard_desc.h"


/** @addtogroup AT32F402_periph_examples
  * @{
  */

/** @addtogroup 402_USB_device_keyboard USB_device_keyboard
  * @{
  */

/* usb global struct define */
otg_core_type otg_core_struct;
extern __IO uint8_t hid_suspend_flag;
void usb_clock48m_select(usb_clk48_s clk_s);
void keyboard_send_string(void *udev, uint8_t *string, uint8_t len);
void usb_gpio_config(void);
void usb_low_power_wakeup_config(void);
void system_clock_recover(void);
void button_exint_init(void);

/**
  * @brief  initialize the low power mode only for at32f402xx
  *         this function has a 10ms delay using an 8MHz system clock.
  *         in addition, to enter this low power mode, need to enable 
  *         the PLL or HEXT.
  * @param  none
  * @retval none
  */
void low_power_init(void)
{
#ifdef AT32F402xx 
  volatile uint32_t delay = 8000;
  if(CRM->ctrl_bit.hextstbl)
  {
    *(__IO uint32_t *)0x40023878 = 0x00;
  }
  else if(CRM->ctrl_bit.pllstbl == SET)
  {
    CRM->pllcfg_bit.plluen = TRUE;
    while(CRM->ctrl_bit.pllstbl != SET || CRM->ctrl_bit.pllustbl != SET);
    *(__IO uint32_t *)0x40023878 = 0x10;
  }
  else
  {
    /* the pll or hext need to be enable */
    return;
  }
  CRM->ahben1 |= 1 << 29;
  *(__IO uint32_t *)0x40040038 = 0x210000;
  *(__IO uint32_t *)0x4004000C |= 0x40000000;
  *(__IO uint32_t *)0x40040804 &= ~0x2;
  while(delay --)
  {
    if(*(__IO uint32_t *)0x40040808 & 0x1)
      break;      
  }
  *(__IO uint32_t *)0x40040038 |= 0x400000;
  *(__IO uint32_t *)0x40040E00 |= 0x1;  
  *(__IO uint32_t *)0x40040038 &= ~0x10000;
  *(__IO uint32_t *)0x40023878 = 0x0;
#endif
  return;
}

/**
  * @brief  keyboard send string
  * @param  string: send string
  * @param  len: send string length
  * @retval none
  */
void keyboard_send_string(void *udev, uint8_t *string, uint8_t len)
{
  uint8_t index = 0;
  usbd_core_type *pudev = (usbd_core_type *)udev;
  keyboard_type *pkeyboard = (keyboard_type *)pudev->class_handler->pdata;
  for(index = 0; index < len; index ++)
  {
    while(1)
    {
      if(pkeyboard->g_u8tx_completed == 1)
      {
        pkeyboard->g_u8tx_completed = 0;
        usb_hid_keyboard_send_char(udev, string[index]);
        break;
      }
    }
    /* send 0x00 */
    while(1)
    {
      if(pkeyboard->g_u8tx_completed == 1)
      {
        pkeyboard->g_u8tx_completed = 0;
        usb_hid_keyboard_send_char(udev, 0x00);
        break;
      }
    }
  }

}


/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  __IO uint32_t delay_index = 0;
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);

  system_clock_config();

  at32_board_init();

  /* usb gpio config */
  usb_gpio_config();

#ifdef USB_LOW_POWER_WAKUP
  /* enable pwc and bpr clock */
  crm_periph_clock_enable(CRM_PWC_PERIPH_CLOCK, TRUE);
  button_exint_init();
  usb_low_power_wakeup_config();
#endif

  /* enable otgfs clock */
  crm_periph_clock_enable(OTG_CLOCK, TRUE);

  /* select usb 48m clcok source */
  usb_clock48m_select(USB_CLK_HEXT);

  /* enable otgfs irq */
  nvic_irq_enable(OTG_IRQ, 0, 0);

  /* init usb */
  usbd_init(&otg_core_struct,
            USB_SPEED_CORE_ID,
            USB_ID,
            &keyboard_class_handler,
            &keyboard_desc_handler);
  at32_led_on(LED2);
  at32_led_on(LED3);
  at32_led_on(LED4);
  while(1)
  {
    if(at32_button_press() == USER_BUTTON)
    {
      if(usbd_connect_state_get(&otg_core_struct.dev) == USB_CONN_STATE_CONFIGURED)
      {
        keyboard_send_string(&otg_core_struct.dev, (uint8_t *)" Keyboard Demo\r\n", 16);
      }
      /* remote wakeup */
      if(usbd_connect_state_get(&otg_core_struct.dev) == USB_CONN_STATE_SUSPENDED
        && (otg_core_struct.dev.remote_wakup == 1))
      {
        usbd_remote_wakeup(&otg_core_struct.dev);
      }
    }

#ifdef USB_LOW_POWER_WAKUP
     /* enter deep sleep */
    if(((keyboard_type *)(otg_core_struct.dev.class_handler->pdata))->hid_suspend_flag == 1)
    {
      __disable_irq();
      
      if(OTG_PCGCCTL(otg_core_struct.usb_reg)->pcgcctl_bit.suspendm == 1
         && usb_suspend_status_get(otg_core_struct.usb_reg) == 1)
      {
        at32_led_off(LED2);
        at32_led_off(LED3);
        at32_led_off(LED4);
        
        /* select system clock source as hick before ldo set */
        crm_sysclk_switch(CRM_SCLK_HICK);

        /* wait till hick is used as system clock source */
        while(crm_sysclk_switch_status_get() != CRM_SCLK_HICK)
        {
        }
        
        /* initialize the low power mode only for at32f402xx
         this function has a 5ms delay using an 8MHz system clock.
         in addition, to enter this low power mode, need to enable 
         the PLL or HEXT.*/
        low_power_init();
        
        /* reduce ldo before enter deepsleep mode */
        pwc_ldo_output_voltage_set(PWC_LDO_OUTPUT_1V1);
        
        /* congfig the voltage regulator mode */
        pwc_voltage_regulate_set(PWC_REGULATOR_ON);

        /* enter deep sleep mode */
        pwc_deep_sleep_mode_enter(PWC_DEEP_SLEEP_ENTER_WFI);
        /* wait clock stable */
        delay_us(120);
        
      /* resume ldo before system clock source enhance */
        pwc_ldo_output_voltage_set(PWC_LDO_OUTPUT_1V3);
        
        system_clock_recover();
      }
      ((keyboard_type *)(otg_core_struct.dev.class_handler->pdata))->hid_suspend_flag = 0;
      __enable_irq();
      at32_led_on(LED2);
      at32_led_on(LED3);
      at32_led_on(LED4);
    }
 #endif

  }
}

/**
  * @brief  usb 48M clock select
  * @param  clk_s:USB_CLK_HICK, USB_CLK_HEXT
  * @retval none
  */
void usb_clock48m_select(usb_clk48_s clk_s)
{
  if(clk_s == USB_CLK_HICK)
  {
    crm_usb_clock_source_select(CRM_USB_CLOCK_SOURCE_HICK);

    /* enable the acc calibration ready interrupt */
    crm_periph_clock_enable(CRM_ACC_PERIPH_CLOCK, TRUE);

    /* update the c1\c2\c3 value */
    acc_write_c1(7980);
    acc_write_c2(8000);
    acc_write_c3(8020);

    /* open acc calibration */
    acc_calibration_mode_enable(ACC_CAL_HICKTRIM, TRUE);
  }
  else
  {
    crm_pllu_output_set(TRUE);
    /* wait till pll is ready */
    while(crm_flag_get(CRM_PLLU_STABLE_FLAG) != SET)
    {
    }
    crm_usb_clock_source_select(CRM_USB_CLOCK_SOURCE_PLLU);
  }
}

/**
  * @brief  this function config gpio.
  * @param  none
  * @retval none
  */
void usb_gpio_config(void)
{
  gpio_init_type gpio_init_struct;

  crm_periph_clock_enable(OTG_PIN_GPIO_CLOCK, TRUE);
  gpio_default_para_init(&gpio_init_struct);

  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;

#ifdef USB_SOF_OUTPUT_ENABLE
  crm_periph_clock_enable(OTG_PIN_SOF_GPIO_CLOCK, TRUE);
  gpio_init_struct.gpio_pins = OTG_PIN_SOF;
  gpio_init(OTG_PIN_SOF_GPIO, &gpio_init_struct);
  gpio_pin_mux_config(OTG_PIN_SOF_GPIO, OTG_PIN_SOF_SOURCE, OTG_PIN_MUX);
#endif

  /* otgfs use vbus pin */
#ifndef USB_VBUS_IGNORE
  gpio_init_struct.gpio_pins = OTG_PIN_VBUS;
  gpio_init_struct.gpio_pull = GPIO_PULL_DOWN;
  gpio_pin_mux_config(OTG_PIN_GPIO, OTG_PIN_VBUS_SOURCE, OTG_PIN_MUX);
  gpio_init(OTG_PIN_GPIO, &gpio_init_struct);
#endif


}
#ifdef USB_LOW_POWER_WAKUP
/**
  * @brief  usb low power wakeup interrupt config
  * @param  none
  * @retval none
  */
void usb_low_power_wakeup_config(void)
{
  exint_init_type exint_init_struct;

  crm_periph_clock_enable(CRM_SCFG_PERIPH_CLOCK, TRUE);
  exint_default_para_init(&exint_init_struct);

  exint_init_struct.line_enable = TRUE;
  exint_init_struct.line_mode = EXINT_LINE_INTERRUPT;
  exint_init_struct.line_select = OTG_WKUP_EXINT_LINE;
  exint_init_struct.line_polarity = EXINT_TRIGGER_RISING_EDGE;
  exint_init(&exint_init_struct);

  nvic_irq_enable(OTG_WKUP_IRQ, 0, 0);
}

/**
  * @brief  system clock recover.
  * @param  none
  * @retval none
  */
void system_clock_recover(void)
{
  /* enable external high-speed crystal oscillator - hext */
  crm_clock_source_enable(CRM_CLOCK_SOURCE_HEXT, TRUE);

  /* wait till hext is ready */
  while(crm_hext_stable_wait() == ERROR);

  /* config pllu div */
  crm_pllu_div_set(CRM_PLL_FU_18);

  /* enable pll */
  crm_clock_source_enable(CRM_CLOCK_SOURCE_PLL, TRUE);

  /* wait till pll is ready */
  while(crm_flag_get(CRM_PLL_STABLE_FLAG) == RESET);

  /* enable auto step mode */
  crm_auto_step_mode_enable(TRUE);

  /* select pll as system clock source */
  crm_sysclk_switch(CRM_SCLK_PLL);

  /* wait till pll is used as system clock source */
  while(crm_sysclk_switch_status_get() != CRM_SCLK_PLL);
}

/**
  * @brief  configure button exint
  * @param  none
  * @retval none
  */
void button_exint_init(void)
{
  exint_init_type exint_init_struct;

  crm_periph_clock_enable(CRM_SCFG_PERIPH_CLOCK, TRUE);
  scfg_exint_line_config(SCFG_PORT_SOURCE_GPIOA, SCFG_PINS_SOURCE0);

  exint_default_para_init(&exint_init_struct);
  exint_init_struct.line_enable = TRUE;
  exint_init_struct.line_mode = EXINT_LINE_INTERRUPT;
  exint_init_struct.line_select = EXINT_LINE_0;
  exint_init_struct.line_polarity = EXINT_TRIGGER_RISING_EDGE;
  exint_init(&exint_init_struct);

  nvic_irq_enable(EXINT0_IRQn, 0, 0);
}

/**
  * @brief  this function handles otgfs wakup interrupt.
  * @param  none
  * @retval none
  */
void OTG_WKUP_HANDLER(void)
{
  exint_flag_clear(OTG_WKUP_EXINT_LINE);
}

/**
  * @brief  exint0 interrupt handler
  * @param  none
  * @retval none
  */
void EXINT0_IRQHandler(void)
{
  exint_flag_clear(EXINT_LINE_0);
}


#endif

/**
  * @brief  this function handles otgfs interrupt.
  * @param  none
  * @retval none
  */
void OTG_IRQ_HANDLER(void)
{
  usbd_irq_handler(&otg_core_struct);
}

/**
  * @brief  usb delay millisecond function.
  * @param  ms: number of millisecond delay
  * @retval none
  */
void usb_delay_ms(uint32_t ms)
{
  /* user can define self delay function */
  delay_ms(ms);
}

/**
  * @brief  usb delay microsecond function.
  * @param  us: number of microsecond delay
  * @retval none
  */
void usb_delay_us(uint32_t us)
{
  delay_us(us);
}

/**
  * @}
  */

/**
  * @}
  */
