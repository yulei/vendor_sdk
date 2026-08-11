/*!
    \file    emmc.c
    \brief   eMMC driver

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

#include <stddef.h>
#include <string.h>
#include "emmc.h"
#include "gd32h73x_75x_sdio.h"

/* card status of R1 definitions */
#define EMMC_R1_OUT_OF_RANGE                  BIT(31)                   /* command's argument was out of the allowed range */
#define EMMC_R1_ADDRESS_ERROR                 BIT(30)                   /* misaligned address which did not match the block length */
#define EMMC_R1_BLOCK_LEN_ERROR               BIT(29)                   /* transferred block length is not allowed */
#define EMMC_R1_ERASE_SEQ_ERROR               BIT(28)                   /* an error in the sequence of erase commands occurred */
#define EMMC_R1_ERASE_PARAM                   BIT(27)                   /* an invalid selection of write-blocks for erase occurred */
#define EMMC_R1_WP_VIOLATION                  BIT(26)                   /* the host attempts to write to a protected block or to the temporary or permanent write protected card */
#define EMMC_R1_CARD_IS_LOCKED                BIT(25)                   /* the card is locked by the host */
#define EMMC_R1_LOCK_UNLOCK_FAILED            BIT(24)                   /* a sequence or password error has been detected in lock/unlock card command */
#define EMMC_R1_COM_CRC_ERROR                 BIT(23)                   /* CRC check of the previous command failed */
#define EMMC_R1_ILLEGAL_COMMAND               BIT(22)                   /* command not legal for the card state */
#define EMMC_R1_CARD_ECC_FAILED               BIT(21)                   /* card internal ECC was applied but failed to correct the data */
#define EMMC_R1_CC_ERROR                      BIT(20)                   /* internal card controller error */
#define EMMC_R1_GENERAL_UNKNOWN_ERROR         BIT(19)                   /* a general or an unknown error occurred during the operation */
#define EMMC_R1_CSD_OVERWRITE                 BIT(16)                   /* read only section of the CSD does not match or attempt to reverse the copy or permanent WP bits */
#define EMMC_R1_WP_ERASE_SKIP                 BIT(15)                   /* partial address space was erased */
#define EMMC_R1_CARD_ECC_DISABLED             BIT(14)                   /* command has been executed without using the internal ECC */
#define EMMC_R1_ERASE_RESET                   BIT(13)                   /* an erase sequence was cleared before executing */
#define EMMC_R1_READY_FOR_DATA                BIT(8)                    /* correspond to buffer empty signaling on the bus */
#define EMMC_R1_APP_CMD                       BIT(5)                    /* card will expect ACMD */
#define EMMC_R1_AKE_SEQ_ERROR                 BIT(3)                    /* error in the sequence of the authentication process */
#define EMMC_R1_ERROR_BITS                    ((uint32_t)0xFDF9E008U)   /* all the R1 error bits */

/* card status of R6 definitions */
#define EMMC_R6_COM_CRC_ERROR                 BIT(15)                   /* CRC check of the previous command failed */
#define EMMC_R6_ILLEGAL_COMMAND               BIT(14)                   /* command not legal for the card state */
#define EMMC_R6_GENERAL_UNKNOWN_ERROR         BIT(13)                   /* a general or an unknown error occurred during the operation */

/* card state */
#define EMMC_CARDSTATE_IDLE                   ((uint8_t)0x00)           /* eMMC is in idle state */
#define EMMC_CARDSTATE_READY                  ((uint8_t)0x01)           /* eMMC is in ready state */
#define EMMC_CARDSTATE_IDENTIFICATION         ((uint8_t)0x02)           /* eMMC is in identification state */
#define EMMC_CARDSTATE_STANDBY                ((uint8_t)0x03)           /* eMMC is in standby state */
#define EMMC_CARDSTATE_TRANSFER               ((uint8_t)0x04)           /* eMMC is in transfer state */
#define EMMC_CARDSTATE_DATA                   ((uint8_t)0x05)           /* eMMC is in data sending state */
#define EMMC_CARDSTATE_RECEIVING              ((uint8_t)0x06)           /* eMMC is in receiving state */
#define EMMC_CARDSTATE_PROGRAMMING            ((uint8_t)0x07)           /* eMMC is in programming state */
#define EMMC_CARDSTATE_DISCONNECT             ((uint8_t)0x08)           /* eMMC is in disconnect state */
#define EMMC_CARDSTATE_LOCKED                 ((uint32_t)0x02000000U)   /* eMMC is in locked state */

/* eMMC bus width, check SCR register */
#define EMMC_BUS_WIDTH_8BIT                   ((uint32_t)0x00000002U)    /* 8-bit width bus mode */
#define EMMC_BUS_WIDTH_4BIT                   ((uint32_t)0x00000001U)    /* 4-bit width bus mode */
#define EMMC_BUS_WIDTH_1BIT                   ((uint32_t)0x00000000U)    /* 1-bit width bus mode */

/* masks for SCR register */
#define EMMC_MASK_0_7BITS                     ((uint32_t)0x000000FFU)    /* mask [7:0] bits */
#define EMMC_MASK_8_15BITS                    ((uint32_t)0x0000FF00U)    /* mask [15:8] bits */
#define EMMC_MASK_16_23BITS                   ((uint32_t)0x00FF0000U)    /* mask [23:16] bits */
#define EMMC_MASK_24_31BITS                   ((uint32_t)0xFF000000U)    /* mask [31:24] bits */

#define SDIO_FIFO_ADDR                        SDIO_FIFO(SDIO)            /* address of SDIO_FIFO */
#define EMMC_FIFOHALF_WORDS                   ((uint32_t)0x00000008U)    /* words of FIFO half full/empty */
#define EMMC_FIFOHALF_BYTES                   ((uint32_t)0x00000020U)    /* bytes of FIFO half full/empty */

#define EMMC_DATATIMEOUT                      ((uint32_t)0xFFFFFFFFU)    /* DSM data timeout */
#define EMMC_MAX_VOLT_VALIDATION              ((uint32_t)0x0000FFFFU)    /* the maximum times of voltage validation */
#define EMMC_MAX_DATA_LENGTH                  ((uint32_t)0x01FFFFFFU)    /* the maximum length of data */
#define EMMC_ALLZERO                          ((uint32_t)0x00000000U)    /* all zero */
#define EMMC_RCA_SHIFT                        ((uint8_t)0x10U)           /* RCA shift bits */

#define EMMC_CLK_DIV_INIT                     ((uint16_t)0x01F4)        /* eMMC clock division in initialization phase */
#define EMMC_CLK_DIV_TRANS_DSPEED             ((uint16_t)0x0008)        /* eMMC clock division in default speed transmission phase */
#define EMMC_CLK_DIV_TRANS_HSPEED             ((uint16_t)0x0002)        /* eMMC clock division in high speed transmission phase */

#define SDIO_MASK_INTC_FLAGS                  ((uint32_t)0x1FE00FFF)    /* mask flags of SDIO_INTC */
#define SDIO_MASK_CMD_FLAGS                   ((uint32_t)0x002000C5)    /* mask flags of  CMD FLAGS */
#define SDIO_MASK_DATA_FLAGS                  ((uint32_t)0x18000F3A)    /* mask flags of  DATA FLAGS */

uint32_t emmc_csd[4] = {0, 0, 0, 0};                                   /* content of CSD register */
uint32_t emmc_cid[4] = {0, 0, 0, 0};                                   /* content of CID register */
uint8_t  emmc_extcsd[512] = {0};                                       /* content of EXT_CSD register */
uint16_t emmc_rca = 0U;                                                /* RCA of eMMC card */
static uint32_t transmode = EMMC_POLLING_MODE;
static uint32_t totalnumber_bytes = 0U, stopcondition = 0U;

static __IO emmc_error_enum transerror = EMMC_OK;
static __IO uint32_t transend = 0U, number_bytes = 0U;

/* check if the command sent error occurs */
static emmc_error_enum cmdsent_error_check(void);
/* check if error occurs for R1 response */
static emmc_error_enum r1_error_check(uint8_t cmdindex);
/* check if error type for R1 response */
static emmc_error_enum r1_error_type_check(uint32_t resp);
/* check if error occurs for R2 response */
static emmc_error_enum r2_error_check(void);
/* check if error occurs for R3 response */
static emmc_error_enum r3_error_check(void);

/* get the state which the card is in */
static emmc_error_enum emmc_card_state_get(uint8_t *pcardstate);
/* get the data block size */
static uint32_t emmc_datablocksize_get(uint16_t bytesnumber);

/* configure the GPIO of SDIO interface */
static void gpio_config(void);
/* configure the RCU of SDIO */
static void rcu_config(void);
/* configure the DMA for SDIO request */
static void dma_config(uint32_t *srcbuf, uint32_t bufsize);

/*!
    \brief      initialize the eMMC card and make it in standby state
    \param[in]  none
    \param[out] none
    \retval     emmc_error_enum
*/
emmc_error_enum emmc_init(void)
{
    emmc_error_enum status = EMMC_OK;
    /* configure the RCU and GPIO, deinitialize the SDIO */
    rcu_config();
    gpio_config();
    sdio_deinit(SDIO);

    /* configure the clock and working voltage */
    status = emmc_power_on();
    if(EMMC_OK != status) {
        return status;
    }

    /* initialize the card and get CID and CSD of the card */
    status = emmc_card_init();
    if(EMMC_OK != status) {
        return status;
    }

    /* configure the SDIO peripheral */
    sdio_clock_config(SDIO, SDIO_SDIOCLKEDGE_RISING, SDIO_CLOCKPWRSAVE_DISABLE, EMMC_CLK_DIV_INIT);
    sdio_bus_mode_set(SDIO, SDIO_BUSMODE_1BIT);
    sdio_hardware_clock_disable(SDIO);

    return status;
}

/*!
    \brief      initialize the card and get CID and CSD of the card
    \param[in]  none
    \param[out] none
    \retval     emmc_error_enum
*/
emmc_error_enum emmc_card_init(void)
{
    emmc_error_enum status = EMMC_OK;

    if(SDIO_POWER_OFF == sdio_power_state_get(SDIO)) {
        status = EMMC_OPERATION_IMPROPER;
        return status;
    }

    /* send CMD2(EMMC_CMD_ALL_SEND_CID) to get the CID numbers */
    sdio_command_response_config(SDIO, EMMC_CMD_ALL_SEND_CID, (uint32_t)0x0, SDIO_RESPONSETYPE_LONG);
    sdio_wait_type_set(SDIO, SDIO_WAITTYPE_NO);
    sdio_csm_enable(SDIO);
    /* check if some error occurs */
    status = r2_error_check();

    if(EMMC_OK != status) {
        return status;
    }

    /* store the CID numbers */
    emmc_cid[0] = sdio_response_get(SDIO, SDIO_RESPONSE0);
    emmc_cid[1] = sdio_response_get(SDIO, SDIO_RESPONSE1);
    emmc_cid[2] = sdio_response_get(SDIO, SDIO_RESPONSE2);
    emmc_cid[3] = sdio_response_get(SDIO, SDIO_RESPONSE3);

    /* send CMD3(SEND_RELATIVE_ADDR) to ask the eMMC to publish a new relative address (RCA) */
    sdio_command_response_config(SDIO, EMMC_CMD_SEND_RELATIVE_ADDR, (uint32_t)emmc_rca << 16U, SDIO_RESPONSETYPE_SHORT);
    sdio_wait_type_set(SDIO, SDIO_WAITTYPE_NO);
    sdio_csm_enable(SDIO);
    /* check if some error occurs */
    status = r1_error_check(EMMC_CMD_SEND_RELATIVE_ADDR);
    if(EMMC_OK != status) {
        return status;
    }

    /* send CMD9(SEND_CSD) to get the addressed card's card-specific data (CSD) */
    sdio_command_response_config(SDIO, EMMC_CMD_SEND_CSD, (uint32_t)(emmc_rca << EMMC_RCA_SHIFT), SDIO_RESPONSETYPE_LONG);
    sdio_wait_type_set(SDIO, SDIO_WAITTYPE_NO);
    sdio_csm_enable(SDIO);
    /* check if some error occurs */
    status = r2_error_check();
    if(EMMC_OK != status) {
        return status;
    }
    /* store the card-specific data (CSD) */
    emmc_csd[0] = sdio_response_get(SDIO, SDIO_RESPONSE0);
    emmc_csd[1] = sdio_response_get(SDIO, SDIO_RESPONSE1);
    emmc_csd[2] = sdio_response_get(SDIO, SDIO_RESPONSE2);
    emmc_csd[3] = sdio_response_get(SDIO, SDIO_RESPONSE3);

    return status;
}

/*!
    \brief      configure the clock and the working voltage, and get the card type
    \param[in]  none
    \param[out] none
    \retval     emmc_error_enum
*/
emmc_error_enum emmc_power_on(void)
{
    emmc_error_enum status = EMMC_OK;
    uint32_t response = 0U;
    uint32_t timedelay = 0U;

    /* configure the SDIO peripheral */
    sdio_clock_config(SDIO, SDIO_SDIOCLKEDGE_RISING, SDIO_CLOCKPWRSAVE_DISABLE, EMMC_CLK_DIV_INIT);
    sdio_bus_mode_set(SDIO, SDIO_BUSMODE_1BIT);
    sdio_hardware_clock_disable(SDIO);
    sdio_power_state_set(SDIO, SDIO_POWER_ON);

    /* time delay for power up*/
    timedelay = 500U;
    while(timedelay > 0U) {
        timedelay--;
    }

    /* send CMD0(GO_IDLE_STATE) to reset the card */
    sdio_command_response_config(SDIO, EMMC_CMD_GO_IDLE_STATE, (uint32_t)0x0, SDIO_RESPONSETYPE_NO);
    sdio_wait_type_set(SDIO, SDIO_WAITTYPE_NO);
    /* enable the CSM */
    sdio_csm_enable(SDIO);

    /* check if command sent error occurs */
    status = cmdsent_error_check();
    if(EMMC_OK != status) {
        return status;
    }

    timedelay = 0x4000U;
    while(timedelay--) {
        sdio_command_response_config(SDIO, EMMC_CMD_SEND_OP_COND, (uint32_t)0x40ff8000, SDIO_RESPONSETYPE_SHORT);
        sdio_wait_type_set(SDIO, SDIO_WAITTYPE_NO);
        /* enable the CSM */
        sdio_csm_enable(SDIO);

        /* check if some error occurs */
        status = r3_error_check();
        if(EMMC_OK != status) {
            return status;
        }
        if(!timedelay) {
            status = EMMC_ERROR;
            return status;
        }

        /* get the response and check card power up status bit(busy) */
        response = sdio_response_get(SDIO, SDIO_RESPONSE0);
        if(0x80000000 == (response & 0x80000000)) {
            break;
        } else {
            continue;
        }
    }
    return status;
}

/*!
    \brief      close the power of SDIO
    \param[in]  none
    \param[out] none
    \retval     emmc_error_enum
*/
emmc_error_enum emmc_power_off(void)
{
    emmc_error_enum status = EMMC_OK;
    sdio_power_state_set(SDIO, SDIO_POWER_OFF);
    return status;
}

/*!
    \brief      configure the bus mode
    \param[in]  busmode: the bus mode
      \arg        SDIO_BUSMODE_1BIT: 1-bit SDIO card bus mode
      \arg        SDIO_BUSMODE_4BIT: 4-bit SDIO card bus mode
      \arg        SDIO_BUSMODE_8BIT: 8-bit SDIO card bus mode
     \param[in]  datarate: the data rate mode
      \arg        SDIO_DATA_RATE_SDR:  1/4/8 bits bus data SDR selected
      \arg        SDIO_DATA_RATE_DDR:  4/8 bits bus data DDR selected
    \param[out] none
    \retval     emmc_error_enum
*/
emmc_error_enum emmc_bus_mode_config(uint32_t busmode, uint32_t datarate)
{
    emmc_error_enum status = EMMC_OK;

    /* send CMD13(SEND_STATUS), addressed card sends its status registers */
    sdio_command_response_config(SDIO, EMMC_CMD_SEND_STATUS, (uint32_t)emmc_rca << EMMC_RCA_SHIFT, SDIO_RESPONSETYPE_SHORT);
    sdio_wait_type_set(SDIO, SDIO_WAITTYPE_NO);
    sdio_csm_enable(SDIO);
    /* check if some error occurs */
    status = r1_error_check(EMMC_CMD_SEND_STATUS);
    if(EMMC_OK != status) {
        return status;
    }

    if(SDIO_DATA_RATE_DDR == datarate) {
        /* send CMD6(EMMC_CMD_SWITCH) to configure bus interface timing */
        sdio_command_response_config(SDIO, EMMC_CMD_SWITCH_FUNC, (uint32_t)0x03B90100, SDIO_RESPONSETYPE_SHORT);
        sdio_wait_type_set(SDIO, SDIO_WAITTYPE_NO);
        /* enable the CSM */
        sdio_csm_enable(SDIO);

        /* check if some error occurs */
        status = r1_error_check(EMMC_CMD_SWITCH_FUNC);
        if(EMMC_OK != status) {
            return status;
        }

        if(SDIO_BUSMODE_8BIT == busmode) {
            /* send CMD6(EMMC_CMD_SWITCH) to set the width */
            sdio_command_response_config(SDIO, EMMC_CMD_SWITCH_FUNC, (uint32_t)0x03B70600, SDIO_RESPONSETYPE_SHORT);
            sdio_wait_type_set(SDIO, SDIO_WAITTYPE_NO);
            /* enable the CSM */
            sdio_csm_enable(SDIO);

            /* check if some error occurs */
            status = r1_error_check(EMMC_CMD_SWITCH_FUNC);
            if(EMMC_OK != status) {
                return status;
            }

            sdio_clock_config(SDIO, SDIO_SDIOCLKEDGE_FALLING, SDIO_CLOCKPWRSAVE_DISABLE, EMMC_CLK_DIV_TRANS_DSPEED);
            sdio_bus_mode_set(SDIO, busmode);
            sdio_data_rate_set(SDIO, SDIO_DATA_RATE_DDR);
            sdio_bus_speed_set(SDIO, SDIO_BUSSPEED_HIGH);
            sdio_hardware_clock_enable(SDIO);

        } else if(SDIO_BUSMODE_4BIT == busmode) {
            /* send CMD6(EMMC_CMD_SWITCH) to set the width */
            sdio_command_response_config(SDIO, EMMC_CMD_SWITCH_FUNC, (uint32_t)0x03B70500, SDIO_RESPONSETYPE_SHORT);
            sdio_wait_type_set(SDIO, SDIO_WAITTYPE_NO);
            /* enable the CSM */
            sdio_csm_enable(SDIO);

            /* check if some error occurs */
            status = r1_error_check(EMMC_CMD_SWITCH_FUNC);
            if(EMMC_OK != status) {
                return status;
            }

            sdio_clock_config(SDIO, SDIO_SDIOCLKEDGE_FALLING, SDIO_CLOCKPWRSAVE_DISABLE, EMMC_CLK_DIV_TRANS_DSPEED);
            sdio_bus_mode_set(SDIO, busmode);
            sdio_data_rate_set(SDIO, SDIO_DATA_RATE_DDR);
            sdio_bus_speed_set(SDIO, SDIO_BUSSPEED_HIGH);
            sdio_hardware_clock_enable(SDIO);
        } else {
            status = EMMC_PARAMETER_INVALID;
        }
    } else if(SDIO_DATA_RATE_SDR == datarate) {
        if(SDIO_BUSMODE_8BIT == busmode) {
            /* send CMD6(EMMC_CMD_SWITCH) to set the width */
            sdio_command_response_config(SDIO, EMMC_CMD_SWITCH_FUNC, (uint32_t)0x03B70200, SDIO_RESPONSETYPE_SHORT);
            sdio_wait_type_set(SDIO, SDIO_WAITTYPE_NO);
            /* enable the CSM */
            sdio_csm_enable(SDIO);

            /* check if some error occurs */
            status = r1_error_check(EMMC_CMD_SWITCH_FUNC);
            if(EMMC_OK != status) {
                return status;
            }

            sdio_clock_config(SDIO, SDIO_SDIOCLKEDGE_RISING, SDIO_CLOCKPWRSAVE_DISABLE, EMMC_CLK_DIV_TRANS_DSPEED);
            sdio_bus_mode_set(SDIO, busmode);
            sdio_hardware_clock_enable(SDIO);
        } else if(SDIO_BUSMODE_4BIT == busmode) {
            /* send CMD6(EMMC_CMD_SWITCH) to set the width */
            sdio_command_response_config(SDIO, EMMC_CMD_SWITCH_FUNC, (uint32_t)0x03B70100, SDIO_RESPONSETYPE_SHORT);
            sdio_wait_type_set(SDIO, SDIO_WAITTYPE_NO);
            /* enable the CSM */
            sdio_csm_enable(SDIO);

            /* check if some error occurs */
            status = r1_error_check(EMMC_CMD_SWITCH_FUNC);
            if(EMMC_OK != status) {
                return status;
            }

            sdio_clock_config(SDIO, SDIO_SDIOCLKEDGE_RISING, SDIO_CLOCKPWRSAVE_DISABLE, EMMC_CLK_DIV_TRANS_DSPEED);
            sdio_bus_mode_set(SDIO, busmode);
            sdio_hardware_clock_enable(SDIO);

        } else if(SDIO_BUSMODE_1BIT == busmode) {
            sdio_clock_config(SDIO, SDIO_SDIOCLKEDGE_RISING, SDIO_CLOCKPWRSAVE_DISABLE, EMMC_CLK_DIV_TRANS_DSPEED);
            sdio_bus_mode_set(SDIO, busmode);
            sdio_hardware_clock_enable(SDIO);
        } else {
            status = EMMC_PARAMETER_INVALID;
        }
    } else {
        status = EMMC_PARAMETER_INVALID;
    }
    sdio_flag_clear(SDIO, SDIO_MASK_INTC_FLAGS);
    return status;
}

/*!
    \brief      configure the mode of transmission
    \param[in]  txmode: transfer mode
      \arg        EMMC_DMA_MODE: DMA mode
      \arg        EMMC_POLLING_MODE: polling mode
    \param[out] none
    \retval     emmc_error_enum
*/
emmc_error_enum emmc_transfer_mode_config(uint32_t txmode)
{
    emmc_error_enum status = EMMC_OK;
    /* set the transfer mode */
    if((EMMC_DMA_MODE == txmode) || (EMMC_POLLING_MODE == txmode)) {
        transmode = txmode;
    } else {
        status = EMMC_PARAMETER_INVALID;
    }
    return status;
}

/*!
    \brief      read a block data into a buffer from the specified address of a card
    \param[out] preadbuffer: a pointer that store a block read data
    \param[in]  readaddr: the read data address
    \param[in]  blocksize: the data block size
    \retval     emmc_error_enum
*/
emmc_error_enum emmc_block_read(uint32_t *preadbuffer, uint32_t readaddr, uint16_t blocksize)
{
    /* initialize the variables */
    emmc_error_enum status = EMMC_OK;
    uint32_t count = 0U, align = 0U, datablksize = SDIO_DATABLOCKSIZE_1BYTE, *ptempbuff = preadbuffer;
    __IO uint32_t timeout = 0U;

    if(NULL == preadbuffer) {
        status = EMMC_PARAMETER_INVALID;
        return status;
    }

    transerror = EMMC_OK;
    transend = 0U;
    totalnumber_bytes = 0U;
    /* clear all DSM configuration */
    sdio_data_config(SDIO, 0U, 0U, SDIO_DATABLOCKSIZE_1BYTE);
    sdio_data_transfer_config(SDIO, SDIO_TRANSMODE_BLOCKCOUNT, SDIO_TRANSDIRECTION_TOCARD);
    sdio_dsm_disable(SDIO);
    sdio_idma_disable(SDIO);

    /* check whether the card is locked */
    if(sdio_response_get(SDIO, SDIO_RESPONSE0) & EMMC_CARDSTATE_LOCKED) {
        status = EMMC_LOCK_UNLOCK_FAILED;
        return status;
    }

    blocksize = 512U;

    align = blocksize & (blocksize - 1U);

    if((blocksize > 0U) && (blocksize <= 2048U) && (0U == align)) {
        datablksize = emmc_datablocksize_get(blocksize);
        if(EMMC_OK != status) {
            return status;
        }
    } else {
        status = EMMC_PARAMETER_INVALID;
        return status;
    }

    stopcondition = 0U;
    totalnumber_bytes = (uint32_t)blocksize;

    if(EMMC_POLLING_MODE == transmode) {

        /* configure SDIO data transmission */
        sdio_data_config(SDIO, EMMC_DATATIMEOUT, totalnumber_bytes, datablksize);
        sdio_data_transfer_config(SDIO, SDIO_TRANSMODE_BLOCKCOUNT, SDIO_TRANSDIRECTION_TOSDIO);
        sdio_trans_start_enable(SDIO);


        /* send CMD17(READ_SINGLE_BLOCK) to read a block */
        sdio_command_response_config(SDIO, EMMC_CMD_READ_SINGLE_BLOCK, (uint32_t)readaddr, SDIO_RESPONSETYPE_SHORT);
        sdio_wait_type_set(SDIO, SDIO_WAITTYPE_NO);
        sdio_csm_enable(SDIO);
        /* check if some error occurs */
        status = r1_error_check(EMMC_CMD_READ_SINGLE_BLOCK);
        if(EMMC_OK != status) {
            return status;
        }

        /* polling mode */
        while(!sdio_flag_get(SDIO, SDIO_FLAG_DTCRCERR | SDIO_FLAG_DTTMOUT | SDIO_FLAG_RXORE | SDIO_FLAG_DTBLKEND | SDIO_FLAG_DTEND)) {
            if(RESET != sdio_flag_get(SDIO, SDIO_FLAG_RFH)) {
                /* at least 8 words can be read in the FIFO */
                for(count = 0U; count < EMMC_FIFOHALF_WORDS; count++) {
                    *(ptempbuff + count) = sdio_data_read(SDIO);
                }
                ptempbuff += EMMC_FIFOHALF_WORDS;
            }
        }

        sdio_trans_start_disable(SDIO);

        /* whether some error occurs and return it */
        if(RESET != sdio_flag_get(SDIO, SDIO_FLAG_DTCRCERR)) {
            status = EMMC_DATA_CRC_ERROR;
            sdio_flag_clear(SDIO, SDIO_FLAG_DTCRCERR);
            return status;
        } else if(RESET != sdio_flag_get(SDIO, SDIO_FLAG_DTTMOUT)) {
            status = EMMC_DATA_TIMEOUT;
            sdio_flag_clear(SDIO, SDIO_FLAG_DTTMOUT);
            return status;
        } else if(RESET != sdio_flag_get(SDIO, SDIO_FLAG_RXORE)) {
            status = EMMC_RX_OVERRUN_ERROR;
            sdio_flag_clear(SDIO, SDIO_FLAG_RXORE);
            return status;
        } else {
            /* if else end */
        }

        while((SET != sdio_flag_get(SDIO, SDIO_FLAG_RFE)) && (SET == sdio_flag_get(SDIO, SDIO_FLAG_DATSTA))) {
            *ptempbuff = sdio_data_read(SDIO);
            ++ptempbuff;
        }
        /* clear the DATA_FLAGS flags */
        sdio_flag_clear(SDIO, SDIO_MASK_DATA_FLAGS);
    } else if(EMMC_DMA_MODE == transmode) {
        /* DMA mode */
        /* enable the SDIO corresponding interrupts and DMA function */
        sdio_interrupt_enable(SDIO, SDIO_INT_CCRCERR | SDIO_INT_DTTMOUT | SDIO_INT_RXORE | SDIO_INT_DTEND);
        dma_config(preadbuffer, (uint32_t)(blocksize >> 5));
        sdio_idma_enable(SDIO);

        /* configure SDIO data transmission */
        sdio_data_config(SDIO, EMMC_DATATIMEOUT, totalnumber_bytes, datablksize);
        sdio_data_transfer_config(SDIO, SDIO_TRANSMODE_BLOCKCOUNT, SDIO_TRANSDIRECTION_TOSDIO);
        sdio_trans_start_enable(SDIO);

        /* send CMD17(READ_SINGLE_BLOCK) to read a block */
        sdio_command_response_config(SDIO, EMMC_CMD_READ_SINGLE_BLOCK, (uint32_t)readaddr, SDIO_RESPONSETYPE_SHORT);
        sdio_wait_type_set(SDIO, SDIO_WAITTYPE_NO);
        sdio_csm_enable(SDIO);
        /* check if some error occurs */
        status = r1_error_check(EMMC_CMD_READ_SINGLE_BLOCK);
        if(EMMC_OK != status) {
            return status;
        }

        while((0U == transend) && (EMMC_OK == transerror)) {
        }
        if(EMMC_OK != transerror) {
            return transerror;
        }
    } else {
        status = EMMC_PARAMETER_INVALID;
    }
    return status;
}

/*!
    \brief      read multiple blocks data into a buffer from the specified address of a card
    \param[out] preadbuffer: a pointer that store multiple blocks read data
    \param[in]  readaddr: the read data address
    \param[in]  blocksize: the data block size
    \param[in]  blocksnumber: number of blocks that will be read
    \retval     emmc_error_enum
*/
emmc_error_enum emmc_multiblocks_read(uint32_t *preadbuffer, uint32_t readaddr, uint16_t blocksize, uint32_t blocksnumber)
{
    /* initialize the variables */
    emmc_error_enum status = EMMC_OK;
    uint32_t count = 0U, align = 0U, datablksize = SDIO_DATABLOCKSIZE_1BYTE, *ptempbuff = preadbuffer;
    __IO uint32_t timeout = 0U;

    if(NULL == preadbuffer) {
        status = EMMC_PARAMETER_INVALID;
        return status;
    }

    transerror = EMMC_OK;
    transend = 0U;
    totalnumber_bytes = 0U;
    /* clear all DSM configuration */
    sdio_data_config(SDIO, 0U, 0U, SDIO_DATABLOCKSIZE_1BYTE);
    sdio_data_transfer_config(SDIO, SDIO_TRANSMODE_BLOCKCOUNT, SDIO_TRANSDIRECTION_TOCARD);
    sdio_dsm_disable(SDIO);
    sdio_idma_disable(SDIO);

    /* check whether the card is locked */
    if(sdio_response_get(SDIO, SDIO_RESPONSE0) & EMMC_CARDSTATE_LOCKED) {
        status = EMMC_LOCK_UNLOCK_FAILED;
        return status;
    }

    blocksize = 512U;

    align = blocksize & (blocksize - 1U);
    if((blocksize > 0U) && (blocksize <= 2048U) && (0U == align)) {
        datablksize = emmc_datablocksize_get(blocksize);
        if(EMMC_OK != status) {
            return status;
        }
    } else {
        status = EMMC_PARAMETER_INVALID;
        return status;
    }

    if(blocksnumber > 1U) {
        if(blocksnumber * blocksize > EMMC_MAX_DATA_LENGTH) {
            /* exceeds the maximum length */
            status = EMMC_PARAMETER_INVALID;
            return status;
        }

        stopcondition = 1U;
        totalnumber_bytes = blocksnumber * blocksize;

        if(EMMC_POLLING_MODE == transmode) {

            /* configure SDIO data transmission */
            sdio_data_config(SDIO, EMMC_DATATIMEOUT, totalnumber_bytes, datablksize);
            sdio_data_transfer_config(SDIO, SDIO_TRANSMODE_BLOCKCOUNT, SDIO_TRANSDIRECTION_TOSDIO);
            sdio_trans_start_enable(SDIO);

            /* send CMD18(READ_MULTIPLE_BLOCK) to read multiple blocks */
            sdio_command_response_config(SDIO, EMMC_CMD_READ_MULTIPLE_BLOCK, readaddr, SDIO_RESPONSETYPE_SHORT);
            sdio_wait_type_set(SDIO, SDIO_WAITTYPE_NO);
            sdio_csm_enable(SDIO);

            /* check if some error occurs */
            status = r1_error_check(EMMC_CMD_READ_MULTIPLE_BLOCK);
            if(EMMC_OK != status) {
                return status;
            }

            /* polling mode */
            while(!sdio_flag_get(SDIO, SDIO_FLAG_DTCRCERR | SDIO_FLAG_DTTMOUT | SDIO_FLAG_RXORE | SDIO_FLAG_DTBLKEND | SDIO_FLAG_DTEND)) {
                if(RESET != sdio_flag_get(SDIO, SDIO_FLAG_RFH)) {
                    /* at least 8 words can be read in the FIFO */
                    for(count = 0U; count < EMMC_FIFOHALF_WORDS; count++) {
                        *(ptempbuff + count) = sdio_data_read(SDIO);
                    }
                    ptempbuff += EMMC_FIFOHALF_WORDS;
                }
            }

            /* whether some error occurs and return it */
            if(RESET != sdio_flag_get(SDIO, SDIO_FLAG_DTCRCERR)) {
                status = EMMC_DATA_CRC_ERROR;
                sdio_flag_clear(SDIO, SDIO_FLAG_DTCRCERR);
                return status;
            } else if(RESET != sdio_flag_get(SDIO, SDIO_FLAG_DTTMOUT)) {
                status = EMMC_DATA_TIMEOUT;
                sdio_flag_clear(SDIO, SDIO_FLAG_DTTMOUT);
                return status;
            } else if(RESET != sdio_flag_get(SDIO, SDIO_FLAG_RXORE)) {
                status = EMMC_RX_OVERRUN_ERROR;
                sdio_flag_clear(SDIO, SDIO_FLAG_RXORE);
                return status;
            } else {
                /* if else end */
            }

            while((SET != sdio_flag_get(SDIO, SDIO_FLAG_RFE)) && (SET == sdio_flag_get(SDIO, SDIO_FLAG_DATSTA))) {
                *ptempbuff = sdio_data_read(SDIO);
                ++ptempbuff;
            }

            sdio_trans_start_disable(SDIO);

            if(RESET != sdio_flag_get(SDIO, SDIO_FLAG_DTEND)) {
                /* send CMD12(STOP_TRANSMISSION) to stop transmission */
                sdio_command_response_config(SDIO, EMMC_CMD_STOP_TRANSMISSION, (uint32_t)0x0, SDIO_RESPONSETYPE_SHORT);
                sdio_wait_type_set(SDIO, SDIO_WAITTYPE_NO);
                sdio_csm_enable(SDIO);
                /* check if some error occurs */
                status = r1_error_check(EMMC_CMD_STOP_TRANSMISSION);
                if(EMMC_OK != status) {
                    return status;
                }
            }
            /* clear the DATA_FLAGS flags */
            sdio_flag_clear(SDIO, SDIO_MASK_DATA_FLAGS);
        } else if(EMMC_DMA_MODE == transmode) {
            /* DMA mode */
            /* enable the SDIO corresponding interrupts and DMA function */
            sdio_interrupt_enable(SDIO, SDIO_INT_CCRCERR | SDIO_INT_DTTMOUT | SDIO_INT_RXORE | SDIO_INT_DTEND);
            dma_config(preadbuffer, (uint32_t)(blocksize >> 5));
            sdio_idma_enable(SDIO);

            /* configure SDIO data transmission */
            sdio_data_config(SDIO, EMMC_DATATIMEOUT, totalnumber_bytes, datablksize);
            sdio_data_transfer_config(SDIO, SDIO_TRANSMODE_BLOCKCOUNT, SDIO_TRANSDIRECTION_TOSDIO);
            sdio_trans_start_enable(SDIO);

            /* send CMD18(READ_MULTIPLE_BLOCK) to read multiple blocks */
            sdio_command_response_config(SDIO, EMMC_CMD_READ_MULTIPLE_BLOCK, readaddr, SDIO_RESPONSETYPE_SHORT);
            sdio_wait_type_set(SDIO, SDIO_WAITTYPE_NO);
            sdio_csm_enable(SDIO);
            /* check if some error occurs */
            status = r1_error_check(EMMC_CMD_READ_MULTIPLE_BLOCK);
            if(EMMC_OK != status) {
                return status;
            }

            while((0U == transend) && (EMMC_OK == transerror)) {
            }
            if(EMMC_OK != transerror) {
                return transerror;
            }
        } else {
            status = EMMC_PARAMETER_INVALID;
        }
    }
    return status;
}

/*!
    \brief      write a block data to the specified address of a card
    \param[in]  pwritebuffer: a pointer that store a block data to be transferred
    \param[in]  writeaddr: the read data address
    \param[in]  blocksize: the data block size
    \param[out] none
    \retval     emmc_error_enum
*/
emmc_error_enum emmc_block_write(uint32_t *pwritebuffer, uint32_t writeaddr, uint16_t blocksize)
{
    /* initialize the variables */
    emmc_error_enum status = EMMC_OK;
    uint8_t cardstate = 0U;
    __IO uint32_t count = 0U, align = 0U, datablksize = SDIO_DATABLOCKSIZE_1BYTE, *ptempbuff = pwritebuffer;
    uint32_t transbytes = 0U, restwords = 0U, response = 0U;
    __IO uint32_t timeout = 0U;

    if(NULL == pwritebuffer) {
        status = EMMC_PARAMETER_INVALID;
        return status;
    }

    transerror = EMMC_OK;
    transend = 0U;
    totalnumber_bytes = 0U;
    /* clear all DSM configuration */
    sdio_data_config(SDIO, 0U, 0U, SDIO_DATABLOCKSIZE_1BYTE);
    sdio_data_transfer_config(SDIO, SDIO_TRANSMODE_BLOCKCOUNT, SDIO_TRANSDIRECTION_TOCARD);
    sdio_dsm_disable(SDIO);
    sdio_idma_disable(SDIO);

    /* check whether the card is locked */
    if(sdio_response_get(SDIO, SDIO_RESPONSE0) & EMMC_CARDSTATE_LOCKED) {
        status = EMMC_LOCK_UNLOCK_FAILED;
        return status;
    }

    align = blocksize & (blocksize - 1U);
    if((blocksize > 0U) && (blocksize <= 2048U) && (0U == align)) {
        datablksize = emmc_datablocksize_get(blocksize);
        if(EMMC_OK != status) {
            return status;
        }
    } else {
        status = EMMC_PARAMETER_INVALID;
        return status;
    }

    /* send CMD13(SEND_STATUS), addressed card sends its status registers */
    sdio_command_response_config(SDIO, EMMC_CMD_SEND_STATUS, (uint32_t)emmc_rca << EMMC_RCA_SHIFT, SDIO_RESPONSETYPE_SHORT);
    sdio_wait_type_set(SDIO, SDIO_WAITTYPE_NO);
    sdio_csm_enable(SDIO);
    /* check if some error occurs */
    status = r1_error_check(EMMC_CMD_SEND_STATUS);
    if(EMMC_OK != status) {
        return status;
    }

    response = sdio_response_get(SDIO, SDIO_RESPONSE0);
    timeout = 100000U;

    while((0U == (response & EMMC_R1_READY_FOR_DATA)) && (timeout > 0U)) {
        /* continue to send CMD13 to polling the state of card until buffer empty or timeout */
        --timeout;
        /* send CMD13(SEND_STATUS), addressed card sends its status registers */
        sdio_command_response_config(SDIO, EMMC_CMD_SEND_STATUS, (uint32_t)emmc_rca << EMMC_RCA_SHIFT, SDIO_RESPONSETYPE_SHORT);
        sdio_wait_type_set(SDIO, SDIO_WAITTYPE_NO);
        sdio_csm_enable(SDIO);
        /* check if some error occurs */
        status = r1_error_check(EMMC_CMD_SEND_STATUS);
        if(EMMC_OK != status) {
            return status;
        }
        response = sdio_response_get(SDIO, SDIO_RESPONSE0);
    }
    if(0U == timeout) {
        return EMMC_ERROR;
    }

    stopcondition = 0U;
    totalnumber_bytes = blocksize;

    /* configure the SDIO data transmission */
    sdio_data_config(SDIO, EMMC_DATATIMEOUT, totalnumber_bytes, datablksize);
    sdio_data_transfer_config(SDIO, SDIO_TRANSMODE_BLOCKCOUNT, SDIO_TRANSDIRECTION_TOCARD);
    sdio_trans_start_enable(SDIO);

    if(EMMC_POLLING_MODE == transmode) {
        /* send CMD24(WRITE_BLOCK) to write a block */
        sdio_command_response_config(SDIO, EMMC_CMD_WRITE_BLOCK, writeaddr, SDIO_RESPONSETYPE_SHORT);
        sdio_wait_type_set(SDIO, SDIO_WAITTYPE_NO);
        sdio_csm_enable(SDIO);
        /* check if some error occurs */
        status = r1_error_check(EMMC_CMD_WRITE_BLOCK);
        if(EMMC_OK != status) {
            return status;
        }

        /* polling mode */
        while(!sdio_flag_get(SDIO, SDIO_FLAG_DTCRCERR | SDIO_FLAG_DTTMOUT | SDIO_FLAG_TXURE | SDIO_FLAG_DTBLKEND | SDIO_FLAG_DTEND)) {
            if(RESET != sdio_flag_get(SDIO, SDIO_FLAG_TFH)) {
                /* at least 8 words can be written into the FIFO */
                if((totalnumber_bytes - transbytes) < EMMC_FIFOHALF_BYTES) {
                    restwords = (totalnumber_bytes - transbytes) / 4U + (((totalnumber_bytes - transbytes) % 4U == 0U) ? 0U : 1U);
                    for(count = 0U; count < restwords; count++) {
                        sdio_data_write(SDIO, *ptempbuff);
                        ++ptempbuff;
                        transbytes += 4U;
                    }
                } else {
                    for(count = 0U; count < EMMC_FIFOHALF_WORDS; count++) {
                        sdio_data_write(SDIO, *(ptempbuff + count));
                    }
                    /* 8 words(32 bytes) has been transferred */
                    ptempbuff += EMMC_FIFOHALF_WORDS;
                    transbytes += EMMC_FIFOHALF_BYTES;
                }
            }
        }
        sdio_trans_start_disable(SDIO);
        /* whether some error occurs and return it */
        if(RESET != sdio_flag_get(SDIO, SDIO_FLAG_DTCRCERR)) {
            status = EMMC_DATA_CRC_ERROR;
            sdio_flag_clear(SDIO, SDIO_FLAG_DTCRCERR);
            return status;
        } else if(RESET != sdio_flag_get(SDIO, SDIO_FLAG_DTTMOUT)) {
            status = EMMC_DATA_TIMEOUT;
            sdio_flag_clear(SDIO, SDIO_FLAG_DTTMOUT);
            return status;
        } else if(RESET != sdio_flag_get(SDIO, SDIO_FLAG_TXURE)) {
            status = EMMC_TX_UNDERRUN_ERROR;
            sdio_flag_clear(SDIO, SDIO_FLAG_TXURE);
            return status;
        } else {
            /* if else end */
        }

    } else if(EMMC_DMA_MODE == transmode) {
        /* DMA mode */
        /* enable the SDIO corresponding interrupts and DMA */
        sdio_interrupt_enable(SDIO, SDIO_INT_DTCRCERR | SDIO_INT_DTTMOUT | SDIO_INT_TXURE | SDIO_INT_DTEND);
        dma_config(pwritebuffer, (uint32_t)(blocksize >> 5));
        sdio_idma_enable(SDIO);

        /* send CMD24(WRITE_BLOCK) to write a block */
        sdio_command_response_config(SDIO, EMMC_CMD_WRITE_BLOCK, writeaddr, SDIO_RESPONSETYPE_SHORT);
        sdio_wait_type_set(SDIO, SDIO_WAITTYPE_NO);
        sdio_csm_enable(SDIO);
        /* check if some error occurs */
        status = r1_error_check(EMMC_CMD_WRITE_BLOCK);
        if(EMMC_OK != status) {
            return status;
        }

        while((0U == transend) && (EMMC_OK == transerror)) {
        }

        if(EMMC_OK != transerror) {
            return transerror;
        }
    } else {
        status = EMMC_PARAMETER_INVALID;
        return status;
    }

    /* clear the DATA_FLAGS flags */
    sdio_flag_clear(SDIO, SDIO_MASK_DATA_FLAGS);
    /* get the card state and wait the card is out of programming and receiving state */
    status = emmc_card_state_get(&cardstate);
    while((EMMC_OK == status) && ((EMMC_CARDSTATE_PROGRAMMING == cardstate) || (EMMC_CARDSTATE_RECEIVING == cardstate))) {
        status = emmc_card_state_get(&cardstate);
    }
    return status;
}

/*!
    \brief      write multiple blocks data to the specified address of a card
    \param[in]  pwritebuffer: a pointer that store multiple blocks data to be transferred
    \param[in]  writeaddr: the read data address
    \param[in]  blocksize: the data block size
    \param[in]  blocksnumber: number of blocks that will be written
    \param[out] none
    \retval     emmc_error_enum
*/
emmc_error_enum emmc_multiblocks_write(uint32_t *pwritebuffer, uint32_t writeaddr, uint16_t blocksize, uint32_t blocksnumber)
{
    /* initialize the variables */
    emmc_error_enum status = EMMC_OK;
    uint8_t cardstate = 0U;
    uint32_t count = 0U, align = 0U, datablksize = SDIO_DATABLOCKSIZE_1BYTE, *ptempbuff = pwritebuffer;
    uint32_t transbytes = 0U, restwords = 0U, response = 0U;
    __IO uint32_t timeout = 0U;

    if(NULL == pwritebuffer) {
        status = EMMC_PARAMETER_INVALID;
        return status;
    }

    transerror = EMMC_OK;
    transend = 0U;
    totalnumber_bytes = 0U;
    /* clear all DSM configuration */
    sdio_data_config(SDIO, 0U, 0U, SDIO_DATABLOCKSIZE_1BYTE);
    sdio_data_transfer_config(SDIO, SDIO_TRANSMODE_BLOCKCOUNT, SDIO_TRANSDIRECTION_TOCARD);
    sdio_dsm_disable(SDIO);
    sdio_idma_disable(SDIO);

    /* check whether the card is locked */
    if(sdio_response_get(SDIO, SDIO_RESPONSE0) & EMMC_CARDSTATE_LOCKED) {
        status = EMMC_LOCK_UNLOCK_FAILED;
        return status;
    }

    blocksize = 512U;

    align = blocksize & (blocksize - 1U);
    if((blocksize > 0U) && (blocksize <= 2048U) && (0U == align)) {
        datablksize = emmc_datablocksize_get(blocksize);
        if(EMMC_OK != status) {
            return status;
        }
    } else {
        status = EMMC_PARAMETER_INVALID;
        return status;
    }

    /* send CMD13(SEND_STATUS), addressed card sends its status registers */
    sdio_command_response_config(SDIO, EMMC_CMD_SEND_STATUS, (uint32_t)emmc_rca << EMMC_RCA_SHIFT, SDIO_RESPONSETYPE_SHORT);
    sdio_wait_type_set(SDIO, SDIO_WAITTYPE_NO);
    sdio_csm_enable(SDIO);
    /* check if some error occurs */
    status = r1_error_check(EMMC_CMD_SEND_STATUS);
    if(EMMC_OK != status) {
        return status;
    }

    response = sdio_response_get(SDIO, SDIO_RESPONSE0);
    timeout = 100000U;

    while((0U == (response & EMMC_R1_READY_FOR_DATA)) && (timeout > 0U)) {
        /* continue to send CMD13 to polling the state of card until buffer empty or timeout */
        --timeout;
        /* send CMD13(SEND_STATUS), addressed card sends its status registers */
        sdio_command_response_config(SDIO, EMMC_CMD_SEND_STATUS, (uint32_t)emmc_rca << EMMC_RCA_SHIFT, SDIO_RESPONSETYPE_SHORT);
        sdio_wait_type_set(SDIO, SDIO_WAITTYPE_NO);
        sdio_csm_enable(SDIO);
        /* check if some error occurs */
        status = r1_error_check(EMMC_CMD_SEND_STATUS);
        if(EMMC_OK != status) {
            return status;
        }
        response = sdio_response_get(SDIO, SDIO_RESPONSE0);
    }
    if(0U == timeout) {
        return EMMC_ERROR;
    }

    if(blocksnumber > 1U) {
        if(blocksnumber * blocksize > EMMC_MAX_DATA_LENGTH) {
            status = EMMC_PARAMETER_INVALID;
            return status;
        }

        /* send CMD23(EMMC_SET_BLOCK_COUNT) to set the number of write blocks to be preerased before writing */
        sdio_command_response_config(SDIO, EMMC_SET_BLOCK_COUNT, blocksnumber, SDIO_RESPONSETYPE_SHORT);
        sdio_wait_type_set(SDIO, SDIO_WAITTYPE_NO);
        sdio_csm_enable(SDIO);
        /* check if some error occurs */
        status = r1_error_check(EMMC_SET_BLOCK_COUNT);
        if(EMMC_OK != status) {
            return status;
        }

        stopcondition = 0U;
        totalnumber_bytes = blocksnumber * blocksize;

        if(EMMC_POLLING_MODE == transmode) {
            /* configure the SDIO data transmission */
            sdio_data_config(SDIO, EMMC_DATATIMEOUT, totalnumber_bytes, datablksize);
            sdio_data_transfer_config(SDIO, SDIO_TRANSMODE_BLOCKCOUNT, SDIO_TRANSDIRECTION_TOCARD);
            sdio_trans_start_enable(SDIO);

            /* send CMD25(WRITE_MULTIPLE_BLOCK) to continuously write blocks of data */
            sdio_command_response_config(SDIO, EMMC_CMD_WRITE_MULTIPLE_BLOCK, writeaddr, SDIO_RESPONSETYPE_SHORT);
            sdio_wait_type_set(SDIO, SDIO_WAITTYPE_NO);
            sdio_csm_enable(SDIO);
            /* check if some error occurs */
            status = r1_error_check(EMMC_CMD_WRITE_MULTIPLE_BLOCK);
            if(EMMC_OK != status) {
                return status;
            }
            sdio_dsm_enable(SDIO);

            /* polling mode */
            while(!sdio_flag_get(SDIO, SDIO_FLAG_DTCRCERR | SDIO_FLAG_DTTMOUT | SDIO_FLAG_TXURE | SDIO_FLAG_DTBLKEND | SDIO_FLAG_DTEND)) {
                if(RESET != sdio_flag_get(SDIO, SDIO_FLAG_TFH)) {
                    /* at least 8 words can be written into the FIFO */
                    if((totalnumber_bytes - transbytes) < EMMC_FIFOHALF_BYTES) {
                        restwords = (totalnumber_bytes - transbytes) / 4U + (((totalnumber_bytes - transbytes) % 4U == 0U) ? 0U : 1U);
                        for(count = 0U; count < restwords; count++) {
                            sdio_data_write(SDIO, *ptempbuff);
                            ++ptempbuff;
                            transbytes += 4U;
                        }
                    } else {
                        for(count = 0U; count < EMMC_FIFOHALF_WORDS; count++) {
                            sdio_data_write(SDIO, *(ptempbuff + count));
                        }
                        /* 8 words(32 bytes) has been transferred */
                        ptempbuff += EMMC_FIFOHALF_WORDS;
                        transbytes += EMMC_FIFOHALF_BYTES;
                    }
                }
            }
            sdio_trans_start_disable(SDIO);

            /* whether some error occurs and return it */
            if(RESET != sdio_flag_get(SDIO, SDIO_FLAG_DTCRCERR)) {
                status = EMMC_DATA_CRC_ERROR;
                sdio_flag_clear(SDIO, SDIO_FLAG_DTCRCERR);
                return status;
            } else if(RESET != sdio_flag_get(SDIO, SDIO_FLAG_DTTMOUT)) {
                status = EMMC_DATA_TIMEOUT;
                sdio_flag_clear(SDIO, SDIO_FLAG_DTTMOUT);
                return status;
            } else if(RESET != sdio_flag_get(SDIO, SDIO_FLAG_TXURE)) {
                status = EMMC_TX_UNDERRUN_ERROR;
                sdio_flag_clear(SDIO, SDIO_FLAG_TXURE);
                return status;
            } else {
                /* if else end */
            }
        } else if(EMMC_DMA_MODE == transmode) {
            /* DMA mode */
            /* enable the SDIO corresponding interrupts and DMA */
            sdio_interrupt_enable(SDIO, SDIO_INT_DTCRCERR | SDIO_INT_DTTMOUT | SDIO_INT_TXURE | SDIO_INT_DTEND);
            dma_config(pwritebuffer, (uint32_t)(blocksize >> 5));
            sdio_idma_enable(SDIO);

            /* configure the SDIO data transmission */
            sdio_data_config(SDIO, EMMC_DATATIMEOUT, totalnumber_bytes, datablksize);
            sdio_data_transfer_config(SDIO, SDIO_TRANSMODE_BLOCKCOUNT, SDIO_TRANSDIRECTION_TOCARD);
            sdio_trans_start_enable(SDIO);

            /* send CMD25(WRITE_MULTIPLE_BLOCK) to continuously write blocks of data */
            sdio_command_response_config(SDIO, EMMC_CMD_WRITE_MULTIPLE_BLOCK, writeaddr, SDIO_RESPONSETYPE_SHORT);
            sdio_wait_type_set(SDIO, SDIO_WAITTYPE_NO);
            sdio_csm_enable(SDIO);
            /* check if some error occurs */
            status = r1_error_check(EMMC_CMD_WRITE_MULTIPLE_BLOCK);
            if(EMMC_OK != status) {
                return status;
            }
            while((0U == transend) && (EMMC_OK == transerror)) {
            }

            if(EMMC_OK != transerror) {
                return transerror;
            }
        } else {
            status = EMMC_PARAMETER_INVALID;
            return status;
        }
    }
    /* clear the DATA_FLAGS flags */
    sdio_flag_clear(SDIO, SDIO_MASK_DATA_FLAGS);
    /* get the card state and wait the card is out of programming and receiving state */
    status = emmc_card_state_get(&cardstate);
    while((EMMC_OK == status) && ((EMMC_CARDSTATE_PROGRAMMING == cardstate) || (EMMC_CARDSTATE_RECEIVING == cardstate))) {
        status = emmc_card_state_get(&cardstate);
    }
    return status;
}

/*!
    \brief      process all the interrupts which the corresponding flags are set
    \param[in]  none
    \param[out] none
    \retval     emmc_error_enum
*/
emmc_error_enum emmc_interrupts_process(void)
{
    transerror = EMMC_OK;
    if(RESET != sdio_interrupt_flag_get(SDIO, SDIO_INT_FLAG_DTEND)) {
        /* clear DTEND flag */
        sdio_interrupt_flag_clear(SDIO, SDIO_INT_FLAG_DTEND);
        /* disable idma for idma transfer */
        sdio_idma_disable(SDIO);

        /* disable all the interrupts */
        sdio_interrupt_disable(SDIO, SDIO_INT_DTCRCERR | SDIO_INT_DTTMOUT | SDIO_INT_DTEND |
                               SDIO_INT_TFH | SDIO_INT_RFH | SDIO_INT_TXURE | SDIO_INT_RXORE);
        sdio_trans_start_disable(SDIO);
        /* send CMD12 to stop data transfer in multiple blocks operation */
        if(1 == stopcondition) {
            transerror = emmc_transfer_stop();
        } else {
            transerror = EMMC_OK;
        }
        transend = 1;
        number_bytes = 0;
        /* clear data flags */
        sdio_flag_clear(SDIO, SDIO_MASK_DATA_FLAGS);
        return transerror;
    }

    if(RESET != sdio_interrupt_flag_get(SDIO, SDIO_INT_FLAG_DTCRCERR | SDIO_INT_FLAG_DTTMOUT | SDIO_INT_FLAG_TXURE | SDIO_INT_FLAG_RXORE)) {
        /* set different errors */
        if(RESET != sdio_interrupt_flag_get(SDIO, SDIO_INT_FLAG_DTCRCERR)) {
            transerror = EMMC_DATA_CRC_ERROR;
        }
        if(RESET != sdio_interrupt_flag_get(SDIO, SDIO_INT_FLAG_DTTMOUT)) {
            transerror = EMMC_DATA_TIMEOUT;
        }
        if(RESET != sdio_interrupt_flag_get(SDIO, SDIO_INT_FLAG_TXURE)) {
            transerror = EMMC_TX_UNDERRUN_ERROR;
        }
        if(RESET != sdio_interrupt_flag_get(SDIO, SDIO_INT_FLAG_RXORE)) {
            transerror = EMMC_RX_OVERRUN_ERROR;
        }
        /* clear data flags */
        sdio_flag_clear(SDIO, SDIO_MASK_DATA_FLAGS);
        /* disable all the interrupts */
        sdio_interrupt_disable(SDIO, SDIO_INT_DTCRCERR | SDIO_INT_DTTMOUT | SDIO_INT_DTEND |
                               SDIO_INT_TXURE | SDIO_INT_RXORE);

        sdio_trans_start_disable(SDIO);
        sdio_fifo_reset_enable(SDIO);
        sdio_fifo_reset_disable(SDIO);
        /* send CMD12 to stop data transfer in multiple blocks operation */
        transerror = emmc_transfer_stop();
        sdio_flag_clear(SDIO, SDIO_FLAG_DTABORT);
        number_bytes = 0;

        if(transmode == EMMC_DMA_MODE) {
            sdio_idma_disable(SDIO);
        }
        return transerror;
    }

    if(RESET != sdio_interrupt_flag_get(SDIO, SDIO_INT_FLAG_IDMAERR)) {
        sdio_interrupt_flag_clear(SDIO, SDIO_INT_FLAG_IDMAERR);
        transerror = EMMC_DMA_ERROR;
        /* disable all the interrupts */
        sdio_interrupt_disable(SDIO, SDIO_INT_DTCRCERR | SDIO_INT_DTTMOUT | SDIO_INT_DTEND |
                               SDIO_INT_TFH | SDIO_INT_RFH | SDIO_INT_TXURE | SDIO_INT_RXORE);
        sdio_trans_start_disable(SDIO);
        sdio_fifo_reset_enable(SDIO);
        sdio_fifo_reset_disable(SDIO);
        /* send CMD12 to stop data transfer in multiple blocks operation */
        transerror = emmc_transfer_stop();
        sdio_flag_clear(SDIO, SDIO_FLAG_DTABORT);
        number_bytes = 0;

        sdio_idma_disable(SDIO);
        return transerror;
    }
    return transerror;
}

/*!
    \brief      select or deselect a card
    \param[in]  cardrca: the RCA of a card
    \param[out] none
    \retval     emmc_error_enum
*/
emmc_error_enum emmc_card_select_deselect(uint16_t cardrca)
{
    emmc_error_enum status = EMMC_OK;
    /* send CMD7(SELECT/DESELECT_CARD) to select or deselect the card */
    sdio_command_response_config(SDIO, EMMC_CMD_SELECT_DESELECT_CARD, (uint32_t)cardrca << EMMC_RCA_SHIFT, SDIO_RESPONSETYPE_SHORT);
    sdio_wait_type_set(SDIO, SDIO_WAITTYPE_NO);
    sdio_csm_enable(SDIO);

    status = r1_error_check(EMMC_CMD_SELECT_DESELECT_CARD);
    return status;
}

/*!
    \brief      get the EXT_CSD register detailed information of the eMMC
    \param[in]  none
    \param[out] none
    \retval     emmc_error_enum
*/
emmc_error_enum emmc_card_extcsd_get(void)
{
    uint32_t buf[128] = {0};
    emmc_error_enum status = EMMC_OK;
    uint32_t count = 0U, *ptempbuff = buf;

    /* configure SDIO data transmission */
    sdio_data_config(SDIO, EMMC_DATATIMEOUT, (uint32_t)512, SDIO_DATABLOCKSIZE_512BYTES);
    sdio_data_transfer_config(SDIO, SDIO_TRANSMODE_BLOCKCOUNT, SDIO_TRANSDIRECTION_TOSDIO);
    sdio_trans_start_enable(SDIO);


    /* send CMD8(READ_SINGLE_BLOCK) to read a block */
    sdio_command_response_config(SDIO, (uint32_t)EMMC_CMD_SEND_EXT_CSD, (uint32_t)0x0, SDIO_RESPONSETYPE_SHORT);
    sdio_wait_type_set(SDIO, SDIO_WAITTYPE_NO);
    sdio_csm_enable(SDIO);
    /* check if some error occurs */
    status = r1_error_check(EMMC_CMD_SEND_EXT_CSD);
    if(EMMC_OK != status) {
        return status;
    }

    /* polling mode */
    while(!sdio_flag_get(SDIO, SDIO_FLAG_DTCRCERR | SDIO_FLAG_DTTMOUT | SDIO_FLAG_RXORE | SDIO_FLAG_DTBLKEND | SDIO_FLAG_DTEND)) {
        if(RESET != sdio_flag_get(SDIO, SDIO_FLAG_RFH)) {
            /* at least 8 words can be read in the FIFO */
            for(count = 0U; count < EMMC_FIFOHALF_WORDS; count++) {
                *(ptempbuff + count) = sdio_data_read(SDIO);
            }
            ptempbuff += EMMC_FIFOHALF_WORDS;
        }
    }
    /*get EXT_CSD register data */
    memcpy(emmc_extcsd, (uint8_t *)(ptempbuff - 128U), 512U);

    sdio_trans_start_disable(SDIO);
    /* whether some error occurs and return it */
    if(RESET != sdio_flag_get(SDIO, SDIO_FLAG_DTCRCERR)) {
        status = EMMC_DATA_CRC_ERROR;
        sdio_flag_clear(SDIO, SDIO_FLAG_DTCRCERR);
        return status;

    } else if(RESET != sdio_flag_get(SDIO, SDIO_FLAG_DTTMOUT)) {
        status = EMMC_DATA_TIMEOUT;
        sdio_flag_clear(SDIO, SDIO_FLAG_DTTMOUT);
        return status;

    } else if(RESET != sdio_flag_get(SDIO, SDIO_FLAG_RXORE)) {
        status = EMMC_RX_OVERRUN_ERROR;
        sdio_flag_clear(SDIO, SDIO_FLAG_RXORE);
        return status;
    }
    /* clear the SDIO_INTC flags */
    sdio_flag_clear(SDIO, SDIO_MASK_INTC_FLAGS);
    return status;
}

/*!
    \brief      get the card status whose response format R1 contains a 32-bit field
    \param[in]  none
    \param[out] pcardstatus: a pointer that store card status
    \retval     emmc_error_enum
*/
emmc_error_enum emmc_cardstatus_get(uint32_t *pcardstatus)
{
    emmc_error_enum status = EMMC_OK;
    if(NULL == pcardstatus) {
        status = EMMC_PARAMETER_INVALID;
        return status;
    }

    /* send CMD13(SEND_STATUS), addressed card sends its status register */
    sdio_command_response_config(SDIO, EMMC_CMD_SEND_STATUS, (uint32_t)emmc_rca << EMMC_RCA_SHIFT, SDIO_RESPONSETYPE_SHORT);
    sdio_wait_type_set(SDIO, SDIO_WAITTYPE_NO);
    sdio_csm_enable(SDIO);
    /* check if some error occurs */
    status = r1_error_check(EMMC_CMD_SEND_STATUS);
    if(EMMC_OK != status) {
        return status;
    }

    *pcardstatus = sdio_response_get(SDIO, SDIO_RESPONSE0);
    return status;
}

/*!
    \brief      check if the command sent error occurs
    \param[in]  none
    \param[out] none
    \retval     emmc_error_enum
*/
static emmc_error_enum cmdsent_error_check(void)
{
    emmc_error_enum status = EMMC_OK;
    uint32_t timeout = 100000U;
    /* check command sent flag */
    while((RESET == sdio_flag_get(SDIO, SDIO_FLAG_CMDSEND)) && (timeout > 0U)) {
        --timeout;
    }
    /* command response is timeout */
    if(0U == timeout) {
        status = EMMC_CMD_RESP_TIMEOUT;
        return status;
    }
    /* if the command is sent, clear the CMD_FLAGS flags */
    sdio_flag_clear(SDIO, SDIO_MASK_CMD_FLAGS);
    return status;
}

/*!
    \brief      check if error type for R1 response
    \param[in]  resp: content of response
    \param[out] none
    \retval     emmc_error_enum
*/
static emmc_error_enum r1_error_type_check(uint32_t resp)
{
    emmc_error_enum status = EMMC_ERROR;
    /* check which error occurs */
    if(resp & EMMC_R1_OUT_OF_RANGE) {
        status = EMMC_OUT_OF_RANGE;
    } else if(resp & EMMC_R1_ADDRESS_ERROR) {
        status = EMMC_ADDRESS_ERROR;
    } else if(resp & EMMC_R1_BLOCK_LEN_ERROR) {
        status = EMMC_BLOCK_LEN_ERROR;
    } else if(resp & EMMC_R1_ERASE_SEQ_ERROR) {
        status = EMMC_ERASE_SEQ_ERROR;
    } else if(resp & EMMC_R1_ERASE_PARAM) {
        status = EMMC_ERASE_PARAM;
    } else if(resp & EMMC_R1_WP_VIOLATION) {
        status = EMMC_WP_VIOLATION;
    } else if(resp & EMMC_R1_LOCK_UNLOCK_FAILED) {
        status = EMMC_LOCK_UNLOCK_FAILED;
    } else if(resp & EMMC_R1_COM_CRC_ERROR) {
        status = EMMC_COM_CRC_ERROR;
    } else if(resp & EMMC_R1_ILLEGAL_COMMAND) {
        status = EMMC_ILLEGAL_COMMAND;
    } else if(resp & EMMC_R1_CARD_ECC_FAILED) {
        status = EMMC_CARD_ECC_FAILED;
    } else if(resp & EMMC_R1_CC_ERROR) {
        status = EMMC_CC_ERROR;
    } else if(resp & EMMC_R1_GENERAL_UNKNOWN_ERROR) {
        status = EMMC_GENERAL_UNKNOWN_ERROR;
    } else if(resp & EMMC_R1_CSD_OVERWRITE) {
        status = EMMC_CSD_OVERWRITE;
    } else if(resp & EMMC_R1_WP_ERASE_SKIP) {
        status = EMMC_WP_ERASE_SKIP;
    } else if(resp & EMMC_R1_CARD_ECC_DISABLED) {
        status = EMMC_CARD_ECC_DISABLED;
    } else if(resp & EMMC_R1_ERASE_RESET) {
        status = EMMC_ERASE_RESET;
    } else if(resp & EMMC_R1_AKE_SEQ_ERROR) {
        status = EMMC_AKE_SEQ_ERROR;
    } else {
        /*no todo,  if else end */
    }
    return status;
}

/*!
    \brief      check if error occurs for R1 response
    \param[in]  cmdindex: the index of command
    \param[out] none
    \retval     emmc_error_enum
*/
static emmc_error_enum r1_error_check(uint8_t cmdindex)
{
    emmc_error_enum status = EMMC_OK;
    uint32_t reg_status = 0U, resp_r1 = 0U;

    /* store the content of SDIO_STAT */
    reg_status = SDIO_STAT(SDIO);
    while(!(reg_status & (SDIO_FLAG_CCRCERR | SDIO_FLAG_CMDTMOUT | SDIO_FLAG_CMDRECV))) {
        reg_status = SDIO_STAT(SDIO);
    }
    /* check whether an error or timeout occurs or command response received */
    if(reg_status & SDIO_FLAG_CCRCERR) {
        status = EMMC_CMD_CRC_ERROR;
        sdio_flag_clear(SDIO, SDIO_FLAG_CCRCERR);
        return status;
    } else if(reg_status & SDIO_FLAG_CMDTMOUT) {
        status = EMMC_CMD_RESP_TIMEOUT;
        sdio_flag_clear(SDIO, SDIO_FLAG_CMDTMOUT);
        return status;
    } else {
        /* if else end */
    }

    /* check whether the last response command index is the desired one */
    if(sdio_command_index_get(SDIO) != cmdindex) {
        status = EMMC_ILLEGAL_COMMAND;
        return status;
    }
    /* clear all the CMD_FLAGS flags */
    sdio_flag_clear(SDIO, SDIO_MASK_CMD_FLAGS);
    /* get the SDIO response register 0 for checking */
    resp_r1 = sdio_response_get(SDIO, SDIO_RESPONSE0);
    if(EMMC_ALLZERO == (resp_r1 & EMMC_R1_ERROR_BITS)) {
        /* no error occurs, return EMMC_OK */
        status = EMMC_OK;
        return status;
    }

    /* if some error occurs, return the error type */
    status = r1_error_type_check(resp_r1);
    return status;
}

/*!
    \brief      check if error occurs for R2 response
    \param[in]  none
    \param[out] none
    \retval     emmc_error_enum
*/
static emmc_error_enum r2_error_check(void)
{
    emmc_error_enum status = EMMC_OK;
    uint32_t reg_status = 0U;

    /* store the content of SDIO_STAT */
    reg_status = SDIO_STAT(SDIO);
    while(!(reg_status & (SDIO_FLAG_CCRCERR | SDIO_FLAG_CMDTMOUT | SDIO_FLAG_CMDRECV))) {
        reg_status = SDIO_STAT(SDIO);
    }
    /* check whether an error or timeout occurs or command response received */
    if(reg_status & SDIO_FLAG_CCRCERR) {
        status = EMMC_CMD_CRC_ERROR;
        sdio_flag_clear(SDIO, SDIO_FLAG_CCRCERR);
        return status;
    } else if(reg_status & SDIO_FLAG_CMDTMOUT) {
        status = EMMC_CMD_RESP_TIMEOUT;
        sdio_flag_clear(SDIO, SDIO_FLAG_CMDTMOUT);
        return status;
    } else {
        /* if else end */
    }
    /* clear all the CMD_FLAGS flags */
    sdio_flag_clear(SDIO, SDIO_MASK_CMD_FLAGS);

    return status;
}

/*!
    \brief      check if error occurs for R3 response
    \param[in]  none
    \param[out] none
    \retval     emmc_error_enum
*/
static emmc_error_enum r3_error_check(void)
{
    emmc_error_enum status = EMMC_OK;
    uint32_t reg_status = 0U;

    /* store the content of SDIO_STAT */
    reg_status = SDIO_STAT(SDIO);
    while(!(reg_status & (SDIO_FLAG_CCRCERR | SDIO_FLAG_CMDTMOUT | SDIO_FLAG_CMDRECV))) {
        reg_status = SDIO_STAT(SDIO);
    }
    if(reg_status & SDIO_FLAG_CMDTMOUT) {
        status = EMMC_CMD_RESP_TIMEOUT;
        sdio_flag_clear(SDIO, SDIO_FLAG_CMDTMOUT);
        return status;
    }
    /* clear all the CMD_FLAGS flags */
    sdio_flag_clear(SDIO, SDIO_MASK_CMD_FLAGS);
    return status;
}

/*!
    \brief      get the state which the card is in
    \param[in]  none
    \param[out] pcardstate: a pointer that store the card state
      \arg        EMMC_CARDSTATE_IDLE: card is in idle state
      \arg        EMMC_CARDSTATE_READY: card is in ready state
      \arg        EMMC_CARDSTATE_IDENTIFICATION: card is in identification state
      \arg        EMMC_CARDSTATE_STANDBY: card is in standby state
      \arg        EMMC_CARDSTATE_TRANSFER: card is in transfer state
      \arg        EMMC_CARDSTATE_DATA: card is in data state
      \arg        EMMC_CARDSTATE_RECEIVING: card is in receiving state
      \arg        EMMC_CARDSTATE_PROGRAMMING: card is in programming state
      \arg        EMMC_CARDSTATE_DISCONNECT: card is in disconnect state
      \arg        EMMC_CARDSTATE_LOCKED: card is in locked state
    \retval     emmc_error_enum
*/
static emmc_error_enum emmc_card_state_get(uint8_t *pcardstate)
{
    emmc_error_enum status = EMMC_OK;
    __IO uint32_t reg_status = 0U, response = 0U;

    /* send CMD13(SEND_STATUS), addressed card sends its status register */
    sdio_command_response_config(SDIO, EMMC_CMD_SEND_STATUS, (uint32_t)emmc_rca << EMMC_RCA_SHIFT, SDIO_RESPONSETYPE_SHORT);
    sdio_wait_type_set(SDIO, SDIO_WAITTYPE_NO);
    sdio_csm_enable(SDIO);

    /* store the content of SDIO_STAT */
    reg_status = SDIO_STAT(SDIO);
    while(!(reg_status & (SDIO_FLAG_CCRCERR | SDIO_FLAG_CMDTMOUT | SDIO_FLAG_CMDRECV))) {
        reg_status = SDIO_STAT(SDIO);
    }
    /* check whether an error or timeout occurs or command response received */
    if(reg_status & SDIO_FLAG_CCRCERR) {
        status = EMMC_CMD_CRC_ERROR;
        sdio_flag_clear(SDIO, SDIO_FLAG_CCRCERR);
        return status;
    } else if(reg_status & SDIO_FLAG_CMDTMOUT) {
        status = EMMC_CMD_RESP_TIMEOUT;
        sdio_flag_clear(SDIO, SDIO_FLAG_CMDTMOUT);
        return status;
    } else {
        /* if else end */
    }

    /* command response received, store the response command index */
    reg_status = (uint32_t)sdio_command_index_get(SDIO);
    if(reg_status != (uint32_t)EMMC_CMD_SEND_STATUS) {
        status = EMMC_ILLEGAL_COMMAND;
        return status;
    }
    /* clear all the SDIO_INTC flags */
    sdio_flag_clear(SDIO, SDIO_MASK_INTC_FLAGS);
    /* get the SDIO response register 0 for checking */
    response = sdio_response_get(SDIO, SDIO_RESPONSE0);
    *pcardstate = (uint8_t)((response >> 9U) & 0x0000000FU);

    if(EMMC_ALLZERO == (response & EMMC_R1_ERROR_BITS)) {
        /* no error occurs, return EMMC_OK */
        status = EMMC_OK;
        return status;
    }

    /* if some error occurs, return the error type */
    status = r1_error_type_check(response);
    return status;
}

/*!
    \brief      stop an ongoing data transfer
    \param[in]  none
    \param[out] none
    \retval     emmc_error_enum
*/
emmc_error_enum emmc_transfer_stop(void)
{
    emmc_error_enum status = EMMC_OK;
    /* send CMD12(STOP_TRANSMISSION) to stop transmission */
    sdio_command_response_config(SDIO, EMMC_CMD_STOP_TRANSMISSION, (uint32_t)0x0U, SDIO_RESPONSETYPE_SHORT);
    sdio_wait_type_set(SDIO, SDIO_WAITTYPE_NO);
    sdio_trans_stop_enable(SDIO);
    sdio_csm_enable(SDIO);
    /* check if some error occurs */
    status = r1_error_check(EMMC_CMD_STOP_TRANSMISSION);
    sdio_trans_stop_disable(SDIO);
    return status;
}

/*!
    \brief      get the data block size
    \param[in]  bytesnumber: the number of bytes
    \param[out] none
    \retval     data block size
      \arg        SDIO_DATABLOCKSIZE_1BYTE: block size = 1 byte
      \arg        SDIO_DATABLOCKSIZE_2BYTES: block size = 2 bytes
      \arg        SDIO_DATABLOCKSIZE_4BYTES: block size = 4 bytes
      \arg        SDIO_DATABLOCKSIZE_8BYTES: block size = 8 bytes
      \arg        SDIO_DATABLOCKSIZE_16BYTES: block size = 16 bytes
      \arg        SDIO_DATABLOCKSIZE_32BYTES: block size = 32 bytes
      \arg        SDIO_DATABLOCKSIZE_64BYTES: block size = 64 bytes
      \arg        SDIO_DATABLOCKSIZE_128BYTES: block size = 128 bytes
      \arg        SDIO_DATABLOCKSIZE_256BYTES: block size = 256 bytes
      \arg        SDIO_DATABLOCKSIZE_512BYTES: block size = 512 bytes
      \arg        SDIO_DATABLOCKSIZE_1024BYTES: block size = 1024 bytes
      \arg        SDIO_DATABLOCKSIZE_2048BYTES: block size = 2048 bytes
      \arg        SDIO_DATABLOCKSIZE_4096BYTES: block size = 4096 bytes
      \arg        SDIO_DATABLOCKSIZE_8192BYTES: block size = 8192 bytes
      \arg        SDIO_DATABLOCKSIZE_16384BYTES: block size = 16384 bytes
*/
static uint32_t emmc_datablocksize_get(uint16_t bytesnumber)
{
    uint8_t exp_val = 0U;
    /* calculate the exponent of 2 */
    while(1U != bytesnumber) {
        bytesnumber >>= 1U;
        ++exp_val;
    }
    return DATACTL_BLKSZ(exp_val);
}

/*!
    \brief      set the host and card into interrupt mode
    \param[in]  none
    \param[out] none
    \retval     emmc_error_enum
*/
emmc_error_enum emmc_interrupt_mode_entry(void)
{
    emmc_error_enum status = EMMC_OK;
    /* send CMD40(GO_IRQ_STATE) to set the host and card into interrupt mode */
    sdio_command_response_config(SDIO, EMMC_CMD_GO_IRQ_STATE, (uint32_t)0, SDIO_RESPONSETYPE_SHORT);
    sdio_wait_type_set(SDIO, SDIO_WAITTYPE_INTERRUPT);
    sdio_csm_enable(SDIO);

    return status;
}
/*!
    \brief      set the host and card out of interrupt mode
    \param[in]  none
    \param[out] none
    \retval     emmc_error_enum
*/
emmc_error_enum emmc_interrupt_mode_exit(void)
{
    emmc_error_enum status = EMMC_OK;
    sdio_csm_disable(SDIO);
    /* send CMD40(GO_IRQ_STATE) to set the host and card out of interrupt mode */
    sdio_command_response_config(SDIO, EMMC_CMD_GO_IRQ_STATE, (uint32_t)0, SDIO_RESPONSETYPE_NO);
    sdio_wait_type_set(SDIO, SDIO_WAITTYPE_NO);
    sdio_csm_enable(SDIO);
    
    /* check if command sent error occurs */
    status = cmdsent_error_check();
    if(EMMC_OK != status){
        return status;
    }

    return status;
}

/*!
    \brief      configure the GPIO of SDIO interface
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void gpio_config(void)
{
    /* configure the SDIO_DAT0(PC8), SDIO_DAT1(PC9), SDIO_DAT2(PC10), SDIO_DAT3(PC11),
        SDIO_DAT4(PB8), SDIO_DAT5(PB9), SDIO_DAT6(PC6), SDIO_DAT7(PC7),  SDIO_CLK(PC12) and SDIO_CMD(PD2)  */
    gpio_af_set(GPIOB, GPIO_AF_12, GPIO_PIN_8 | GPIO_PIN_9);
    gpio_af_set(GPIOC, GPIO_AF_12, GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12);
    gpio_af_set(GPIOD, GPIO_AF_12, GPIO_PIN_2);


    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_8 | GPIO_PIN_9);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_8 | GPIO_PIN_9);

    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ,  GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11);

    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_12);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_12);

    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_2);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_2);
}

/*!
    \brief      configure the RCU of SDIO
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void rcu_config(void)
{
    /* SDIO clock 200M */
    /* configure the pll1 input and output clock range */
    rcu_pll_input_output_clock_range_config(IDX_PLL1, RCU_PLL1RNG_4M_8M, RCU_PLL1VCO_192M_836M);
    /* configure the PLL1 clock: CK_PLL1P/CK_PLL1Q/CK_PLL1R = HXTAL_VALUE / 5 * 40 / 1 */
    rcu_pll1_config(5, 40, 1, 1, 1);
    /* enable PLL1R clock output */
    rcu_pll_clock_output_enable(RCU_PLL1R);
    /* enable PLL1 clock */
    rcu_osci_on(RCU_PLL1_CK);

    if(ERROR == rcu_osci_stab_wait(RCU_PLL1_CK)) {
        while(1) {
        }
    }

    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_sdio_clock_config(IDX_SDIO0, RCU_SDIO0SRC_PLL1R);
    rcu_periph_clock_enable(RCU_SDIO0);
}

/*!
    \brief      configure the IDMA
    \param[in]  srcbuf: a pointer point to a buffer which will be transferred
    \param[in]  bufsize: the size of buffer(not used in flow controller is peripheral)
    \param[out] none
    \retval     none
*/
static void dma_config(uint32_t *srcbuf, uint32_t bufsize)
{
    sdio_idma_set(SDIO, SDIO_IDMA_SINGLE_BUFFER, bufsize);
    sdio_idma_buffer0_address_set(SDIO, (uint32_t)srcbuf);
}
