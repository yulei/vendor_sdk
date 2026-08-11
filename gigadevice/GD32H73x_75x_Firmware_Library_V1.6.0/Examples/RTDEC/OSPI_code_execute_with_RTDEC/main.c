/*!
    \file    main.c
    \brief   OSPI flash code decrypt with RTDEC example

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
#include "ospi_operation.h"
#include "gd32h759i_eval.h"
#include "systick.h"
#include <stdio.h>

#define OSPI_PERIPH            OSPI1

#define RTDEC_CLK              RCU_RTDEC1
#define RTDEC_USE              RTDEC1
#define AREA_USE               RTDEC_AREA2
#define AREA_START_ADDR        (uint32_t)0x70004000U
#define AREA_END_ADDR          (uint32_t)0x700041FFU
#define AREA_READ_WRITE_ADDR   (uint32_t)0x00000000U

#define OSPI_START_ADDR        (uint32_t)0x70000000U

#define TEXT_SIZE              64U

uint32_t rtdec_key[4]          = {0x01234567, 0x89ABCDEF, 0x01234567, 0x89ABCDEF};
uint32_t nonce[2]              = {0x89ABCDEF, 0x01234567};
uint16_t fw_version            = 0x0102U;

uint32_t ciper_code_text[4]    = {0x63EA01DF, 0x45CF913E, 0x845EDA11, 0x6EB967AA};

typedef uint32_t (*average_func)(uint32_t a, uint32_t b);
uint32_t test_average(uint32_t a, uint32_t b);
void cache_enable(void);

static void rtdec_struct_init(rtdec_parameter_struct *rtdec_struct);
static uint8_t crc_cal(uint32_t *keyin);

uint32_t average_res = 0U;
uint32_t ospi_flash_func_ave_res = 0U;

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    ospi_parameter_struct ospi_struct;
    rtdec_parameter_struct rtdec_struct;
    uint32_t flash_id;
    average_func ospi_flash_average_func;
    ErrStatus key_cac_status;

    /* enable I-Cache and D-Cache */
    cache_enable();

    systick_config();

    /* configure USART */
    gd_eval_com_init(EVAL_COM);

    printf("RTDEC decrypt code from OSPI flash example.\r\n");

    /* configure OSPI_PERIPH */
    ospi_config(OSPI_PERIPH, &ospi_struct);
    /* reset ospi flash */
    ospi_flash_reset_enable(OSPI_PERIPH, &ospi_struct, SPI_MODE);
    ospi_flash_reset_memory(OSPI_PERIPH, &ospi_struct, SPI_MODE);
    ospi_flash_reset_enable(OSPI_PERIPH, &ospi_struct, OSPI_MODE);
    ospi_flash_reset_memory(OSPI_PERIPH, &ospi_struct, OSPI_MODE);
    delay_1ms(1);

    /* read flash id */
    flash_id = ospi_flash_read_id(OSPI_PERIPH, &ospi_struct);
    printf("OSPI flash id is : 0x%04X.\r\n", flash_id);

    /* erase flash sector for specified address */
    ospi_flash_autopolling_memory_ready(OSPI_PERIPH, &ospi_struct, SPI_MODE);
    ospi_flash_write_enable(OSPI_PERIPH, &ospi_struct);
    ospi_flash_sector_erase(OSPI_PERIPH, &ospi_struct, AREA_START_ADDR - OSPI_START_ADDR);
    ospi_flash_autopolling_memory_ready(OSPI_PERIPH, &ospi_struct, SPI_MODE);

    /* OSPI flash page progrom */
    ospi_flash_autopolling_memory_ready(OSPI_PERIPH, &ospi_struct, SPI_MODE);
    ospi_flash_write_enable(OSPI_PERIPH, &ospi_struct);
    ospi_flash_page_program(OSPI_PERIPH, &ospi_struct, AREA_START_ADDR - OSPI_START_ADDR, TEXT_SIZE, (uint8_t *)ciper_code_text);
    ospi_flash_autopolling_memory_ready(OSPI_PERIPH, &ospi_struct, SPI_MODE);

    /* configure RTDEC */
    rcu_periph_clock_enable(RTDEC_CLK);
    rtdec_deinit(RTDEC_USE);
    rtdec_struct_para_init(&rtdec_struct);
    /* initialize rtdec_struct parameter */
    rtdec_struct_init(&rtdec_struct);
    /* initialize rtdec peripheral */
    key_cac_status = rtdec_init(RTDEC_USE, AREA_USE, &rtdec_struct);
    if(SUCCESS != key_cac_status) {
        printf("RTDEC key CRC val is error.\r\n");
        while(1) {
        }
    }
    /* lock rtdec key */
    rtdec_lock(RTDEC_USE, AREA_USE, RTDEC_ARE_K_LK);
    /* enable RTDEC0 */
    rtdec_enable(RTDEC_USE, AREA_USE);
    /* lock rtdec register */
    rtdec_lock(RTDEC_USE, AREA_USE, RTDEC_ARE_CFG_LK);

    /* configure OSPI FLASH dummy cycles */
    ospi_flash_write_enable(OSPI_PERIPH, &ospi_struct);
    ospi_flash_write_volatile_cfg_reg(OSPI_PERIPH, &ospi_struct, GD25LX512ME_CFG_REG1_ADDR, GD25LX512ME_CFG_16_DUMMY_CYCLES);
    /* configure OSPI FLASH enter STR OSPI mode */
    ospi_flash_write_enable(OSPI_PERIPH, &ospi_struct);
    ospi_flash_write_volatile_cfg_reg(OSPI_PERIPH, &ospi_struct, GD25LX512ME_CFG_REG0_ADDR, GD25LX512ME_CFG_OCTAL_STR_WO);
    ospi_flash_autopolling_memory_ready(OSPI_PERIPH, &ospi_struct, OSPI_MODE);

    /* enable memory map mode */
    ospi_memory_map_enable(OSPI_PERIPH, &ospi_struct);

    /* executing code of arithmetic average */
    ospi_flash_average_func = (average_func)(AREA_START_ADDR + AREA_READ_WRITE_ADDR + 1U);
    ospi_flash_func_ave_res = ospi_flash_average_func(4U, 8U);

    printf("The executed result of the average test code decrypt with RTDEC is: %d\r\n", ospi_flash_func_ave_res);

    average_res = test_average(4U, 8U);

    /* compare average result */
    if(ospi_flash_func_ave_res == average_res) {
        printf("The executed result of RTDEC decrypted code is correct.\r\n");
    } else {
        printf("The executed result of RTDEC decrypted code is error.\r\n");
    }

    while(1) {
    }
}

/*!
    \brief      calculate average
    \param[in]  a: one of the value to average
    \param[in]  b: another one of the value to average
    \param[out] none
    \retval     average val
*/
uint32_t test_average(uint32_t a, uint32_t b)
{
    return ((a + b) / 2U);
}

/*!
    \brief      enable the CPU Chache
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

/*!
    \brief      initialize rtdec struct
    \param[in]  rtdec_struct: RTDEC parameter initialization stuct members of the structure
    \param[out] none
    \retval     none
*/
static void rtdec_struct_init(rtdec_parameter_struct *rtdec_struct)
{
    rtdec_struct->access_mode = RTDEC_MODE_CODE_ACCESS;
    rtdec_struct->key = rtdec_key;
    rtdec_struct->nonce = nonce;
    rtdec_struct->key_crc = crc_cal(rtdec_struct->key);
    rtdec_struct->fw_version = fw_version;
    rtdec_struct->start_addr = AREA_START_ADDR;
    rtdec_struct->end_addr = AREA_END_ADDR;
}

/*!
    \brief      calculate key CRC by software
    \param[in]  keyin: decryption key written in the RTDEC register
    \param[out] none
    \retval     crc_val: CRC value
*/
static uint8_t crc_cal(uint32_t *keyin)
{
    const uint8_t crc7_poly = 0x07U;
    const uint32_t key_strobe[4] = {0xAA55AA55, 0x00000003, 0x00000018, 0x000000C0};
    uint8_t i, j, k, crc_val = 0x00U;
    uint32_t keyval;

    for(j = 0U; j < 4U; j++) {
        keyval = *(keyin + j);

        if(j == 0U) {
            keyval ^= key_strobe[0];
        } else {
            keyval ^= (key_strobe[j] << 24U) | (crc_val << 16U) | (key_strobe[j] << 8U) | crc_val;
        }

        for(i = 0, crc_val = 0; i < 32; i++) {
            k = (((crc_val >> 7U) ^ ((keyval >> (31U - i)) & 0x0FU))) & 1U;
            crc_val <<= 1U;
            if(k) {
                crc_val ^= crc7_poly;
            }
        }
        crc_val ^= 0x55U;
    }
    return crc_val;
}
