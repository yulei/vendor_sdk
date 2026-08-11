/*!
    \file    main.c
    \brief   MDIO read & write

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

#include "gd32h73x_75x.h"
#include "gd32h759i_eval.h"

void cache_enable(void);
void rcu_config(void);
void nvic_config(void);
void gpio_config(void);
void mdio_config(void);

volatile uint16_t data_array[8];
volatile uint32_t mdio_message = 0;
volatile uint32_t mdio_addr = 0;

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    uint16_t i = 0;
    /* enable I-Cache and D-Cache */
    cache_enable();
    /* initialize the array */
    for(i = 0; i < 8; i++) {
        data_array[i] = i;
    }

    /* clock configuration */
    rcu_config();
    /* NVIC configuration */
    nvic_config();
    /* GPIO configuration */
    gpio_config();
    /* configure the MDIO peripheral */
    mdio_config();

    while(1) {
    }
}


/*!
    \brief      clock configuration
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* MDIO clock enable */
    rcu_periph_clock_enable(RCU_MDIO);
    /* enable the GPIOA clock */
    rcu_periph_clock_enable(RCU_GPIOA);
}

/*!
    \brief      NVIC configuration
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_config(void)
{
    nvic_irq_enable(MDIO_IRQn, 0U, 0U);
}

/*!
    \brief      GPIO configuration
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* configure MDC(PA6)/MDIO(PA10) GPIO port */
    gpio_af_set(GPIOA, GPIO_AF_11, GPIO_PIN_6 | GPIO_PIN_10);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6 | GPIO_PIN_10);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_OD, GPIO_OSPEED_60MHZ, GPIO_PIN_6 | GPIO_PIN_10);
}

/*!
    \brief      configure the MDIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void mdio_config(void)
{
    mdio_deinit();
    mdio_software_reset();

    /* PHY use 5 bits, select software provided address '0x0' as PHYADR, DEVADD is 0x1 */
    mdio_init(MDIO_PHY_BITS_5, 0x0, MDIO_PHYADR_SOFTWARE, DEVADD_PMA_PMD);
    /* configure frame bit timeout */
    mdio_timeout_config(0xFFFF);
    mdio_timeout_enable();

    /* clear all the flags, enable the 4 types of frame interrupts */
    mdio_flag_clear(MDIO_FLAG_WRFRM | MDIO_FLAG_ADDRFRM | MDIO_FLAG_RDINCFRM | MDIO_FLAG_RDFRM);
    mdio_interrupt_enable(MDIO_INT_WRFRM | MDIO_INT_ADDRFRM | MDIO_INT_RDINCFRM | MDIO_INT_RDFRM);
}

/*!
    \brief      enable the CPU Cache
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cache_enable(void)
{
    /* enable I-Cache */
    SCB_EnableICache();

    /* enable D-Cache */
    SCB_EnableDCache();
}
