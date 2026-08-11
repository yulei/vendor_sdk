/*!
    \file    emmc.h
    \brief   the header file of eMMC driver

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

#ifndef EMMC_H
#define EMMC_H

#define SDIO SDIO0

#include "gd32h73x_75x.h"

extern uint16_t emmc_rca;
/* eMMC memory card bus commands index */
#define EMMC_CMD_GO_IDLE_STATE                  ((uint8_t)0)   /* CMD0, GO_IDLE_STATE */
#define EMMC_CMD_SEND_OP_COND                   ((uint8_t)1)   /* CMD1, SEND_OP_COND */
#define EMMC_CMD_ALL_SEND_CID                   ((uint8_t)2)   /* CMD2, ALL_SEND_CID */
#define EMMC_CMD_SEND_RELATIVE_ADDR             ((uint8_t)3)   /* CMD3, SEND_RELATIVE_ADDR */
#define EMMC_CMD_SET_DSR                        ((uint8_t)4)   /* CMD4, SET_DSR */
#define EMMC_CMD_SWITCH_FUNC                    ((uint8_t)6)   /* CMD6, SWITCH */
#define EMMC_CMD_SELECT_DESELECT_CARD           ((uint8_t)7)   /* CMD7, SELECT_DESELECT_CARD */
#define EMMC_CMD_SEND_EXT_CSD                   ((uint8_t)8)   /* CMD8, SEND_EXT_CSD */
#define EMMC_CMD_SEND_CSD                       ((uint8_t)9)   /* CMD9, SEND_CSD */
#define EMMC_CMD_SEND_CID                       ((uint8_t)10)  /* CMD10, SEND_CID */
#define EMMC_CMD_STOP_TRANSMISSION              ((uint8_t)12)  /* CMD12, STOP_TRANSMISSION */
#define EMMC_CMD_SEND_STATUS                    ((uint8_t)13)  /* CMD13, SEND_STATUS */
#define EMMC_CMD_GO_INACTIVE_STATE              ((uint8_t)15)  /* CMD15, GO_INACTIVE_STATE */
#define EMMC_CMD_SET_BLOCKLEN                   ((uint8_t)16)  /* CMD16, SET_BLOCKLEN */
#define EMMC_CMD_READ_SINGLE_BLOCK              ((uint8_t)17)  /* CMD17, READ_SINGLE_BLOCK */
#define EMMC_CMD_READ_MULTIPLE_BLOCK            ((uint8_t)18)  /* CMD18, READ_MULTIPLE_BLOCK */
#define EMMC_SET_BLOCK_COUNT                    ((uint8_t)23)  /* CMD23, SET_BLOCK_COUNT */
#define EMMC_CMD_WRITE_BLOCK                    ((uint8_t)24)  /* CMD24, WRITE_BLOCK */
#define EMMC_CMD_WRITE_MULTIPLE_BLOCK           ((uint8_t)25)  /* CMD25, WRITE_MULTIPLE_BLOCK */
#define EMMC_CMD_PROG_CSD                       ((uint8_t)27)  /* CMD27, PROG_CSD */
#define EMMC_CMD_SET_WRITE_PROT                 ((uint8_t)28)  /* CMD28, SET_WRITE_PROT */
#define EMMC_CMD_CLR_WRITE_PROT                 ((uint8_t)29)  /* CMD29, CLR_WRITE_PROT */
#define EMMC_CMD_SEND_WRITE_PROT                ((uint8_t)30)  /* CMD30, SEND_WRITE_PROT */
#define EMMC_CMD_SEND_WRITE_PROT_TYPE           ((uint8_t)31)  /* CMD31, SEND_WRITE_PROT_TYPE */
#define EMMC_CMD_ERASE_GROUP_START              ((uint8_t)35)  /* CMD35, ERASE_GROUP_START */
#define EMMC_CMD_ERASE_GROUP_END                ((uint8_t)36)  /* CMD36, ERASE_GROUP_END */
#define EMMC_CMD_ERASE                          ((uint8_t)38)  /* CMD38, ERASE */
#define EMMC_CMD_GO_IRQ_STATE                   ((uint8_t)40)  /* CMD40, GO_IRQ_STATE */
#define EMMC_CMD_LOCK_UNLOCK                    ((uint8_t)42)  /* CMD42, LOCK_UNLOCK */
#define EMMC_CMD_APP_CMD                        ((uint8_t)55)  /* CMD55, APP_CMD */
#define EMMC_CMD_GEN_CMD                        ((uint8_t)56)  /* CMD56, GEN_CMD */

/* eMMC card data transmission mode */
#define EMMC_DMA_MODE                           ((uint32_t)0x00000000) /* DMA mode */
#define EMMC_POLLING_MODE                       ((uint32_t)0x00000001) /* polling mode */

/* lock unlock status */
#define EMMC_LOCK                               ((uint8_t)0x05)        /* lock the eMMC */
#define EMMC_UNLOCK                             ((uint8_t)0x02)        /* unlock the eMMC */

/* eMMC error flags */
typedef enum {
    EMMC_OUT_OF_RANGE = 0,                  /* command's argument was out of range */
    EMMC_ADDRESS_ERROR,                     /* misaligned address which did not match the block length */
    EMMC_BLOCK_LEN_ERROR,                   /* transferred block length is not allowed for the card or the number of transferred bytes does not match the block length */
    EMMC_ERASE_SEQ_ERROR,                   /* an error in the sequence of erase command occurs */
    EMMC_ERASE_PARAM,                       /* an invalid selection of write-blocks for erase occurred */
    EMMC_WP_VIOLATION,                      /* attempt to program a write protect block or permanent write protected card */
    EMMC_LOCK_UNLOCK_FAILED,                /* sequence or password error has been detected in lock/unlock card command */
    EMMC_COM_CRC_ERROR,                     /* CRC check of the previous command failed */
    EMMC_ILLEGAL_COMMAND,                   /* command not legal for the card state */
    EMMC_CARD_ECC_FAILED,                   /* card internal ECC was applied but failed to correct the data */
    EMMC_CC_ERROR,                          /* internal card controller error */
    EMMC_GENERAL_UNKNOWN_ERROR,             /* general or unknown error occurred during the operation */
    EMMC_CSD_OVERWRITE,                     /* read only section of the CSD does not match the card content or an attempt to reverse the copy or permanent WP bits was made */
    EMMC_WP_ERASE_SKIP,                     /* only partial address space was erased or the temporary or permanent write protected card was erased */
    EMMC_CARD_ECC_DISABLED,                 /* command has been executed without using internal ECC */
    EMMC_ERASE_RESET,                       /* erase sequence was cleared before executing because an out of erase sequence command was received */
    EMMC_AKE_SEQ_ERROR,                     /* error in the sequence of the authentication process */

    EMMC_CMD_CRC_ERROR,                     /* command response received (CRC check failed) */
    EMMC_DATA_CRC_ERROR,                    /* data block sent/received (CRC check failed) */
    EMMC_CMD_RESP_TIMEOUT,                  /* command response timeout */
    EMMC_DATA_TIMEOUT,                      /* data timeout */
    EMMC_TX_UNDERRUN_ERROR,                 /* transmit FIFO underrun error occurs */
    EMMC_RX_OVERRUN_ERROR,                  /* received FIFO overrun error occurs */
    EMMC_START_BIT_ERROR,                   /* start bit error in the bus */

    EMMC_VOLTRANGE_INVALID,                 /* the voltage range is invalid */
    EMMC_PARAMETER_INVALID,                 /* the parameter is invalid */
    EMMC_OPERATION_IMPROPER,                /* the operation is improper */
    EMMC_FUNCTION_UNSUPPORTED,              /* the function is unsupported */
    EMMC_ERROR,                             /* an error occurred */
    EMMC_DMA_ERROR,                         /* an error when idma transfer */
    EMMC_OK                                 /* no error occurred */
} emmc_error_enum;

extern uint32_t cardcapacity;               /* eMMC capacity type */
extern uint32_t speedlevel;                 /* eMMC speed level */
extern uint32_t emmc_csd[4];                /* content of CSD register */
extern uint32_t emmc_cid[4];                /* content of CID register */
extern uint8_t  emmc_extcsd[512];           /* content of EXT_CSD register */

/* function declarations */
/* initialize the eMMC and make it in standby state */
emmc_error_enum emmc_init(void);
/* initialize the card and get CID and CSD of the card */
emmc_error_enum emmc_card_init(void);
/* configure the clock and the work voltage, and get the card type */
emmc_error_enum emmc_power_on(void);
/* close the power of SDIO */
emmc_error_enum emmc_power_off(void);
/* configure the bus mode */
emmc_error_enum emmc_bus_mode_config(uint32_t busmode, uint32_t datarate);
/* configure the mode of transmission */
emmc_error_enum emmc_transfer_mode_config(uint32_t txmode);

/* read a block data into a buffer from the specified address of a card */
emmc_error_enum emmc_block_read(uint32_t *preadbuffer, uint32_t readaddr, uint16_t blocksize);
/* read multiple blocks data into a buffer from the specified address of a card */
emmc_error_enum emmc_multiblocks_read(uint32_t *preadbuffer, uint32_t readaddr, uint16_t blocksize, uint32_t blocksnumber);
/* write a block data to the specified address of a card */
emmc_error_enum emmc_block_write(uint32_t *pwritebuffer, uint32_t writeaddr, uint16_t blocksize);
/* write multiple blocks data to the specified address of a card */
emmc_error_enum emmc_multiblocks_write(uint32_t *pwritebuffer, uint32_t writeaddr, uint16_t blocksize, uint32_t blocksnumber);
/* erase a continuous area of a card */
emmc_error_enum emmc_erase(uint32_t startaddr, uint32_t endaddr);
/* process all the interrupts which the corresponding flags are set */
emmc_error_enum emmc_interrupts_process(void);

/* select or deselect a card */
emmc_error_enum emmc_card_select_deselect(uint16_t cardrca);
/* get the EXT_CSD register detailed information of the eMMC */
emmc_error_enum emmc_card_extcsd_get(void);
/* get the card status whose response format R1 contains a 32-bit field */
emmc_error_enum emmc_cardstatus_get(uint32_t *pcardstatus);
/* get the eMMC status  */
emmc_error_enum emmc_status_get(uint32_t *psdstatus);
/* stop an ongoing data transfer */
emmc_error_enum emmc_transfer_stop(void);
/* set the host and card into interrupt mode */
emmc_error_enum emmc_interrupt_mode_entry(void);
/* set the host and card out of interrupt mode */
emmc_error_enum emmc_interrupt_mode_exit(void);
#endif /* EMMC_H */
