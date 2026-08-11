/*!
    \file    main.c
    \brief   MDMA software trigger multi block transfer example

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
#include "gd32h759i_eval.h"

#define DTCM_RAM_ADDR                 ((uint32_t)0x20000000U)
#define AXIRAM_ADDR                   ((uint32_t)0x24010000U)
#define BLOCK_TRANSFER_LENGTH         (256U)
#define BLOCK_REPEATED_COUNT          (4U)

uint32_t mdma_source_addr = AXIRAM_ADDR;
uint32_t mdma_dest_addr = DTCM_RAM_ADDR;
ErrStatus status;
__IO FlagStatus g_transfer_complete = RESET;

void cache_enable(void);
void nvic_config(void);
void mdma_config(void);
ErrStatus memory_compare(uint8_t *src, uint8_t *dst, uint16_t length);

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
    /* enable MDMA clock */
    rcu_periph_clock_enable(RCU_MDMA);

    /* initialize LED */
    gd_eval_led_init(LED1);
    /* configure MDMA interrupt */
    nvic_config();
    /* configure MDMA */
    mdma_config();

    /* enable multi-block transfer complete interrupt */
    mdma_interrupt_enable(MDMA_CH0, MDMA_INT_MBTC);
    /* clean cache */
    SCB_CleanDCache();
    /* enable the MDMA channel0 */
    mdma_channel_enable(MDMA_CH0);
    /* generate software request */
    mdma_channel_software_request_enable(MDMA_CH0);

    /* waiting for the transfer to complete */
    while(RESET == g_transfer_complete) {
    }

    /* compare mdma_dest_addr data with mdma_source_addr data */
    status = memory_compare((uint8_t *)mdma_source_addr, (uint8_t *)mdma_dest_addr, BLOCK_TRANSFER_LENGTH * BLOCK_REPEATED_COUNT);

    if(SUCCESS == status) {
        gd_eval_led_on(LED1);
    } else {
        gd_eval_led_off(LED1);
    }

    while(1) {
    }
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

/*!
    \brief      configure MDMA interrupt
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_config(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);
    nvic_irq_enable(MDMA_IRQn, 0, 0);
}

/*!
    \brief      configure MDMA
    \param[in]  none
    \param[out] none
    \retval     none
*/
void mdma_config(void)
{
    mdma_parameter_struct mdma_init_struct;
    mdma_multi_block_parameter_struct block_init_struct;

    /* initialize MDMA parameter structure */
    mdma_para_struct_init(&mdma_init_struct);
    /* configure MDMA trigger source as software trigger */
    mdma_init_struct.request             = MDMA_REQUEST_SW;
    mdma_init_struct.trans_trig_mode     = MDMA_MULTI_BLOCK_TRANSFER;
    mdma_init_struct.priority            = MDMA_PRIORITY_HIGH;
    mdma_init_struct.endianness          = MDMA_LITTLE_ENDIANNESS;
    mdma_init_struct.source_addr         = mdma_source_addr;
    mdma_init_struct.destination_addr    = mdma_dest_addr;
    mdma_init_struct.source_inc          = MDMA_SOURCE_INCREASE_8BIT;
    mdma_init_struct.dest_inc            = MDMA_DESTINATION_INCREASE_8BIT;
    mdma_init_struct.source_data_size    = MDMA_SOURCE_DATASIZE_8BIT;
    mdma_init_struct.dest_data_size      = MDMA_DESTINATION_DATASIZE_8BIT;
    mdma_init_struct.source_burst        = MDMA_SOURCE_BURST_SINGLE;
    mdma_init_struct.dest_burst          = MDMA_DESTINATION_BURST_SINGLE;
    /* configure bus type for source and destination */
    mdma_init_struct.source_bus          = MDMA_SOURCE_AXI;
    mdma_init_struct.destination_bus     = MDMA_DESTINATION_AHB_TCM;
    /* configure data alignment and pack mode */
    mdma_init_struct.data_alignment      = MDMA_DATAALIGN_PKEN;
    /* configure buffer transfer length and transfer byte count */
    mdma_init_struct.buff_trans_len      = 0x40-1;
    mdma_init_struct.tbytes_num_in_block = BLOCK_TRANSFER_LENGTH;
    /* no need to configure mask address and mask data in software request mode */
    mdma_init_struct.mask_addr           = 0U;
    mdma_init_struct.mask_data           = 0U;
    /* configure MDMA channel 0 */
    mdma_init(MDMA_CH0, &mdma_init_struct);

    /* initialize MDMA multi block transfer parameter structure */
    mdma_multi_block_para_struct_init(&block_init_struct);
    block_init_struct.block_num          = BLOCK_REPEATED_COUNT - 1;
    block_init_struct.saddr_update_val   = 0;
    block_init_struct.dstaddr_update_val = 0;
    /* configure block transfer configure register */
    mdma_multi_block_mode_config(MDMA_CH0, BLOCK_TRANSFER_LENGTH, &block_init_struct);
}

/*!
    \brief      memory compare function
    \param[in]  src : source data
    \param[in]  dst : destination data
    \param[in]  length : the compare data length
    \param[out] none
    \retval     ErrStatus : ERROR or SUCCESS
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
