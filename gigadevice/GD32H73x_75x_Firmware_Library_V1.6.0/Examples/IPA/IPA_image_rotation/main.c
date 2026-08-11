/*!
    \file    main.c
    \brief   IPA image rotate demo

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
#include <stdio.h>
#include "systick.h"
#include "gd32h759i_eval.h"
#include "image_logo.h"

#define HORIZONTAL_SYNCHRONOUS_PULSE  41
#define HORIZONTAL_BACK_PORCH         2
#define ACTIVE_WIDTH                  480
#define HORIZONTAL_FRONT_PORCH        2

#define VERTICAL_SYNCHRONOUS_PULSE    10
#define VERTICAL_BACK_PORCH           2
#define ACTIVE_HEIGHT                 272
#define VERTICAL_FRONT_PORCH          2

#define AXI_SRAM_ADDR                 0x24000000

#define SPACE_WIDTH                   128
#define SPACE_HEIGHT                  128

__ALIGNED(16) uint8_t test_foreground[SPACE_WIDTH * SPACE_HEIGHT * 4 + 12] = {0};
__ALIGNED(16) uint8_t test_background[SPACE_WIDTH * SPACE_HEIGHT * 4 + 12] = {0};
__ALIGNED(16) uint8_t test_destination[SPACE_WIDTH * SPACE_HEIGHT * 4 + 12] = {0};

uint32_t test_width  = 128;
uint32_t test_height = 128;

tli_parameter_struct               tli_init_struct;
tli_layer_parameter_struct         tli_layer0_init_struct;
tli_layer_parameter_struct         tli_layer1_init_struct;

ipa_destination_parameter_struct  ipa_destination_init_struct;
ipa_foreground_parameter_struct   ipa_fg_init_struct;
ipa_background_parameter_struct   ipa_bg_init_struct;

/* private function prototypes */
static void ipa_config(uint32_t *fore_buf, uint32_t *back_buf, uint32_t *dest_buf);
static void tli_config(uint32_t test_width, uint32_t test_height);
static void tli_gpio_config(void);
void cache_enable(void);
void mpu_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    static uint8_t flag = 0;
    /* enable the CPU cache */
    cache_enable();
    /* configure MPU */
    mpu_config();

    systick_config();
    gd_eval_key_init(KEY_TAMPER, KEY_MODE_GPIO);

    /* initialize TLI and TLI layer */
    tli_config(test_width, test_height);
    tli_layer_enable(LAYER0);
    tli_layer_enable(LAYER1);
    tli_reload_config(TLI_REQUEST_RELOAD_EN);
    tli_enable();

    /* IPA configuration */
    ipa_config((uint32_t *)&test_foreground, (uint32_t *)&test_background, (uint32_t *)&test_destination);

    uint32_t i, h = 0, v = 0;
    /* copy the image from FLASH to test_foreground */
    for(v = 0, i = 0; v < test_height; v++) {
        for(h = 0; h < test_width; h++) {
            test_foreground[(v * test_width + h) * 4 + 0] = (uint8_t)(img_logo[i] & 0x000000FF);
            test_foreground[(v * test_width + h) * 4 + 1] = (uint8_t)((img_logo[i] & 0x0000FF00) >> 8);
            test_foreground[(v * test_width + h) * 4 + 2] = (uint8_t)((img_logo[i] & 0x00FF0000) >> 16);
            test_foreground[(v * test_width + h) * 4 + 3] = (uint8_t)((img_logo[i] & 0xFF000000) >> 24);
            i++;
        }
    }

    ipa_destination_init_struct.image_rotate        = DESTINATION_ROTATE_0;
    ipa_destination_init_struct.destination_memaddr = (uint32_t)&test_destination;
    /* ipa destination initialization */
    ipa_destination_init(&ipa_destination_init_struct);
    /* transfer enable */
    ipa_transfer_enable();

    while(1) {
        /* wait for key is pressed */
        if(gd_eval_key_state_get(KEY_TAMPER) != SET) {
            delay_1ms(100);
            if(gd_eval_key_state_get(KEY_TAMPER) != SET) {
                /* image rotate */
                switch(flag) {
                case 0:
                    ipa_destination_init_struct.image_rotate = DESTINATION_ROTATE_90;
                    flag = 1;
                    break;
                case 1:
                    ipa_destination_init_struct.image_rotate = DESTINATION_ROTATE_180;
                    flag = 2;
                    break;
                case 2:
                    ipa_destination_init_struct.image_rotate = DESTINATION_ROTATE_270;
                    flag = 3;
                    break;
                default:
                    ipa_destination_init_struct.image_rotate = DESTINATION_ROTATE_0;
                    flag = 0;
                }
                ipa_destination_init_struct.destination_memaddr = (uint32_t)&test_destination;
                /* ipa destination initialization */
                ipa_destination_init(&ipa_destination_init_struct);
                /* transfer enable */
                ipa_transfer_enable();

                while(ipa_flag_get(IPA_FLAG_FTF) == RESET);
                ipa_flag_clear(IPA_FLAG_FTF);
            }
        }
    }
}

/*!
    \brief      IPA initialize and configuration
    \param[in]  fore_buf: foreground buffer address
    \param[in]  back_buf: background buffer address
    \param[in]  dest_buf: destination buffer address
    \param[out] none
    \retval     none
*/
static void ipa_config(uint32_t *fore_buf, uint32_t *back_buf, uint32_t *dest_buf)
{
    rcu_periph_clock_enable(RCU_IPA);
    ipa_deinit();

    ipa_foreground_struct_para_init(&ipa_fg_init_struct);
    ipa_background_struct_para_init(&ipa_bg_init_struct);
    ipa_destination_struct_para_init(&ipa_destination_init_struct);

    /* configure IPA pixel format convert mode */
    ipa_pixel_format_convert_mode_set(IPA_FGBGTODE);
    /* configure destination pixel format */
    ipa_destination_init_struct.destination_pf          = IPA_DPF_ARGB8888;
    /* configure destination memory base address */
    ipa_destination_init_struct.destination_memaddr     = (uint32_t)&test_destination;
    /* configure destination pre-defined alpha value RGB */
    ipa_destination_init_struct.destination_prealpha    = 0;
    ipa_destination_init_struct.destination_prered      = 0;
    ipa_destination_init_struct.destination_pregreen    = 0;
    ipa_destination_init_struct.destination_preblue     = 0;
    /* configure destination line offset */
    ipa_destination_init_struct.destination_lineoff     = 0;

    /* configure height of the image to be processed */
    /* configure width of the image to be processed */
    ipa_destination_init_struct.image_height            = test_height;
    ipa_destination_init_struct.image_width             = test_width;

    ipa_destination_init_struct.image_scaling_width     = 0x1000;
    ipa_destination_init_struct.image_scaling_height    = 0x1000;

    ipa_destination_init_struct.image_rotate            = DESTINATION_ROTATE_0;
    /* ipa destination initialization */
    ipa_destination_init(&ipa_destination_init_struct);

    /* configure IPA foreground */
    ipa_fg_init_struct.foreground_memaddr               = (uint32_t)&test_foreground;
    ipa_fg_init_struct.foreground_pf                    = FOREGROUND_PPF_ARGB8888;
    ipa_fg_init_struct.foreground_alpha_algorithm       = IPA_FG_ALPHA_MODE_0;
    ipa_fg_init_struct.foreground_prealpha              = 0x00;
    ipa_fg_init_struct.foreground_lineoff               = 0x00;
    ipa_fg_init_struct.foreground_preblue               = 0x00;
    ipa_fg_init_struct.foreground_pregreen              = 0x00;
    ipa_fg_init_struct.foreground_prered                = 0x00;
    /* foreground initialization */
    ipa_foreground_init(&ipa_fg_init_struct);

    /* configure IPA background */
    ipa_bg_init_struct.background_memaddr               = (uint32_t)&test_background;
    ipa_bg_init_struct.background_pf                    = BACKGROUND_PPF_ARGB8888;
    ipa_bg_init_struct.background_alpha_algorithm       = IPA_BG_ALPHA_MODE_0;
    ipa_bg_init_struct.background_prealpha              = 0x00;
    ipa_bg_init_struct.background_lineoff               = 0x00;
    ipa_bg_init_struct.background_preblue               = 0x00;
    ipa_bg_init_struct.background_pregreen              = 0x00;
    ipa_bg_init_struct.background_prered                = 0x00;
    /* background initialization */
    ipa_background_init(&ipa_bg_init_struct);
}

/*!
    \brief      LCD configuration
    \param[in]  test_width: width of test image
    \param[in]  test_height: height of test image
    \param[out] none
    \retval     none
*/
static void tli_config(uint32_t test_width, uint32_t test_height)
{
    rcu_periph_clock_enable(RCU_TLI);
    tli_gpio_config();

    rcu_pll_input_output_clock_range_config(IDX_PLL2, RCU_PLL2RNG_1M_2M, RCU_PLL2VCO_192M_836M);
    if(ERROR == rcu_pll2_config(HXTAL_VALUE / 1000000, 240, 3, 3, 3)) {
        while(1) {
        }
    }
    rcu_pll_clock_output_enable(RCU_PLL2R);
    rcu_tli_clock_div_config(RCU_PLL2R_DIV8);
    rcu_osci_on(RCU_PLL2_CK);
    if(ERROR == rcu_osci_stab_wait(RCU_PLL2_CK)) {
        while(1) {
        }
    }

    /* configure TLI parameter structure */
    tli_init_struct.signalpolarity_hs = TLI_HSYN_ACTIVE_LOW;
    tli_init_struct.signalpolarity_vs = TLI_VSYN_ACTIVE_LOW;
    tli_init_struct.signalpolarity_de = TLI_DE_ACTIVE_LOW;
    tli_init_struct.signalpolarity_pixelck = TLI_PIXEL_CLOCK_TLI;
    /* LCD display timing configuration */
    tli_init_struct.synpsz_hpsz = HORIZONTAL_SYNCHRONOUS_PULSE - 1;
    tli_init_struct.synpsz_vpsz = VERTICAL_SYNCHRONOUS_PULSE - 1;
    tli_init_struct.backpsz_hbpsz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH - 1;
    tli_init_struct.backpsz_vbpsz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH - 1;
    tli_init_struct.activesz_hasz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH + ACTIVE_WIDTH - 1;
    tli_init_struct.activesz_vasz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH + ACTIVE_HEIGHT - 1;
    tli_init_struct.totalsz_htsz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH + ACTIVE_WIDTH + HORIZONTAL_FRONT_PORCH - 1;
    tli_init_struct.totalsz_vtsz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH + ACTIVE_HEIGHT + VERTICAL_FRONT_PORCH - 1;
    /* LCD background color configuration */
    tli_init_struct.backcolor_red = 0xFF;
    tli_init_struct.backcolor_green = 0xFF;
    tli_init_struct.backcolor_blue = 0xFF;
    tli_init(&tli_init_struct);

    /* TLI layer0 configuration */
    tli_layer0_init_struct.layer_window_leftpos = 70 + HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH + 1;
    tli_layer0_init_struct.layer_window_rightpos = (70 + test_width + HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH - 1);
    tli_layer0_init_struct.layer_window_toppos = 70 + VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH;
    tli_layer0_init_struct.layer_window_bottompos = 70 + test_height + VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH - 1;
    tli_layer0_init_struct.layer_ppf = LAYER_PPF_ARGB8888;
    tli_layer0_init_struct.layer_sa = 250;
    tli_layer0_init_struct.layer_acf1 = LAYER_ACF1_PASA;
    tli_layer0_init_struct.layer_acf2 = LAYER_ACF2_PASA;
    tli_layer1_init_struct.layer_default_alpha = 0;
    tli_layer1_init_struct.layer_default_blue = 0;
    tli_layer1_init_struct.layer_default_green = 0;
    tli_layer1_init_struct.layer_default_red = 0;
    tli_layer0_init_struct.layer_frame_bufaddr = (uint32_t)&test_foreground;
    tli_layer0_init_struct.layer_frame_line_length = ((test_width * 4) + 7);
    tli_layer0_init_struct.layer_frame_buf_stride_offset = (test_width * 4);
    tli_layer0_init_struct.layer_frame_total_line_number = test_height;
    tli_layer_init(LAYER0, &tli_layer0_init_struct);

    /* TLI layer1 configuration */
    tli_layer1_init_struct.layer_window_leftpos = 260 + HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH + 1;
    tli_layer1_init_struct.layer_window_rightpos = (260 + test_width + HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH - 1);
    tli_layer1_init_struct.layer_window_toppos = 70 + VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH;
    tli_layer1_init_struct.layer_window_bottompos = 70 + test_height + VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH - 1;
    tli_layer1_init_struct.layer_ppf = LAYER_PPF_ARGB8888;
    tli_layer1_init_struct.layer_sa = 250;
    tli_layer1_init_struct.layer_acf1 = LAYER_ACF1_PASA;
    tli_layer1_init_struct.layer_acf2 = LAYER_ACF2_PASA;
    tli_layer1_init_struct.layer_default_alpha = 0;
    tli_layer1_init_struct.layer_default_blue = 0;
    tli_layer1_init_struct.layer_default_green = 0;
    tli_layer1_init_struct.layer_default_red = 0;
    tli_layer1_init_struct.layer_frame_bufaddr = (uint32_t)&test_destination;
    tli_layer1_init_struct.layer_frame_line_length = ((test_width * 4) + 7);
    tli_layer1_init_struct.layer_frame_buf_stride_offset = (test_width * 4);
    tli_layer1_init_struct.layer_frame_total_line_number = test_height;
    tli_layer_init(LAYER1, &tli_layer1_init_struct);
}

/*!
    \brief      configure TLI GPIO
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void tli_gpio_config(void)
{
    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOE);
    rcu_periph_clock_enable(RCU_GPIOF);
    rcu_periph_clock_enable(RCU_GPIOG);
    rcu_periph_clock_enable(RCU_GPIOH);

    /* configure HSYNC(PE15), VSYNC(PA7), PCLK(PG7), DE(PF10) */
    /* configure LCD_R7(PG6), LCD_R6(PH12), LCD_R5(PH11), LCD_R4(PA5), LCD_R3(PH9),LCD_R2(PH8),
                 LCD_R1(PH3), LCD_R0(PH2), LCD_G7(PD3), LCD_G6(PC7), LCD_G5(PC1), LCD_G4(PH15),
                 LCD_G3(PH14), LCD_G2(PH13),LCD_G1(PB0), LCD_G0(PB1),LCD_B7(PB9), LCD_B6(PB8),
                 LCD_B5(PB5), LCD_B4(PC11), LCD_B3(PG11),LCD_B2(PG10), LCD_B1(PG12), LCD_B0(PG14) */
    /* configure TLI pins AF function */
    gpio_af_set(GPIOA, GPIO_AF_14, GPIO_PIN_5);
    gpio_af_set(GPIOA, GPIO_AF_14, GPIO_PIN_7);

    gpio_af_set(GPIOB, GPIO_AF_14, GPIO_PIN_0);
    gpio_af_set(GPIOB, GPIO_AF_14, GPIO_PIN_1);
    gpio_af_set(GPIOB, GPIO_AF_3, GPIO_PIN_5);
    gpio_af_set(GPIOB, GPIO_AF_14, GPIO_PIN_8);
    gpio_af_set(GPIOB, GPIO_AF_14, GPIO_PIN_9);

    gpio_af_set(GPIOC, GPIO_AF_14, GPIO_PIN_1);
    gpio_af_set(GPIOC, GPIO_AF_14, GPIO_PIN_7);

    gpio_af_set(GPIOD, GPIO_AF_14, GPIO_PIN_3);

    gpio_af_set(GPIOE, GPIO_AF_10, GPIO_PIN_15);

    gpio_af_set(GPIOF, GPIO_AF_14, GPIO_PIN_10);

    gpio_af_set(GPIOG, GPIO_AF_14, GPIO_PIN_6);
    gpio_af_set(GPIOG, GPIO_AF_14, GPIO_PIN_7);
    gpio_af_set(GPIOG, GPIO_AF_14, GPIO_PIN_10);
    gpio_af_set(GPIOG, GPIO_AF_14, GPIO_PIN_11);
    gpio_af_set(GPIOG, GPIO_AF_14, GPIO_PIN_12);
    gpio_af_set(GPIOG, GPIO_AF_14, GPIO_PIN_14);

    gpio_af_set(GPIOH, GPIO_AF_14, GPIO_PIN_2);
    gpio_af_set(GPIOH, GPIO_AF_14, GPIO_PIN_3);
    gpio_af_set(GPIOH, GPIO_AF_14, GPIO_PIN_8);
    gpio_af_set(GPIOH, GPIO_AF_14, GPIO_PIN_9);
    gpio_af_set(GPIOH, GPIO_AF_14, GPIO_PIN_12);
    gpio_af_set(GPIOH, GPIO_AF_14, GPIO_PIN_13);
    gpio_af_set(GPIOH, GPIO_AF_14, GPIO_PIN_14);
    gpio_af_set(GPIOH, GPIO_AF_14, GPIO_PIN_15);

    /* configure TLI GPIO */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5 | GPIO_PIN_7);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_5 | GPIO_PIN_7);

    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_9);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_5 | GPIO_PIN_8 |
                            GPIO_PIN_9);

    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_1 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_11);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_1 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_11);

    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_3);

    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_15);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_15);

    gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_10);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_10);

    gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_14);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 |
                            GPIO_PIN_14);

    gpio_mode_set(GPIOH, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_11 |
                  GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_11 |
                            GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);

    /* LCD PWM BackLight(PG13) */
    gpio_mode_set(GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_13);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_13);
    gpio_bit_set(GPIOG, GPIO_PIN_13);
}

/*!
    \brief      enable the CPU cache
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cache_enable(void)
{
    /* enable I-Cache and D-Cache */
    SCB_EnableICache();
    SCB_EnableDCache();
}

/*!
    \brief      configure the MPU attributes
    \param[in]  none
    \param[out] none
    \retval     none
*/
void mpu_config(void)
{
    mpu_region_init_struct mpu_init_struct;
    mpu_region_struct_para_init(&mpu_init_struct);

    /* disable the MPU */
    ARM_MPU_Disable();
    ARM_MPU_SetRegion(0, 0);

    /* configure the MPU attributes for AXI SRAM */
    mpu_init_struct.region_base_address  = AXI_SRAM_ADDR;
    mpu_init_struct.region_size          = MPU_REGION_SIZE_512KB;
    mpu_init_struct.access_permission    = MPU_AP_FULL_ACCESS;
    mpu_init_struct.access_bufferable    = MPU_ACCESS_BUFFERABLE;
    mpu_init_struct.access_cacheable     = MPU_ACCESS_CACHEABLE;
    mpu_init_struct.access_shareable     = MPU_ACCESS_NON_SHAREABLE;
    mpu_init_struct.region_number        = MPU_REGION_NUMBER0;
    mpu_init_struct.subregion_disable    = MPU_SUBREGION_ENABLE;
    mpu_init_struct.instruction_exec     = MPU_INSTRUCTION_EXEC_PERMIT;
    mpu_init_struct.tex_type             = MPU_TEX_TYPE0;
    mpu_region_config(&mpu_init_struct);
    mpu_region_enable();

    /* enable the MPU */
    ARM_MPU_Enable(MPU_MODE_PRIV_DEFAULT);
}
