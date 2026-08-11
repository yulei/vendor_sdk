/*!
    \file    main.c
    \brief   ADC2 channel of temperature, Vref and Vbat

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
#include "systick.h"
#include <stdio.h>

#define ADC_TEMP_CALIBRATION_VALUE_25                REG16(0x1FF0F7C0)
#define ADC_TEMP_CALIBRATION_VALUE_MINUS40           REG16(0x1FF0F7C2)
#define ADC_HIGH_TEMP_CALIBRATION_VALUE_25           REG16(0x1FF0F7C4)

static float temperature;
static float avg_slope;
static float high_precision_temperature;
static float vref_value;
static float vbat_value;

int32_t temperature_value_25;
int32_t temperature_value_minus40;
int32_t high_precision_temperature_value_25;

/* configure the different system clocks */
void rcu_config(void);
/* configure the ADC peripheral */
void adc_config(void);
/* enable the CPU cache */
void cache_enable(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* enable the CPU cache */
    cache_enable();
    /* system clocks configuration */
    rcu_config();
    /* systick configuration */
    systick_config();
    /* configure COM port */
    gd_eval_com_init(EVAL_COM);
    /* ADC configuration */
    adc_config();
    temperature_value_25 = ADC_TEMP_CALIBRATION_VALUE_25 & 0x0FFF;
    high_precision_temperature_value_25 = ADC_HIGH_TEMP_CALIBRATION_VALUE_25 & 0x0FFF;
    temperature_value_minus40 =ADC_TEMP_CALIBRATION_VALUE_MINUS40 & 0x0FFF;
    avg_slope = -(temperature_value_25 - temperature_value_minus40) / (25.0f + 40.0f);

    while(1) {
        /* ADC software trigger enable */
        adc_software_trigger_enable(ADC2, ADC_INSERTED_CHANNEL);
        /* delay a time in milliseconds */
        delay_1ms(1000);
        temperature = (temperature_value_25 - (int32_t)ADC_IDATA1(ADC2))*3.3f/4096/avg_slope*1000 + 25 ;
        high_precision_temperature = ((int32_t)ADC_IDATA3(ADC2) - high_precision_temperature_value_25)*3.3f/4096/3.3f*1000 + 25 ;
        vref_value = (ADC_IDATA2(ADC2) * 3.3f / 4096);
        vbat_value = (ADC_IDATA0(ADC2) * 3.3f / 4096) * 4;
        printf("\r\n inner channel:\r\n");
        printf(" the temperature data is %2.0f degrees Celsius\r\n", temperature);
        printf(" the high precision temperature data is %2.0f degrees Celsius\r\n",
               high_precision_temperature);
        printf(" the reference voltage data is %5.3fV \r\n", vref_value);
        printf(" the battery voltage is %5.3fV\r\n", vbat_value);
    }
}

/*!
    \brief      configure the different system clocks
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC2);
}

/*!
    \brief      configure the ADC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_config(void)
{
    /* reset ADC */
    adc_deinit(ADC2);
    /* ADC clock config */
    rcu_adc_clock_config(IDX_ADC2, RCU_ADCSRC_PER);
    adc_clock_config(ADC2, ADC_CLK_ASYNC_DIV64);
    /* ADC contineous function enable */
    adc_special_function_config(ADC2, ADC_CONTINUOUS_MODE, DISABLE);
    /* ADC scan mode enable */
    adc_special_function_config(ADC2, ADC_SCAN_MODE, ENABLE);
    /* ADC resolution config */
    adc_resolution_config(ADC2, ADC_RESOLUTION_12B);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC2, ADC_DATAALIGN_RIGHT);

    /* ADC channel length config */
    adc_channel_length_config(ADC2, ADC_INSERTED_CHANNEL, 4);

    /* ADC 1/4 voltate of external battery config */
    adc_inserted_channel_config(ADC2, 0, ADC_CHANNEL_17, 20);
    /* ADC temperature sensor channel config */
    adc_inserted_channel_config(ADC2, 1, ADC_CHANNEL_18, 20);
    /* ADC internal reference voltage channel config */
    adc_inserted_channel_config(ADC2, 2, ADC_CHANNEL_19, 20);
    /* ADC high precision temperature sensor channel config */
    adc_inserted_channel_config(ADC2, 3, ADC_CHANNEL_20, 20);

    /* enable 1/4 voltate of external battery channel */
    adc_internal_channel_config(ADC_CHANNEL_INTERNAL_VBAT, ENABLE);
    /* enable ADC temperature channel */
    adc_internal_channel_config(ADC_CHANNEL_INTERNAL_TEMPSENSOR, ENABLE);
    /* enable internal reference voltage channel */
    adc_internal_channel_config(ADC_CHANNEL_INTERNAL_VREFINT, ENABLE);
    /* enable high precision temperature sensor channel */
    adc_internal_channel_config(ADC_CHANNEL_INTERNAL_HP_TEMPSENSOR, ENABLE);

    /* ADC trigger config */
    adc_external_trigger_config(ADC2, ADC_INSERTED_CHANNEL, EXTERNAL_TRIGGER_DISABLE);

    /* enable ADC interface */
    adc_enable(ADC2);
    /* wait for ADC stability */
    delay_1ms(1);
    /* ADC calibration mode config */
    adc_calibration_mode_config(ADC2, ADC_CALIBRATION_OFFSET);
    /* ADC calibration number config */
    adc_calibration_number(ADC2, ADC_CALIBRATION_NUM1);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC2);

    /* ADC software trigger enable */
    adc_software_trigger_enable(ADC2, ADC_INSERTED_CHANNEL);
}

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
    SCB_EnableDCache();
}
