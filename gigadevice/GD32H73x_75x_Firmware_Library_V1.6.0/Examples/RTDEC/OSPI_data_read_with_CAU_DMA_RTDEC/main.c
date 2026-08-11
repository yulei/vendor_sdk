/*!
    \file    main.c
    \brief   data encrypt/decrypt by using CAU in DMA mode and RTDEC example

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
#include <string.h>

#define OSPI_PERIPH             OSPI1

#define RTDEC_CLK               RCU_RTDEC1
#define RTDEC_USE               RTDEC1
#define AREA_USE                RTDEC_AREA3
#define AREA_START_ADDR         (uint32_t)0x70028000U
#define AREA_END_ADDR           (uint32_t)0x700281FFU
#define AREA_READ_WRITE_ADDR    (uint32_t)0x00000000U

#define OSPI_START_ADDR         (uint32_t)0x70000000U

#define TEXT_SIZE               (uint16_t)0x0100U

uint32_t rtdec_key[4]          = {0x01234567, 0x89ABCDEF, 0x01234567, 0x89ABCDEF};
uint32_t nonce[2]              = {0x89ABCDEF, 0x01234567};
uint16_t fw_version            = 0x0112U;

__attribute__ ((aligned(32)))  uint8_t plain_text[TEXT_SIZE]  = "This message is encrypted by CAU with DMA and decrypted by RTDEC based on GD32H73X_H75X MCU.\r\n";
__attribute__ ((aligned(32))) uint8_t swapped_plain_text[TEXT_SIZE];
__attribute__ ((aligned(32))) uint8_t ciper_text[TEXT_SIZE];
__attribute__ ((aligned(32))) uint8_t swapped_ciper_text[TEXT_SIZE];

uint8_t read_test[TEXT_SIZE];

__IO uint32_t *ospi_memory_addr;

void cache_enable(void);
static void rtdec_struct_init(rtdec_parameter_struct *rtdec_struct);
static uint8_t crc_cal(uint32_t *keyin);
static void data_word_swap(uint32_t *input, uint32_t *output, uint32_t size);
static void plain_text_encrypt(void);
static void aes_128_dma(uint32_t *source, uint32_t *dest);

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

    /* encrypt the plain text */
    plain_text_encrypt();

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

    ospi_memory_addr = (uint32_t *)AREA_START_ADDR + AREA_READ_WRITE_ADDR;

    /* read data in memory map mode with RTDEC decrypt */
    preaddata = (uint32_t *)read_test;
    for(i = 0; i < TEXT_SIZE / 4; i++) {
        preaddata[i] = *(uint32_t *)(ospi_memory_addr + i);
    }

    /* compare read decrypted data to the plain data */
    for(i = 0; i < TEXT_SIZE; i++) {
        if(read_test[i] != plain_text[i]) {
            printf("RTDEC decrypt data is failed, data index is: %d.\r\n", i);

            while(1) {
            }
        }
        ospi_memory_addr++;
    }

    printf("The decpyted result by RTDEC is: %s\r\n", read_test);

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
    \brief      swap data that encrpted for CAU or decrypted fo RTDEC
    \param[in]  input: input data to be swaped
    \param[in]  size: data size
    \param[out] output: output swapped data
    \retval     none
*/
static void data_word_swap(uint32_t *input, uint32_t *output, uint32_t size)
{
    uint32_t i, j;
    for(i = 0; i < size; i += 4) {
        for(j = 0; j < 4; j++) {
            output[i + j] = input[i + 4 - j - 1];
        }
    }
}

/*!
    \brief      encrypt the plain text
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void plain_text_encrypt(void)
{
    data_word_swap((uint32_t *)plain_text, (uint32_t *)swapped_plain_text, TEXT_SIZE / 4);

    SCB_CleanDCache_by_Addr((uint32_t*)swapped_plain_text, TEXT_SIZE);
    aes_128_dma((uint32_t *)swapped_plain_text, (uint32_t *)swapped_ciper_text);
    SCB_InvalidateDCache_by_Addr((uint32_t*)swapped_ciper_text, TEXT_SIZE);

    data_word_swap((uint32_t *)swapped_ciper_text, (uint32_t *)ciper_text, TEXT_SIZE / 4);
}

/*!
    \brief      encrypt data using CAU AES-128 in CTR mode with DMA
    \param[in]  source: pointer to the source address
    \param[in]  dest: pointer to the dest address
    \param[out] none
    \retval     none
*/
static void aes_128_dma(uint32_t *source, uint32_t *dest)
{
    cau_key_parameter_struct key_initpara;
    cau_iv_parameter_struct iv_initpara;
    dma_single_data_parameter_struct dma_initpara;

    /* enable CAU clock */
    rcu_periph_clock_enable(RCU_CAU);
    /* enable DMA and DMAMUX clock */
    rcu_periph_clock_enable(RCU_DMA0);
    rcu_periph_clock_enable(RCU_DMAMUX);

    /* key structure initialization */
    cau_deinit();
    cau_key_struct_para_init(&key_initpara);
    cau_aes_keysize_config(CAU_KEYSIZE_128BIT);
    key_initpara.key_2_high = rtdec_key[3];
    key_initpara.key_2_low  = rtdec_key[2];
    key_initpara.key_3_high = rtdec_key[1];
    key_initpara.key_3_low  = rtdec_key[0];
    /* key initialization */
    cau_key_init(&key_initpara);

    /* vectors initialization */
    iv_initpara.iv_0_high = nonce[1];
    iv_initpara.iv_0_low = nonce[0];
    iv_initpara.iv_1_high = fw_version;
    iv_initpara.iv_1_low = (AREA_USE << 28U) | ((AREA_START_ADDR + AREA_READ_WRITE_ADDR) >> 4U);
    cau_iv_init(&iv_initpara);

    /* flush the IN and OUT FIFOs */
    cau_fifo_flush();

    /* initialize the CAU peripheral */
    cau_init(CAU_ENCRYPT, CAU_MODE_AES_CTR, CAU_SWAPPING_32BIT);
    cau_enable();

    /* enable the CAU DMA interface */
    cau_dma_enable(CAU_DMA_INFIFO | CAU_DMA_OUTFIFO);

    /* DMA configuration */
    dma_deinit(DMA0, DMA_CH5);
    dma_deinit(DMA0, DMA_CH6);
    dma_single_data_para_struct_init(&dma_initpara);

    /* DMA channel5 for CAU_IN configuration */
    dma_initpara.direction           = DMA_MEMORY_TO_PERIPH;
    dma_initpara.memory0_addr        = (uint32_t)source;
    dma_initpara.memory_inc          = DMA_MEMORY_INCREASE_ENABLE;
    dma_initpara.periph_memory_width = DMA_PERIPH_WIDTH_32BIT;
    dma_initpara.number              = (TEXT_SIZE / 4);
    dma_initpara.periph_addr         = (uint32_t)(&CAU_DI);
    dma_initpara.periph_inc          = DMA_PERIPH_INCREASE_DISABLE;
    dma_initpara.priority            = DMA_PRIORITY_HIGH;
    dma_initpara.request             = DMA_REQUEST_CAU_IN;
    dma_initpara.circular_mode       = DMA_CIRCULAR_MODE_DISABLE;
    dma_single_data_mode_init(DMA0, DMA_CH5, &dma_initpara);

    /* disable the DMAMUX_MUXCH5 synchronization mode */
    dmamux_synchronization_disable(DMAMUX_MUXCH5);

    /* DMA channel6 for CAU_OUT configuration */
    dma_initpara.direction           = DMA_PERIPH_TO_MEMORY;
    dma_initpara.memory0_addr        = (uint32_t)dest;
    dma_initpara.memory_inc          = DMA_MEMORY_INCREASE_ENABLE;
    dma_initpara.periph_memory_width = DMA_PERIPH_WIDTH_32BIT;
    dma_initpara.number              = (TEXT_SIZE / 4);
    dma_initpara.periph_addr         = (uint32_t)(&CAU_DO);
    dma_initpara.periph_inc          = DMA_PERIPH_INCREASE_DISABLE;
    dma_initpara.priority            = DMA_PRIORITY_HIGH;
    dma_initpara.request             = DMA_REQUEST_CAU_OUT;
    dma_initpara.circular_mode       = DMA_CIRCULAR_MODE_DISABLE;
    dma_single_data_mode_init(DMA0, DMA_CH6, &dma_initpara);

    /* disable the DMAMUX_MUXCH6 synchronization mode */
    dmamux_synchronization_disable(DMAMUX_MUXCH6);

    /* enable DMA transfer */
    dma_channel_enable(DMA0, DMA_CH5);
    dma_channel_enable(DMA0, DMA_CH6);

    /* wait until the last transfer from OUT FIFO */
    while(RESET == dma_flag_get(DMA0, DMA_CH6, DMA_FLAG_FTF)) {
    }

    dma_flag_clear(DMA0, DMA_CH5, DMA_FLAG_FTF);
    dma_flag_clear(DMA0, DMA_CH6, DMA_FLAG_FTF);

    cau_disable();
    cau_dma_disable(CAU_DMA_INFIFO | CAU_DMA_OUTFIFO);

    dma_channel_disable(DMA0, DMA_CH5);
    dma_channel_disable(DMA0, DMA_CH6);
}
