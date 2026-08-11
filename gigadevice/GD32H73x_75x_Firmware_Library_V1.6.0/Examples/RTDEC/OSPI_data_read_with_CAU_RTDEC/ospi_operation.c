/*!
    \file    ospi_operation.c
    \brief   OSPI operation

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
#include <stdio.h>

/*!
    \brief      configure OSPI/OSPIM and GPIO
    \param[in]  ospi_periph: OSPIx(x=0,1)
    \param[out] ospi_struct: OSPI parameter initialization stuct members of the structure
                             and the member values are shown as below:
                prescaler: between 0 and 255
                fifo_threshold: OSPI_FIFO_THRESHOLD_x (x = 1, 2, ..., 31, 32)
                sample_shift: OSPI_SAMPLE_SHIFTING_NONE, OSPI_SAMPLE_SHIFTING_HALF_CYCLE
                device_size: OSPI_MESZ_x_BYTES (x = 2, 4, 8, ..., 512, 1024)
                             OSPI_MESZ_x_KBS (x = 2, 4, 8, ..., 512, 1024)
                             OSPI_MESZ_x_MBS (x = 2, 4, 8, ..., 2048, 4096)
                cs_hightime: OSPI_CS_HIGH_TIME_x_CYCLE (x = 1, 2, ..., 63, 64)
                memory_type: OSPI_MICRON_MODE, OSPI_MACRONIX_MODE, OSPI_STANDARD_MODE
                             OSPI_MACRONIX_RAM_MODE,
                wrap_size: OSPI_DIRECT, OSPI_WRAP_16BYTES, OSPI_WRAP_32BYTES
                           OSPI_WRAP_64BYTES, OSPI_WRAP_128BYTES
                delay_hold_cycle: OSPI_DELAY_HOLD_NONE, OSPI_DELAY_HOLD_QUARTER_CYCLE
    \retval     none
*/
void ospi_config(uint32_t ospi_periph, ospi_parameter_struct *ospi_struct)
{
    rcu_periph_clock_enable(RCU_OSPIM);
    ospim_deinit();

    /* connect port to OSPI0_IO0 -- PD11
                       OSPI0_IO1 -- PD12
                       OSPI0_IO2 -- PA3
                       OSPI0_IO3 -- PD13
                       OSPI0_IO4 -- PD4
                       OSPI0_IO5 -- PD5
                       OSPI0_IO6 -- PD6
                       OSPI0_IO7 -- PD7
                       OSPI0_NCS -- PB6
                       OSPI0_CLK -- PB2   */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOD);

    gpio_af_set(GPIOA, GPIO_AF_6, GPIO_PIN_3);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_3);

    gpio_af_set(GPIOB, GPIO_AF_9, GPIO_PIN_2);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_2);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_2);

    gpio_af_set(GPIOB, GPIO_AF_10, GPIO_PIN_6);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_6);

    gpio_af_set(GPIOD, GPIO_AF_10, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);

    gpio_af_set(GPIOD, GPIO_AF_9, GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13);
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13);

    /* enable SCK, CSN, IO[3:0] and IO[7:4] for OSPIM port0 */
    ospim_port_sck_config(OSPIM_PORT0, OSPIM_PORT_SCK_ENABLE);
    ospim_port_csn_config(OSPIM_PORT0, OSPIM_PORT_CSN_ENABLE);
    ospim_port_io3_0_config(OSPIM_PORT0, OSPIM_IO_LOW_ENABLE);
    ospim_port_io7_4_config(OSPIM_PORT0, OSPIM_IO_HIGH_ENABLE);

    switch(ospi_periph) {
    case OSPI0:
        rcu_periph_clock_enable(RCU_OSPI0);
        ospi_deinit(ospi_periph);
        /* configure OSPIM port1 */
        ospim_port_sck_source_select(OSPIM_PORT0, OSPIM_SCK_SOURCE_OSPI0_SCK);
        ospim_port_csn_source_select(OSPIM_PORT0, OSPIM_CSN_SOURCE_OSPI0_CSN);
        ospim_port_io3_0_source_select(OSPIM_PORT0, OSPIM_SRCPLIO_OSPI0_IO_LOW);
        ospim_port_io7_4_source_select(OSPIM_PORT0, OSPIM_SRCPHIO_OSPI0_IO_HIGH);

        break;
    case OSPI1:
        rcu_periph_clock_enable(RCU_OSPI1);
        ospi_deinit(ospi_periph);
        /* configure OSPIM port1 */
        ospim_port_sck_source_select(OSPIM_PORT0, OSPIM_SCK_SOURCE_OSPI1_SCK);
        ospim_port_csn_source_select(OSPIM_PORT0, OSPIM_CSN_SOURCE_OSPI1_CSN);
        ospim_port_io3_0_source_select(OSPIM_PORT0, OSPIM_SRCPLIO_OSPI1_IO_LOW);
        ospim_port_io7_4_source_select(OSPIM_PORT0, OSPIM_SRCPHIO_OSPI1_IO_HIGH);
        break;
    default:
        break;
    }

    /* initialize the parameters of OSPI struct with default values */
    ospi_struct_init(ospi_struct);

    ospi_struct->prescaler         = 9U;
    ospi_struct->sample_shift      = OSPI_SAMPLE_SHIFTING_NONE;
    ospi_struct->fifo_threshold    = OSPI_FIFO_THRESHOLD_5;
    ospi_struct->device_size       = OSPI_MESZ_4096_MBS;
    ospi_struct->wrap_size         = OSPI_DIRECT;
    ospi_struct->cs_hightime       = OSPI_CS_HIGH_TIME_3_CYCLE;
    ospi_struct->memory_type       = OSPI_MICRON_MODE;
    ospi_struct->delay_hold_cycle  = OSPI_DELAY_HOLD_NONE;

    /* initialize OSPI parameter */
    ospi_init(ospi_periph, ospi_struct);
    /* enable OSPI */
    ospi_enable(ospi_periph);
}

/*!
    \brief      read the flah id
    \param[in]  ospi_periph: OSPIx(x=0,1)
    \param[in]  ospi_struct: OSPI parameter initialization stuct members of the structure
                             and the member values are shown as below:
                prescaler: between 0 and 255
                fifo_threshold: OSPI_FIFO_THRESHOLD_x (x = 1, 2, ..., 31, 32)
                sample_shift: OSPI_SAMPLE_SHIFTING_NONE, OSPI_SAMPLE_SHIFTING_HALF_CYCLE
                device_size: OSPI_MESZ_x_BYTES (x = 2, 4, 8, ..., 512, 1024)
                             OSPI_MESZ_x_KBS (x = 2, 4, 8, ..., 512, 1024)
                             OSPI_MESZ_x_MBS (x = 2, 4, 8, ..., 2048, 4096)
                cs_hightime: OSPI_CS_HIGH_TIME_x_CYCLE (x = 1, 2, ..., 63, 64)
                memory_type: OSPI_MICRON_MODE, OSPI_MACRONIX_MODE, OSPI_STANDARD_MODE
                             OSPI_MACRONIX_RAM_MODE,
                wrap_size: OSPI_DIRECT, OSPI_WRAP_16BYTES, OSPI_WRAP_32BYTES
                           OSPI_WRAP_64BYTES, OSPI_WRAP_128BYTES
                delay_hold_cycle: OSPI_DELAY_HOLD_NONE, OSPI_DELAY_HOLD_QUARTER_CYCLE
    \retval     none
*/
uint32_t ospi_flash_read_id(uint32_t ospi_periph, ospi_parameter_struct *ospi_struct)
{
    uint8_t temp_id[4];
    ospi_regular_cmd_struct cmd_struct = {0};

    /* initialize read ID command */
    cmd_struct.operation_type          = OSPI_OPTYPE_COMMON_CFG;
    cmd_struct.instruction             = GD25LX512ME_READ_ID_CMD;

    cmd_struct.ins_mode                = OSPI_INSTRUCTION_1_LINE;
    cmd_struct.ins_size                = OSPI_INSTRUCTION_8_BITS;

    cmd_struct.addr_mode               = OSPI_ADDRESS_NONE;
    cmd_struct.addr_size               = OSPI_ADDRESS_24_BITS;
    cmd_struct.addr_dtr_mode           = OSPI_ADDRDTR_MODE_DISABLE;

    cmd_struct.alter_bytes_mode        = OSPI_ALTERNATE_BYTES_NONE;
    cmd_struct.alter_bytes_size        = OSPI_ALTERNATE_BYTES_24_BITS;
    cmd_struct.alter_bytes_dtr_mode    = OSPI_ABDTR_MODE_DISABLE;

    cmd_struct.data_mode               = OSPI_DATA_1_LINE;
    cmd_struct.data_dtr_mode           = OSPI_DADTR_MODE_DISABLE;

    cmd_struct.dummy_cycles            = OSPI_DUMYC_CYCLES_0;
    cmd_struct.nbdata = 4;

    /* configure the command */
    ospi_command_config(ospi_periph, ospi_struct, &cmd_struct);

    /* receive data */
    ospi_receive(ospi_periph, temp_id);
    return (temp_id[0] << 24) | (temp_id[1] << 16) | (temp_id[2] << 8) | temp_id[3];
}

/*!
    \brief      enable flash memory reset
    \param[in]  ospi_periph: OSPIx(x=0,1)
    \param[in]  ospi_struct: OSPI parameter initialization stuct members of the structure
                             and the member values are shown as below:
                prescaler: between 0 and 255
                fifo_threshold: OSPI_FIFO_THRESHOLD_x (x = 1, 2, ..., 31, 32)
                sample_shift: OSPI_SAMPLE_SHIFTING_NONE, OSPI_SAMPLE_SHIFTING_HALF_CYCLE
                device_size: OSPI_MESZ_x_BYTES (x = 2, 4, 8, ..., 512, 1024)
                             OSPI_MESZ_x_KBS (x = 2, 4, 8, ..., 512, 1024)
                             OSPI_MESZ_x_MBS (x = 2, 4, 8, ..., 2048, 4096)
                cs_hightime: OSPI_CS_HIGH_TIME_x_CYCLE (x = 1, 2, ..., 63, 64)
                memory_type: OSPI_MICRON_MODE, OSPI_MACRONIX_MODE, OSPI_STANDARD_MODE
                             OSPI_MACRONIX_RAM_MODE,
                wrap_size: OSPI_DIRECT, OSPI_WRAP_16BYTES, OSPI_WRAP_32BYTES
                           OSPI_WRAP_64BYTES, OSPI_WRAP_128BYTES
                delay_hold_cycle: OSPI_DELAY_HOLD_NONE, OSPI_DELAY_HOLD_QUARTER_CYCLE
    \param[in]  mode: flash interface mode
                only one parameter can be selected which is shown as below:
      \arg        SPI_MODE: SPI mode
      \arg        OSPI_MODE: OSPI mode
    \param[out] none
    \retval     none
*/
void ospi_flash_reset_enable(uint32_t ospi_periph, ospi_parameter_struct *ospi_struct, interface_mode mode)
{
    ospi_regular_cmd_struct cmd_struct = {0};

    /* initialize enable flash reset command */
    cmd_struct.operation_type          = OSPI_OPTYPE_COMMON_CFG;
    cmd_struct.instruction             = GD25LX512ME_RESET_ENABLE_CMD;

    /* initialize flash reset command */
    if(SPI_MODE == mode) {
        cmd_struct.ins_mode = OSPI_INSTRUCTION_1_LINE;
    } else {
        cmd_struct.ins_mode = OSPI_INSTRUCTION_8_LINES;
    }

    cmd_struct.ins_size                = OSPI_INSTRUCTION_8_BITS;

    cmd_struct.addr_mode               = OSPI_ADDRESS_NONE;
    cmd_struct.addr_size               = OSPI_ADDRESS_24_BITS;
    cmd_struct.addr_dtr_mode           = OSPI_ADDRDTR_MODE_DISABLE;

    cmd_struct.alter_bytes_mode        = OSPI_ALTERNATE_BYTES_NONE;
    cmd_struct.alter_bytes_size        = OSPI_ALTERNATE_BYTES_24_BITS;
    cmd_struct.alter_bytes_dtr_mode    = OSPI_ABDTR_MODE_DISABLE;

    cmd_struct.data_mode               = OSPI_DATA_NONE;
    cmd_struct.data_dtr_mode           = OSPI_DADTR_MODE_DISABLE;

    cmd_struct.dummy_cycles            = OSPI_DUMYC_CYCLES_0;
    cmd_struct.nbdata                  = 0;

    /* configure the command */
    ospi_command_config(ospi_periph, ospi_struct, &cmd_struct);
}

/*!
    \brief      reset the flash
    \param[in]  ospi_periph: OSPIx(x=0,1)
    \param[in]  ospi_struct: OSPI parameter initialization stuct members of the structure
                             and the member values are shown as below:
                prescaler: between 0 and 255
                fifo_threshold: OSPI_FIFO_THRESHOLD_x (x = 1, 2, ..., 31, 32)
                sample_shift: OSPI_SAMPLE_SHIFTING_NONE, OSPI_SAMPLE_SHIFTING_HALF_CYCLE
                device_size: OSPI_MESZ_x_BYTES (x = 2, 4, 8, ..., 512, 1024)
                             OSPI_MESZ_x_KBS (x = 2, 4, 8, ..., 512, 1024)
                             OSPI_MESZ_x_MBS (x = 2, 4, 8, ..., 2048, 4096)
                cs_hightime: OSPI_CS_HIGH_TIME_x_CYCLE (x = 1, 2, ..., 63, 64)
                memory_type: OSPI_MICRON_MODE, OSPI_MACRONIX_MODE, OSPI_STANDARD_MODE
                             OSPI_MACRONIX_RAM_MODE,
                wrap_size: OSPI_DIRECT, OSPI_WRAP_16BYTES, OSPI_WRAP_32BYTES
                           OSPI_WRAP_64BYTES, OSPI_WRAP_128BYTES
                delay_hold_cycle: OSPI_DELAY_HOLD_NONE, OSPI_DELAY_HOLD_QUARTER_CYCLE
    \param[in]  mode: flash interface mode
                only one parameter can be selected which is shown as below:
      \arg        SPI_MODE: SPI mode
      \arg        OSPI_MODE: OSPI mode
    \param[out] none
    \retval     none
*/
void ospi_flash_reset_memory(uint32_t ospi_periph, ospi_parameter_struct *ospi_struct, interface_mode mode)
{
    ospi_regular_cmd_struct cmd_struct = {0};

    /* initialize flash reset command */
    cmd_struct.operation_type          = OSPI_OPTYPE_COMMON_CFG;
    cmd_struct.instruction             = GD25LX512ME_RESET_MEMORY_CMD;

    /* initialize flash reset command */
    if(SPI_MODE == mode) {
        cmd_struct.ins_mode = OSPI_INSTRUCTION_1_LINE;
    } else {
        cmd_struct.ins_mode = OSPI_INSTRUCTION_8_LINES;
    }

    cmd_struct.ins_size                = OSPI_INSTRUCTION_8_BITS;

    cmd_struct.addr_mode               = OSPI_ADDRESS_NONE;
    cmd_struct.addr_size               = OSPI_ADDRESS_24_BITS;
    cmd_struct.addr_dtr_mode           = OSPI_ADDRDTR_MODE_DISABLE;

    cmd_struct.alter_bytes_mode        = OSPI_ALTERNATE_BYTES_NONE;
    cmd_struct.alter_bytes_size        = OSPI_ALTERNATE_BYTES_24_BITS;
    cmd_struct.alter_bytes_dtr_mode    = OSPI_ABDTR_MODE_DISABLE;

    cmd_struct.data_mode               = OSPI_DATA_NONE;
    cmd_struct.data_dtr_mode           = OSPI_DADTR_MODE_DISABLE;

    cmd_struct.dummy_cycles            = OSPI_DUMYC_CYCLES_0;
    cmd_struct.nbdata                  = 0;

    /* configure the command */
    ospi_command_config(ospi_periph, ospi_struct, &cmd_struct);
}

/*!
    \brief      polling WIP(write in progress) bit become to 0
    \param[in]  ospi_periph: OSPIx(x=0,1)
    \param[in]  ospi_struct: OSPI parameter initialization stuct members of the structure
                             and the member values are shown as below:
                prescaler: between 0 and 255
                fifo_threshold: OSPI_FIFO_THRESHOLD_x (x = 1, 2, ..., 31, 32)
                sample_shift: OSPI_SAMPLE_SHIFTING_NONE, OSPI_SAMPLE_SHIFTING_HALF_CYCLE
                device_size: OSPI_MESZ_x_BYTES (x = 2, 4, 8, ..., 512, 1024)
                             OSPI_MESZ_x_KBS (x = 2, 4, 8, ..., 512, 1024)
                             OSPI_MESZ_x_MBS (x = 2, 4, 8, ..., 2048, 4096)
                cs_hightime: OSPI_CS_HIGH_TIME_x_CYCLE (x = 1, 2, ..., 63, 64)
                memory_type: OSPI_MICRON_MODE, OSPI_MACRONIX_MODE, OSPI_STANDARD_MODE
                             OSPI_MACRONIX_RAM_MODE,
                wrap_size: OSPI_DIRECT, OSPI_WRAP_16BYTES, OSPI_WRAP_32BYTES
                           OSPI_WRAP_64BYTES, OSPI_WRAP_128BYTES
                delay_hold_cycle: OSPI_DELAY_HOLD_NONE, OSPI_DELAY_HOLD_QUARTER_CYCLE
    \param[in]  mode: flash interface mode
                only one parameter can be selected which is shown as below:
      \arg        SPI_MODE: SPI mode
      \arg        OSPI_MODE: OSPI mode
    \param[out] none
    \retval     none
*/
void ospi_flash_autopolling_memory_ready(uint32_t ospi_periph, ospi_parameter_struct *ospi_struct, interface_mode mode)
{
    ospi_regular_cmd_struct         cmd_struct = {0};
    ospi_autopolling_struct autopl_cfg_struct  = {0};

    /* initialize read status register command */
    if(SPI_MODE == mode) {
        cmd_struct.ins_mode = OSPI_INSTRUCTION_1_LINE;
        cmd_struct.addr_mode = OSPI_ADDRESS_NONE;
        cmd_struct.data_mode = OSPI_DATA_1_LINE;
        cmd_struct.dummy_cycles = OSPI_DUMYC_CYCLES_0;
    } else {
        cmd_struct.ins_mode = OSPI_INSTRUCTION_8_LINES;
        cmd_struct.addr_mode = OSPI_ADDRESS_8_LINES;
        cmd_struct.data_mode = OSPI_DATA_8_LINES;
        cmd_struct.dummy_cycles = OSPI_DUMYC_CYCLES_8;
    }

    cmd_struct.operation_type          = OSPI_OPTYPE_COMMON_CFG;
    cmd_struct.instruction             = GD25LX512ME_READ_STATUS_REG_CMD;

    cmd_struct.ins_size                = OSPI_INSTRUCTION_8_BITS;

    cmd_struct.addr_size               = OSPI_ADDRESS_24_BITS;
    cmd_struct.addr_dtr_mode           = OSPI_ADDRDTR_MODE_DISABLE;

    cmd_struct.alter_bytes_mode        = OSPI_ALTERNATE_BYTES_NONE;
    cmd_struct.alter_bytes_size        = OSPI_ALTERNATE_BYTES_24_BITS;
    cmd_struct.alter_bytes_dtr_mode    = OSPI_ABDTR_MODE_DISABLE;

    cmd_struct.data_dtr_mode           = OSPI_DADTR_MODE_DISABLE;

    cmd_struct.nbdata                  = 1;

    /* configure the command */
    ospi_command_config(ospi_periph, ospi_struct, &cmd_struct);

    autopl_cfg_struct.match            = 0U;
    autopl_cfg_struct.mask             = GD25LX512ME_SR_WIP;
    autopl_cfg_struct.match_mode       = OSPI_MATCH_MODE_AND;
    autopl_cfg_struct.interval         = GD25LX512ME_AUTOPOLLING_INTERVAL_TIME;
    autopl_cfg_struct.automatic_stop   = OSPI_AUTOMATIC_STOP_MATCH;
    /* configure the OSPI automatic polling mode */
    ospi_autopolling_mode(ospi_periph, ospi_struct, &autopl_cfg_struct);
}

/*!
    \brief      enable flash write
    \param[in]  ospi_periph: OSPIx(x=0,1)
    \param[in]  ospi_struct: OSPI parameter initialization stuct members of the structure
                             and the member values are shown as below:
                prescaler: between 0 and 255
                fifo_threshold: OSPI_FIFO_THRESHOLD_x (x = 1, 2, ..., 31, 32)
                sample_shift: OSPI_SAMPLE_SHIFTING_NONE, OSPI_SAMPLE_SHIFTING_HALF_CYCLE
                device_size: OSPI_MESZ_x_BYTES (x = 2, 4, 8, ..., 512, 1024)
                             OSPI_MESZ_x_KBS (x = 2, 4, 8, ..., 512, 1024)
                             OSPI_MESZ_x_MBS (x = 2, 4, 8, ..., 2048, 4096)
                cs_hightime: OSPI_CS_HIGH_TIME_x_CYCLE (x = 1, 2, ..., 63, 64)
                memory_type: OSPI_MICRON_MODE, OSPI_MACRONIX_MODE, OSPI_STANDARD_MODE
                             OSPI_MACRONIX_RAM_MODE,
                wrap_size: OSPI_DIRECT, OSPI_WRAP_16BYTES, OSPI_WRAP_32BYTES
                           OSPI_WRAP_64BYTES, OSPI_WRAP_128BYTES
                delay_hold_cycle: OSPI_DELAY_HOLD_NONE, OSPI_DELAY_HOLD_QUARTER_CYCLE
    \param[out] none
    \retval     none
*/
void ospi_flash_write_enable(uint32_t ospi_periph, ospi_parameter_struct *ospi_struct)
{
    ospi_regular_cmd_struct cmd_struct = {0};
    ospi_autopolling_struct autopl_cfg_struct   = {0};

    /* initialize write enable command */
    cmd_struct.operation_type          = OSPI_OPTYPE_COMMON_CFG;
    cmd_struct.instruction             = GD25LX512ME_WRITE_ENABLE_CMD;

    cmd_struct.ins_mode                = OSPI_INSTRUCTION_1_LINE;
    cmd_struct.ins_size                = OSPI_INSTRUCTION_8_BITS;

    cmd_struct.addr_mode               = OSPI_ADDRESS_NONE;
    cmd_struct.addr_size               = OSPI_ADDRESS_24_BITS;
    cmd_struct.addr_dtr_mode           = OSPI_ADDRDTR_MODE_DISABLE;

    cmd_struct.alter_bytes_mode        = OSPI_ALTERNATE_BYTES_NONE;
    cmd_struct.alter_bytes_size        = OSPI_ALTERNATE_BYTES_24_BITS;
    cmd_struct.alter_bytes_dtr_mode    = OSPI_ABDTR_MODE_DISABLE;

    cmd_struct.data_mode               = OSPI_DATA_NONE;
    cmd_struct.data_dtr_mode           = OSPI_DADTR_MODE_DISABLE;

    cmd_struct.dummy_cycles            = OSPI_DUMYC_CYCLES_0;
    cmd_struct.nbdata                  = 0;

    /* configure the command */
    ospi_command_config(ospi_periph, ospi_struct, &cmd_struct);

    /* Configure automatic polling mode to wait for write enabling */
    cmd_struct.instruction              = GD25LX512ME_READ_STATUS_REG_CMD;
    cmd_struct.data_mode                = OSPI_DATA_1_LINE;
    cmd_struct.data_dtr_mode            = OSPI_DADTR_MODE_DISABLE;
    cmd_struct.dummy_cycles             = OSPI_DUMYC_CYCLES_0;
    cmd_struct.nbdata                   = 1U;

    /* configure the command */
    ospi_command_config(ospi_periph, ospi_struct, &cmd_struct);

    autopl_cfg_struct.match            = 2U;
    autopl_cfg_struct.mask             = 2U;
    autopl_cfg_struct.match_mode       = OSPI_MATCH_MODE_AND;
    autopl_cfg_struct.interval         = GD25LX512ME_AUTOPOLLING_INTERVAL_TIME;
    autopl_cfg_struct.automatic_stop   = OSPI_AUTOMATIC_STOP_MATCH;

    ospi_autopolling_mode(ospi_periph, ospi_struct, &autopl_cfg_struct);
}

/*!
    \brief      erase flash sector
    \param[in]  ospi_periph: OSPIx(x=0,1)
    \param[in]  ospi_struct: OSPI parameter initialization stuct members of the structure
                             and the member values are shown as below:
                prescaler: between 0 and 255
                fifo_threshold: OSPI_FIFO_THRESHOLD_x (x = 1, 2, ..., 31, 32)
                sample_shift: OSPI_SAMPLE_SHIFTING_NONE, OSPI_SAMPLE_SHIFTING_HALF_CYCLE
                device_size: OSPI_MESZ_x_BYTES (x = 2, 4, 8, ..., 512, 1024)
                             OSPI_MESZ_x_KBS (x = 2, 4, 8, ..., 512, 1024)
                             OSPI_MESZ_x_MBS (x = 2, 4, 8, ..., 2048, 4096)
                cs_hightime: OSPI_CS_HIGH_TIME_x_CYCLE (x = 1, 2, ..., 63, 64)
                memory_type: OSPI_MICRON_MODE, OSPI_MACRONIX_MODE, OSPI_STANDARD_MODE
                             OSPI_MACRONIX_RAM_MODE,
                wrap_size: OSPI_DIRECT, OSPI_WRAP_16BYTES, OSPI_WRAP_32BYTES
                           OSPI_WRAP_64BYTES, OSPI_WRAP_128BYTES
                delay_hold_cycle: OSPI_DELAY_HOLD_NONE, OSPI_DELAY_HOLD_QUARTER_CYCLE
    \param[in]  sector_addr: sector address to erase
    \param[out] none
    \retval     none
*/
void ospi_flash_sector_erase(uint32_t ospi_periph, ospi_parameter_struct *ospi_struct, uint32_t sector_addr)
{
    ospi_regular_cmd_struct cmd_struct = {0};

    /* initialize erase sector command */
    cmd_struct.operation_type          = OSPI_OPTYPE_COMMON_CFG;
    cmd_struct.instruction             = GD25LX512ME_SECTOR_ERASE_4K_CMD;

    cmd_struct.ins_mode                = OSPI_INSTRUCTION_1_LINE;
    cmd_struct.ins_size                = OSPI_INSTRUCTION_8_BITS;

    cmd_struct.address                 = sector_addr;
    cmd_struct.addr_mode               = OSPI_ADDRESS_1_LINE;
    cmd_struct.addr_size               = OSPI_ADDRESS_24_BITS;
    cmd_struct.addr_dtr_mode           = OSPI_ADDRDTR_MODE_DISABLE;

    cmd_struct.alter_bytes_mode        = OSPI_ALTERNATE_BYTES_NONE;
    cmd_struct.alter_bytes_size        = OSPI_ALTERNATE_BYTES_24_BITS;
    cmd_struct.alter_bytes_dtr_mode    = OSPI_ABDTR_MODE_DISABLE;

    cmd_struct.data_mode               = OSPI_DATA_NONE;
    cmd_struct.data_dtr_mode           = OSPI_DADTR_MODE_DISABLE;

    cmd_struct.dummy_cycles            = OSPI_DUMYC_CYCLES_0;
    cmd_struct.nbdata                  = 0;

    /* configure the command */
    ospi_command_config(ospi_periph, ospi_struct, &cmd_struct);
}

/*!
    \brief      program flash page
    \param[in]  ospi_periph: OSPIx(x=0,1)
    \param[in]  ospi_struct: OSPI parameter initialization stuct members of the structure
                             and the member values are shown as below:
                prescaler: between 0 and 255
                fifo_threshold: OSPI_FIFO_THRESHOLD_x (x = 1, 2, ..., 31, 32)
                sample_shift: OSPI_SAMPLE_SHIFTING_NONE, OSPI_SAMPLE_SHIFTING_HALF_CYCLE
                device_size: OSPI_MESZ_x_BYTES (x = 2, 4, 8, ..., 512, 1024)
                             OSPI_MESZ_x_KBS (x = 2, 4, 8, ..., 512, 1024)
                             OSPI_MESZ_x_MBS (x = 2, 4, 8, ..., 2048, 4096)
                cs_hightime: OSPI_CS_HIGH_TIME_x_CYCLE (x = 1, 2, ..., 63, 64)
                memory_type: OSPI_MICRON_MODE, OSPI_MACRONIX_MODE, OSPI_STANDARD_MODE
                             OSPI_MACRONIX_RAM_MODE,
                wrap_size: OSPI_DIRECT, OSPI_WRAP_16BYTES, OSPI_WRAP_32BYTES
                           OSPI_WRAP_64BYTES, OSPI_WRAP_128BYTES
                delay_hold_cycle: OSPI_DELAY_HOLD_NONE, OSPI_DELAY_HOLD_QUARTER_CYCLE
    \param[in]  write_addr: start address to write
    \param[in]  datasize: size of data to write
    \param[in]  pdata: pointer to data to be written
    \param[out] none
    \retval     none
*/
void ospi_flash_page_program(uint32_t ospi_periph, ospi_parameter_struct *ospi_struct, uint32_t write_addr, uint32_t datasize, uint8_t *pdata)
{
    ospi_regular_cmd_struct cmd_struct = {0};

    /* initialize page program command */
    cmd_struct.operation_type          = OSPI_OPTYPE_COMMON_CFG;
    cmd_struct.instruction             = GD25LX512ME_PAGE_PROG_CMD;

    cmd_struct.ins_mode                = OSPI_INSTRUCTION_1_LINE;
    cmd_struct.ins_size                = OSPI_INSTRUCTION_8_BITS;

    cmd_struct.address                 = write_addr;
    cmd_struct.addr_mode               = OSPI_ADDRESS_1_LINE;
    cmd_struct.addr_size               = OSPI_ADDRESS_24_BITS;
    cmd_struct.addr_dtr_mode           = OSPI_ADDRDTR_MODE_DISABLE;

    cmd_struct.alter_bytes_mode        = OSPI_ALTERNATE_BYTES_NONE;
    cmd_struct.alter_bytes_size        = OSPI_ALTERNATE_BYTES_24_BITS;
    cmd_struct.alter_bytes_dtr_mode    = OSPI_ABDTR_MODE_DISABLE;

    cmd_struct.data_mode               = OSPI_DATA_1_LINE;
    cmd_struct.data_dtr_mode           = OSPI_DADTR_MODE_DISABLE;

    cmd_struct.dummy_cycles            = OSPI_DUMYC_CYCLES_0;
    cmd_struct.nbdata                  = datasize;

    /* configure the command */
    ospi_command_config(ospi_periph, ospi_struct, &cmd_struct);

    /* transmit the data */
    ospi_transmit(ospi_periph, pdata);
}

/*!
    \brief      read flash data
    \param[in]  ospi_periph: OSPIx(x=0,1)
    \param[in]  ospi_struct: OSPI parameter initialization stuct members of the structure
                             and the member values are shown as below:
                prescaler: between 0 and 255
                fifo_threshold: OSPI_FIFO_THRESHOLD_x (x = 1, 2, ..., 31, 32)
                sample_shift: OSPI_SAMPLE_SHIFTING_NONE, OSPI_SAMPLE_SHIFTING_HALF_CYCLE
                device_size: OSPI_MESZ_x_BYTES (x = 2, 4, 8, ..., 512, 1024)
                             OSPI_MESZ_x_KBS (x = 2, 4, 8, ..., 512, 1024)
                             OSPI_MESZ_x_MBS (x = 2, 4, 8, ..., 2048, 4096)
                cs_hightime: OSPI_CS_HIGH_TIME_x_CYCLE (x = 1, 2, ..., 63, 64)
                memory_type: OSPI_MICRON_MODE, OSPI_MACRONIX_MODE, OSPI_STANDARD_MODE
                             OSPI_MACRONIX_RAM_MODE,
                wrap_size: OSPI_DIRECT, OSPI_WRAP_16BYTES, OSPI_WRAP_32BYTES
                           OSPI_WRAP_64BYTES, OSPI_WRAP_128BYTES
                delay_hold_cycle: OSPI_DELAY_HOLD_NONE, OSPI_DELAY_HOLD_QUARTER_CYCLE
    \param[in]  read_addr: start address to read
    \param[in]  datasize: size of data to write
    \param[in]  pdata: pointer to data to read
    \param[out] none
    \retval     none
*/
void ospi_flash_data_read(uint32_t ospi_periph, ospi_parameter_struct *ospi_struct, uint32_t read_addr, uint32_t datasize, uint8_t *pdata)
{
    ospi_regular_cmd_struct cmd_struct = {0};

    /* initialize flash read command */
    cmd_struct.operation_type          = OSPI_OPTYPE_COMMON_CFG;
    cmd_struct.instruction             = GD25LX512ME_FAST_READ_CMD;

    cmd_struct.ins_mode                = OSPI_INSTRUCTION_1_LINE;
    cmd_struct.ins_size                = OSPI_INSTRUCTION_8_BITS;

    cmd_struct.address                 = read_addr;
    cmd_struct.addr_mode               = OSPI_ADDRESS_1_LINE;
    cmd_struct.addr_size               = OSPI_ADDRESS_24_BITS;
    cmd_struct.addr_dtr_mode           = OSPI_ADDRDTR_MODE_DISABLE;

    cmd_struct.alter_bytes_mode        = OSPI_ALTERNATE_BYTES_NONE;
    cmd_struct.alter_bytes_size        = OSPI_ALTERNATE_BYTES_24_BITS;
    cmd_struct.alter_bytes_dtr_mode    = OSPI_ABDTR_MODE_DISABLE;

    cmd_struct.data_mode               = OSPI_DATA_1_LINE;
    cmd_struct.data_dtr_mode           = OSPI_DADTR_MODE_DISABLE;

    cmd_struct.dummy_cycles            = OSPI_DUMYC_CYCLES_8;
    cmd_struct.nbdata                  = datasize;

    /* configure the command */
    ospi_command_config(ospi_periph, ospi_struct, &cmd_struct);

    /* receive the data */
    ospi_receive(ospi_periph, pdata);
}


/*!
    \brief      write flash volatile configuration register
    \param[in]  ospi_periph: OSPIx(x=0,1)
    \param[in]  ospi_struct: OSPI parameter initialization stuct members of the structure
                             and the member values are shown as below:
                prescaler: between 0 and 255
                fifo_threshold: OSPI_FIFO_THRESHOLD_x (x = 1, 2, ..., 31, 32)
                sample_shift: OSPI_SAMPLE_SHIFTING_NONE, OSPI_SAMPLE_SHIFTING_HALF_CYCLE
                device_size: OSPI_MESZ_x_BYTES (x = 2, 4, 8, ..., 512, 1024)
                             OSPI_MESZ_x_KBS (x = 2, 4, 8, ..., 512, 1024)
                             OSPI_MESZ_x_MBS (x = 2, 4, 8, ..., 2048, 4096)
                cs_hightime: OSPI_CS_HIGH_TIME_x_CYCLE (x = 1, 2, ..., 63, 64)
                memory_type: OSPI_MICRON_MODE, OSPI_MACRONIX_MODE, OSPI_STANDARD_MODE
                             OSPI_MACRONIX_RAM_MODE,
                wrap_size: OSPI_DIRECT, OSPI_WRAP_16BYTES, OSPI_WRAP_32BYTES
                           OSPI_WRAP_64BYTES, OSPI_WRAP_128BYTES
                delay_hold_cycle: OSPI_DELAY_HOLD_NONE, OSPI_DELAY_HOLD_QUARTER_CYCLE
    \param[in]  write_addr: start address to write
    \param[in]  value: value to write to configuration register
    \param[out] none
    \retval     none
*/
void ospi_flash_write_volatile_cfg_reg(uint32_t ospi_periph, ospi_parameter_struct *ospi_struct, uint32_t write_addr, uint8_t value)
{
    ospi_regular_cmd_struct cmd_struct = {0};

    /* initialize the write enable for volatile status register command */
    cmd_struct.operation_type          = OSPI_OPTYPE_COMMON_CFG;
    cmd_struct.instruction             = GD25LX512ME_WRITE_ENABLE_VOLATILE_STATUS_CFG_CMD;

    cmd_struct.ins_mode                = OSPI_INSTRUCTION_1_LINE;
    cmd_struct.ins_size                = OSPI_INSTRUCTION_8_BITS;

    cmd_struct.address                 = 0U;
    cmd_struct.addr_mode               = OSPI_ADDRESS_NONE;
    cmd_struct.addr_size               = OSPI_ADDRESS_24_BITS;
    cmd_struct.addr_dtr_mode           = OSPI_ADDRDTR_MODE_DISABLE;

    cmd_struct.alter_bytes_mode        = OSPI_ALTERNATE_BYTES_NONE;
    cmd_struct.alter_bytes_size        = OSPI_ALTERNATE_BYTES_24_BITS;
    cmd_struct.alter_bytes_dtr_mode    = OSPI_ABDTR_MODE_DISABLE;

    cmd_struct.data_mode               = OSPI_DATA_NONE;
    cmd_struct.data_dtr_mode           = OSPI_DADTR_MODE_DISABLE;

    cmd_struct.dummy_cycles            = OSPI_DUMYC_CYCLES_0;
    cmd_struct.nbdata                  = 0;

    /* configure the command */
    ospi_command_config(ospi_periph, ospi_struct, &cmd_struct);

    /* Initialize the writing of volatile configuration register */
    cmd_struct.operation_type          = OSPI_OPTYPE_COMMON_CFG;
    cmd_struct.ins_mode                = OSPI_INSTRUCTION_1_LINE;
    cmd_struct.ins_size                = OSPI_INSTRUCTION_8_BITS;
    cmd_struct.instruction             = GD25LX512ME_WRITE_VOLATILE_CFG_REG_CMD;
    cmd_struct.addr_mode               = OSPI_ADDRESS_1_LINE;
    cmd_struct.addr_dtr_mode           = OSPI_ADDRDTR_MODE_DISABLE;
    cmd_struct.addr_size               = OSPI_ADDRESS_24_BITS;
    cmd_struct.address                 = write_addr;
    cmd_struct.alter_bytes_mode        = OSPI_ALTERNATE_BYTES_NONE;
    cmd_struct.data_mode               = OSPI_DATA_1_LINE;
    cmd_struct.data_dtr_mode           = OSPI_DADTR_MODE_DISABLE;
    cmd_struct.dummy_cycles            = 0U;
    cmd_struct.nbdata                  = 1U;

    /* configure the command */
    ospi_command_config(ospi_periph, ospi_struct, &cmd_struct);

    /* transmit the data */
    ospi_transmit(ospi_periph, &value);
}

/*!
    \brief      enable memory mapped mode for the OSPI memory
    \param[in]  ospi_periph: OSPIx(x=0,1)
    \param[in]  ospi_struct: OSPI parameter initialization stuct members of the structure
                             and the member values are shown as below:
                prescaler: between 0 and 255
                fifo_threshold: OSPI_FIFO_THRESHOLD_x (x = 1, 2, ..., 31, 32)
                sample_shift: OSPI_SAMPLE_SHIFTING_NONE, OSPI_SAMPLE_SHIFTING_HALF_CYCLE
                device_size: OSPI_MESZ_x_BYTES (x = 2, 4, 8, ..., 512, 1024)
                             OSPI_MESZ_x_KBS (x = 2, 4, 8, ..., 512, 1024)
                             OSPI_MESZ_x_MBS (x = 2, 4, 8, ..., 2048, 4096)
                cs_hightime: OSPI_CS_HIGH_TIME_x_CYCLE (x = 1, 2, ..., 63, 64)
                memory_type: OSPI_MICRON_MODE, OSPI_MACRONIX_MODE, OSPI_STANDARD_MODE
                             OSPI_MACRONIX_RAM_MODE,
                wrap_size: OSPI_DIRECT, OSPI_WRAP_16BYTES, OSPI_WRAP_32BYTES
                           OSPI_WRAP_64BYTES, OSPI_WRAP_128BYTES
                delay_hold_cycle: OSPI_DELAY_HOLD_NONE, OSPI_DELAY_HOLD_QUARTER_CYCLE
    \param[out] none
    \retval     none
*/
void ospi_memory_map_enable(uint32_t ospi_periph, ospi_parameter_struct *ospi_struct)
{
    ospi_regular_cmd_struct cmd_struct = {0};

    /* initialize read command */
    cmd_struct.operation_type          = OSPI_OPTYPE_WRAP_CFG;
    cmd_struct.instruction             = GD25LX512ME_OCTAL_IO_FAST_READ_CMD;

    cmd_struct.ins_mode                = OSPI_INSTRUCTION_8_LINES;
    cmd_struct.ins_size                = OSPI_INSTRUCTION_8_BITS;

    cmd_struct.address                 = 0U;
    cmd_struct.addr_mode               = OSPI_ADDRESS_8_LINES;
    cmd_struct.addr_size               = OSPI_ADDRESS_24_BITS;

    cmd_struct.alter_bytes_mode        = OSPI_ALTERNATE_BYTES_NONE;
    cmd_struct.alter_bytes_size        = OSPI_ALTERNATE_BYTES_24_BITS;
    cmd_struct.alter_bytes_dtr_mode    = OSPI_ABDTR_MODE_DISABLE;

    cmd_struct.data_mode               = OSPI_DATA_8_LINES;
    cmd_struct.data_dtr_mode           = OSPI_DADTR_MODE_DISABLE;

    cmd_struct.dummy_cycles            = OSPI_DUMYC_CYCLES_16;
    cmd_struct.nbdata                  = 0;

    /* configure the command */
    ospi_command_config(ospi_periph, ospi_struct, &cmd_struct);

    /* initialize the program command */
    cmd_struct.operation_type          = OSPI_OPTYPE_WRITE_CFG;
    cmd_struct.instruction             = GD25LX512ME_EXT_OCTAL_PAGE_PROG_CMD;
    cmd_struct.dummy_cycles            = 0U;

    /* configure the command */
    ospi_command_config(ospi_periph, ospi_struct, &cmd_struct);

    while(RESET != ospi_flag_get(ospi_periph, OSPI_FLAG_BUSY)) {
    }
    ospi_functional_mode_config(ospi_periph, OSPI_MEMORY_MAPPED);
}
