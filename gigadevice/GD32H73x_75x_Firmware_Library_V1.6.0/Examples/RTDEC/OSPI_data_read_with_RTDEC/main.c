/*!
    \file    main.c
    \brief   OSPI flash data decrypt with RTDEC example

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

#define OSPI_PERIPH            OSPI0

#define RTDEC_CLK              RCU_RTDEC0
#define RTDEC_USE              RTDEC0
#define AREA_USE               RTDEC_AREA2
#define AREA_START_ADDR        (uint32_t)0x90008000U
#define AREA_END_ADDR          (uint32_t)0x900081FFU
#define AREA_READ_WRITE_ADDR   (uint32_t)0x00000000U

#define OSPI_START_ADDR        (uint32_t)0x90000000U

#define TEXT_SIZE              (uint16_t)0x0100U

uint32_t rtdec_key[4]          = {0x01234567, 0x89ABCDEF, 0x01234567, 0x89ABCDEF};
uint32_t nonce[2]              = {0x89ABCDEF, 0x01234567};
uint16_t fw_version            = 0x0112U;

uint8_t read_test[TEXT_SIZE];

uint32_t ciper_text[64]        = {0xD55D9857, 0xDFEC060B, 0x4645FF84, 0xF15772B6, 0x77A96DE7, 0x74C0C17F, 0x6BE063F7, 0xA1A88AF6,
                                  0x05DAC790, 0x71CC1956, 0xA3F443D3, 0x00102EEE, 0x22606298, 0x5339861B, 0x56677C1F, 0x9C114A93,
                                  0x7658ADA5, 0x261B0879, 0x1B9C00CE, 0xB6DC887F, 0x6A7C545B, 0x24A149C3, 0xEA6203F5, 0x0240F185,
                                  0xFA940162, 0x060721BA, 0x3E36F23F, 0x1978C90C, 0x6EDB11EC, 0x89F6B95C, 0x352C5456, 0x29E7BB2C,
                                  0xF3F9FE29, 0xC3A86553, 0x4FC52CB1, 0xBAD9909F, 0xAEAE1FF8, 0x1A455165, 0x1D25C323, 0x40F03332,
                                  0xD1AE5E00, 0xCFB621E3, 0x0F52BE89, 0x08F77FE5, 0xA0C5C441, 0x3E8A32D4, 0xC8F629BF, 0xBE80E4F9,
                                  0x72D39660, 0xF012E870, 0xD10CE422, 0xC9F622B6, 0x602E7735, 0x035821E6, 0x7B482F0C, 0x9ADDEE28,
                                  0xF6D1066B, 0xA6EF7282, 0x1A6CF0D5, 0xF6749EF7, 0x4BC87DFE, 0x5F1585E7, 0x988B6247, 0x912C017D
                                 };

__IO uint32_t *ospi_memory_addr;

void cache_enable(void);
static void rtdec_struct_init(rtdec_parameter_struct *rtdec_struct);
static uint8_t crc_cal(uint32_t *keyin);
ErrStatus memory_compare(uint8_t *src, uint8_t *dst, uint16_t length);

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
    uint32_t *preaddata = NULL;
    uint16_t i;
    ErrStatus key_cac_status;

    /* enable I-Cache and D-Cache */
    cache_enable();

    systick_config();

    /* configure USART */
    gd_eval_com_init(EVAL_COM);

    printf("RTDEC decrypt data from OSPI flash example.\r\n");

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
    ospi_flash_page_program(OSPI_PERIPH, &ospi_struct, AREA_START_ADDR - OSPI_START_ADDR, TEXT_SIZE, (uint8_t *)ciper_text);
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

    /* read data in memory map mode with RTDEC decrypt */
    ospi_memory_addr = (uint32_t *)AREA_START_ADDR + AREA_READ_WRITE_ADDR;
    preaddata = (uint32_t *)read_test;
    for(i = 0; i < TEXT_SIZE / 4; i++) {
        preaddata[i] = *(uint32_t *)(ospi_memory_addr + i);
    }

    printf("\r\nThe decpyted result by RTDEC is: %s\r\n", read_test);

    printf("RTDEC decrypt data is successed.\r\n");

    while(1) {
    }
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
    rtdec_struct->access_mode = RTDEC_MODE_DATA_ACCESS;
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

/*!
    \brief      memory compare function
    \param[in]  src: source data pointer
    \param[in]  dst: destination data pointer
    \param[in]  length: the compare data length
    \param[out] none
    \retval     ErrStatus: ERROR or SUCCESS
*/
ErrStatus memory_compare(uint8_t *src, uint8_t *dst, uint16_t length)
{
    while(length--) {
        if(*src++ != *dst++) {
            return ERROR;
        }
    }
    return SUCCESS;
}
