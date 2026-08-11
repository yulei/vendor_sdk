/*!
    \file    main.c
    \brief   MDMA software trigger link list transfer example

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
#define MDMA_LINK_LIST_NODE_ADDR      ((uint32_t)0x24020000U)
#define BLOCK_TRANSFER_LENGTH         (256U)
#define BLOCK_REPEATED_COUNT          (3U)
#define BUFFER_SIZE                   (8U)

__attribute__ ((aligned(32))) uint32_t mdma_source_addr[BUFFER_SIZE];
__attribute__ ((aligned(32))) uint32_t mdma_dest_addr[BUFFER_SIZE];
ErrStatus status;

void cache_enable(void);
void mdma_config(void);
void mdma_link_list_create(void);
ErrStatus memory_compare(uint8_t *src, uint8_t *dst, uint16_t length);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    uint32_t i;

    /* enable the CPU cache */
    cache_enable();
    /* enable MDMA clock */
    rcu_periph_clock_enable(RCU_MDMA);

    /* initialize LED */
    gd_eval_led_init(LED1);
    /* configure MDMA source and destination address */
    for(i = 0; i < 4; i++) {
        mdma_source_addr[i] = AXIRAM_ADDR + i * BLOCK_TRANSFER_LENGTH * BLOCK_REPEATED_COUNT;
        mdma_dest_addr[i] = DTCM_RAM_ADDR + i * BLOCK_TRANSFER_LENGTH * BLOCK_REPEATED_COUNT;
    }
    /* configure MDMA channel 0 */
    mdma_config();
    /* create MDMA link list nodes and connect them to MDMA channel 0 */
    mdma_link_list_create();
    /* clean cache */
    SCB_CleanDCache();
    /* enable the MDMA channel 0 */
    mdma_channel_enable(MDMA_CH0);
    /* generate software request */
    mdma_channel_software_request_enable(MDMA_CH0);
    /* check MDMA channel actived status */
    while(!mdma_flag_get(MDMA_CH0, MDMA_FLAG_REQAF));

    /* wait MDMA channel 0 transfer done */
    while(!mdma_flag_get(MDMA_CH0, MDMA_FLAG_CHTCF));
    mdma_flag_clear(MDMA_CH0, MDMA_FLAG_CHTCF);

    /* wait request actived status cleared by hardware */
    while(mdma_flag_get(MDMA_CH0, MDMA_FLAG_REQAF));
    /* compare mdma_dest_addr data with mdma_source_addr data */
    status = memory_compare((uint8_t *)mdma_source_addr[0], (uint8_t *)mdma_dest_addr[0], BLOCK_TRANSFER_LENGTH * BLOCK_REPEATED_COUNT * 4);

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
    /* select complete transfer mode */
    mdma_init_struct.trans_trig_mode     = MDMA_COMPLETE_TRANSFER;
    mdma_init_struct.priority            = MDMA_PRIORITY_HIGH;
    mdma_init_struct.endianness          = MDMA_LITTLE_ENDIANNESS;
    mdma_init_struct.source_addr         = mdma_source_addr[0];
    mdma_init_struct.destination_addr    = mdma_dest_addr[0];
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
    \brief      create MDMA link list
    \param[in]  none
    \param[out] none
    \retval     none
*/
void mdma_link_list_create(void)
{
    mdma_parameter_struct mdma_init_struct;
    mdma_multi_block_parameter_struct block_init_struct;

    mdma_link_node_parameter_struct *pnode0   = (mdma_link_node_parameter_struct *)MDMA_LINK_LIST_NODE_ADDR;
    mdma_link_node_parameter_struct *pnode1   = pnode0 + 1;
    mdma_link_node_parameter_struct *pnode2   = pnode0 + 2;
    mdma_link_node_parameter_struct *pre_node = NULL;

    /* initialize MDMA parameter structure */
    mdma_para_struct_init(&mdma_init_struct);
    mdma_init_struct.request             = MDMA_REQUEST_SW;
    mdma_init_struct.trans_trig_mode     = MDMA_COMPLETE_TRANSFER;
    mdma_init_struct.priority            = MDMA_PRIORITY_HIGH;
    mdma_init_struct.endianness          = MDMA_LITTLE_ENDIANNESS;
    mdma_init_struct.source_addr         = mdma_source_addr[1];
    mdma_init_struct.destination_addr    = mdma_dest_addr[1];
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
    mdma_init_struct.mask_addr           = 0U;
    mdma_init_struct.mask_data           = 0U;

    /* initialize MDMA multi block transfer parameter structure */
    mdma_multi_block_para_struct_init(&block_init_struct);
    block_init_struct.block_num          = BLOCK_REPEATED_COUNT - 1;
    block_init_struct.saddr_update_val   = 0U;
    block_init_struct.dstaddr_update_val = 0U;

    /* create MDMA link list node 0: transfer data from mdma_source_addr to mdma_dest_addr */
    mdma_node_create(pnode0, &block_init_struct, &mdma_init_struct);
    /* make pre_node point to configure register of MDMA channel 0 */
    pre_node = (mdma_link_node_parameter_struct *)&MDMA_CHXCFG(0);
    /* add link list node 0 to MDMA channel 0 link list */
    mdma_node_add(pre_node, pnode0);

    /* link list node 1: transfer data from mdma_source_addr[2] to mdma_dest_addr[2] */
    mdma_init_struct.source_addr         = mdma_source_addr[2];
    mdma_init_struct.destination_addr    = mdma_dest_addr[2];

    /* create link list node 1 */
    mdma_node_create(pnode1, &block_init_struct, &mdma_init_struct);
    /* add link list node 1 to link list */
    mdma_node_add(pnode0, pnode1);

    /* link list node 2: transfer data from mdma_source_addr[3] to mdma_dest_addr[3] */
    mdma_init_struct.source_addr         = mdma_source_addr[3];
    mdma_init_struct.destination_addr    = mdma_dest_addr[3];

    /* create link list node 2 */
    mdma_node_create(pnode2, &block_init_struct, &mdma_init_struct);
    /* add link list node 2 to link list */
    mdma_node_add(pnode1, pnode2);
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
