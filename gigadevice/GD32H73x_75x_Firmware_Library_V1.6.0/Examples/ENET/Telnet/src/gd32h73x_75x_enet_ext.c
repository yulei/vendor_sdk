/*!
    \file    gd32h73x_75x_enet_ext.c
    \brief   ENET driver extension

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

#include "gd32h73x_75x_enet.h"
#include <stdlib.h>

#if defined   (__CC_ARM)                                    /*!< ARM compiler */

__attribute__((section(".ARM.__at_0x30000000")))  volatile enet_descriptors_struct  rxdesc_tab0[ENET_RXBUF_NUM];        /*!< ENET0 RxDMA descriptor */
__attribute__((section(".ARM.__at_0x30000160")))  volatile enet_descriptors_struct  txdesc_tab0[ENET_TXBUF_NUM];        /*!< ENET0 TxDMA descriptor */
__attribute__((section(".ARM.__at_0x30000300")))  volatile uint8_t rx_buff0[ENET_RXBUF_NUM][ENET_RXBUF_SIZE];           /*!< ENET0 receive buffer */
__attribute__((section(".ARM.__at_0x30002100")))  volatile uint8_t tx_buff0[ENET_TXBUF_NUM][ENET_TXBUF_SIZE];           /*!< ENET0 transmit buffer */

__attribute__((section(".ARM.__at_0x30000000")))  volatile enet_descriptors_struct  rxdesc_tab1[ENET_RXBUF_NUM];        /*!< ENET1 RxDMA descriptor */
__attribute__((section(".ARM.__at_0x30000160")))  volatile enet_descriptors_struct  txdesc_tab1[ENET_TXBUF_NUM];        /*!< ENET1 TxDMA descriptor */
__attribute__((section(".ARM.__at_0x30000300")))  volatile uint8_t rx_buff1[ENET_RXBUF_NUM][ENET_RXBUF_SIZE];           /*!< ENET1 receive buffer */
__attribute__((section(".ARM.__at_0x30002100")))  volatile uint8_t tx_buff1[ENET_TXBUF_NUM][ENET_TXBUF_SIZE];           /*!< ENET1 transmit buffer */

#elif defined ( __ICCARM__ )                                /*!< IAR compiler */

#pragma location=0x30000000
volatile enet_descriptors_struct  rxdesc_tab0[ENET_RXBUF_NUM];                                                          /*!< ENET0 RxDMA descriptor */
#pragma location=0x30000160
volatile enet_descriptors_struct  txdesc_tab0[ENET_TXBUF_NUM];                                                          /*!< ENET0 TxDMA descriptor */
#pragma location=0x30000300
volatile uint8_t rx_buff0[ENET_RXBUF_NUM][ENET_RXBUF_SIZE];                                                             /*!< ENET0 receive buffer */
#pragma location=0x30002100
volatile uint8_t tx_buff0[ENET_TXBUF_NUM][ENET_TXBUF_SIZE];                                                             /*!< ENET0 transmit buffer */

#pragma location=0x30004000
volatile enet_descriptors_struct  rxdesc_tab1[ENET_RXBUF_NUM];                                                          /*!< ENET1 RxDMA descriptor */
#pragma location=0x30004160
volatile enet_descriptors_struct  txdesc_tab1[ENET_TXBUF_NUM];                                                          /*!< ENET1 TxDMA descriptor */
#pragma location=0x30004300
volatile uint8_t rx_buff1[ENET_RXBUF_NUM][ENET_RXBUF_SIZE];                                                             /*!< ENET1 receive buffer */
#pragma location=0x30006100
volatile uint8_t tx_buff1[ENET_TXBUF_NUM][ENET_TXBUF_SIZE];                                                             /*!< ENET1 transmit buffer */


#elif defined (__clang__)                                    /*! ARM clang compiler */
volatile enet_descriptors_struct  rxdesc_tab0[ENET_RXBUF_NUM] __attribute__((section(".ARM.__at_0x30000000")));         /*!< ENET0 RxDMA descriptor */
volatile enet_descriptors_struct  txdesc_tab0[ENET_TXBUF_NUM] __attribute__((section(".ARM.__at_0x30000160")));         /*!< ENET0 TxDMA descriptor */
volatile uint8_t rx_buff0[ENET_RXBUF_NUM][ENET_RXBUF_SIZE]    __attribute__((section(".ARM.__at_0x30000300")));         /*!< ENET0 receive buffer */
volatile uint8_t tx_buff0[ENET_TXBUF_NUM][ENET_TXBUF_SIZE]    __attribute__((section(".ARM.__at_0x30002100")));         /*!< ENET0 transmit buffer */

volatile enet_descriptors_struct  rxdesc_tab1[ENET_RXBUF_NUM] __attribute__((section(".ARM.__at_0x30004000")));         /*!< ENET1 RxDMA descriptor */
volatile enet_descriptors_struct  txdesc_tab1[ENET_TXBUF_NUM] __attribute__((section(".ARM.__at_0x30004160")));         /*!< ENET1 TxDMA descriptor */
volatile uint8_t rx_buff1[ENET_RXBUF_NUM][ENET_RXBUF_SIZE]    __attribute__((section(".ARM.__at_0x30004300")));         /*!< ENET1 receive buffer */
volatile uint8_t tx_buff1[ENET_TXBUF_NUM][ENET_TXBUF_SIZE]    __attribute__((section(".ARM.__at_0x30006100")));         /*!< ENET1 transmit buffer */

#elif defined (__GNUC__)                                    /*! GNU compiler */

volatile enet_descriptors_struct  rxdesc_tab0[ENET_RXBUF_NUM] __attribute__((section(".sram0.__at_0x30000000")));       /*!< ENET0 RxDMA descriptor */
volatile enet_descriptors_struct  txdesc_tab0[ENET_TXBUF_NUM] __attribute__((section(".sram0.__at_0x30000160")));       /*!< ENET0 TxDMA descriptor */
volatile uint8_t rx_buff0[ENET_RXBUF_NUM][ENET_RXBUF_SIZE]    __attribute__((section(".sram0.__at_0x30000300")));       /*!< ENET0 receive buffer */
volatile uint8_t tx_buff0[ENET_TXBUF_NUM][ENET_TXBUF_SIZE]    __attribute__((section(".sram0.__at_0x30002100")));       /*!< ENET0 transmit buffer */

volatile enet_descriptors_struct  rxdesc_tab1[ENET_RXBUF_NUM] __attribute__((section(".sram1.__at_0x30004000")));       /*!< ENET1 RxDMA descriptor */
volatile enet_descriptors_struct  txdesc_tab1[ENET_TXBUF_NUM] __attribute__((section(".sram1.__at_0x30004160")));       /*!< ENET1 TxDMA descriptor */
volatile uint8_t rx_buff1[ENET_RXBUF_NUM][ENET_RXBUF_SIZE]    __attribute__((section(".sram1.__at_0x30004300")));       /*!< ENET1 receive buffer */
volatile uint8_t tx_buff1[ENET_TXBUF_NUM][ENET_TXBUF_SIZE]    __attribute__((section(".sram1.__at_0x30006100")));       /*!< ENET1 transmit buffer */        /*!< ENET1 transmit buffer */

#endif /* __CC_ARM */

/* ENET0 global transmit and receive descriptors pointers */
volatile enet_descriptors_struct  *dma_current_txdesc0 = NULL;
volatile enet_descriptors_struct  *dma_current_rxdesc0 = NULL;

/* ENET1 global transmit and receive descriptors pointers */
volatile enet_descriptors_struct  *dma_current_txdesc1 = NULL;
volatile enet_descriptors_struct  *dma_current_rxdesc1 = NULL;

/* ENET0 structure pointer of ptp descriptor for normal mode */
volatile enet_descriptors_struct  *dma_current_ptp_txdesc0 = NULL;
volatile enet_descriptors_struct  *dma_current_ptp_rxdesc0 = NULL;

/* ENET1 structure pointer of ptp descriptor for normal mode */
volatile enet_descriptors_struct  *dma_current_ptp_txdesc1 = NULL;
volatile enet_descriptors_struct  *dma_current_ptp_rxdesc1 = NULL;

static uint32_t enet_unknow_err = 0U;

/* change nanosecond to subsecond */
static uint32_t enet_ptp_nanosecond_2_subsecond(uint32_t nanosecond);

#define DESC_FLAG_SET_MASK                  (0xFE302080U)


/*!
    \brief      check receive frame valid and return frame size, API ID: 0x0001
    \param[in]  enet_periph: ENETx(x=0,1)
    \param[out] none
    \retval     size of received frame: 0x0 - 0x3FFF
*/
uint32_t enet_rxframe_size_get(uint32_t enet_periph)
{
    uint32_t size = 0U;
    uint32_t status;

    /* get rdes0 information of current RxDMA descriptor */
    if(ENET0 == enet_periph) {
        /* get rdes0 information of current RxDMA descriptor */
        status = dma_current_rxdesc0->status;
    } else {
        /* get rdes0 information of current RxDMA descriptor */
        status = dma_current_rxdesc1->status;
    }

    /* if the descriptor is owned by DMA */
    if((uint32_t)RESET != (status & ENET_RDES0_DAV)) {
        size = 0U;
    }

    /* if has any error, or the frame uses two or more descriptors */
    else if((((uint32_t)RESET) != (status & ENET_RDES0_ERRS)) ||
            (((uint32_t)RESET) == (status & ENET_RDES0_LDES)) ||
            (((uint32_t)RESET) == (status & ENET_RDES0_FDES))) {
        /* drop current receive frame */
        enet_rxframe_drop(enet_periph);

        size = 1U;
    }

#if SELECT_DESCRIPTORS_ENHANCED_MODE
    /* if is an ethernet-type frame, and IP frame payload error occurred */
    else if(ENET0 == enet_periph) {
        if(((uint32_t)RESET) != (dma_current_rxdesc0->status & ENET_RDES0_FRMT) &&
                ((uint32_t)RESET) != (dma_current_rxdesc0->extended_status & ENET_RDES4_IPPLDERR)) {
            /* drop current receive frame */
            enet_rxframe_drop(enet_periph);

            size = 1U;
        }
    } else if(ENET1 == enet_periph) {
        if(((uint32_t)RESET) != (dma_current_rxdesc1->status & ENET_RDES0_FRMT) &&
                ((uint32_t)RESET) != (dma_current_rxdesc1->extended_status & ENET_RDES4_IPPLDERR)) {
            /* drop current receive frame */
            enet_rxframe_drop(enet_periph);

            size = 1U;
        }
    }
#else
    /* if is an ethernet-type frame, and IP frame payload error occurred */
    else if((((uint32_t)RESET) != (status & ENET_RDES0_FRMT)) &&
            (((uint32_t)RESET) != (status & ENET_RDES0_PCERR))) {
        /* drop current receive frame */
        enet_rxframe_drop(enet_periph);

        size = 1U;
    }
#endif /* SELECT_DESCRIPTORS_ENHANCED_MODE */

    /* if CPU owns current descriptor, no error occurred, the frame uses only one descriptor */
    else if((((uint32_t)RESET) == (status & ENET_RDES0_DAV)) &&
            (((uint32_t)RESET) == (status & ENET_RDES0_ERRS)) &&
            (((uint32_t)RESET) != (status & ENET_RDES0_LDES)) &&
            (((uint32_t)RESET) != (status & ENET_RDES0_FDES))) {
        /* get the size of the received data including CRC */
        size = GET_RDES0_FRML(status);
        /* subtract the CRC size */
        size = size - 4U;

        /* if is a type frame, and CRC is not included in forwarding frame */
        if((RESET != (ENET_MAC_CFG(enet_periph) & ENET_MAC_CFG_TFCD)) && (RESET != (status & ENET_RDES0_FRMT))) {
            size = size + 4U;
        }
    } else {
        enet_unknow_err++;
        enet_rxframe_drop(enet_periph);

        size = 1U;
    }

    /* return packet size */
    return size;
}

/*!
    \brief      initialize the DMA Tx/Rx descriptors's parameters in chain mode, API ID: 0x0002
    \param[in]  enet_periph: ENETx(x=0,1)
    \param[in]  direction: the descriptors which users want to init, refer to enet_dmadirection_enum,
                only one parameter can be selected which is shown as below
      \arg        ENET_DMA_TX: DMA Tx descriptors
      \arg        ENET_DMA_RX: DMA Rx descriptors
    \param[out] none
    \retval     none
*/
void enet_descriptors_chain_init(uint32_t enet_periph, enet_dmadirection_enum direction)
{
    uint32_t num = 0U, count = 0U;
    uint32_t desc_status = 0U, desc_bufsize = 0U;
    volatile enet_descriptors_struct *desc;
    volatile enet_descriptors_struct *desc_tab;

    if(ENET0 == enet_periph) {
        /* if want to initialize DMA Tx descriptors */
        if(ENET_DMA_TX == direction) {
            /* save a copy of the DMA Tx descriptors */
            desc_tab = txdesc_tab0;
            count = ENET_TXBUF_NUM;

            /* select chain mode */
            desc_status = ENET_TDES0_TCHM;

            /* configure DMA Tx descriptor table address register */
            ENET_DMA_TDTADDR(enet_periph) = (uint32_t)desc_tab;
            dma_current_txdesc0 = desc_tab;
        } else {
            /* if want to initialize DMA Rx descriptors */
            /* save a copy of the DMA Rx descriptors */
            desc_tab = rxdesc_tab0;
            count = ENET_RXBUF_NUM;

            /* enable receiving */
            desc_status = ENET_RDES0_DAV;
            /* select receive chained mode and set buffer1 size */
            desc_bufsize = ENET_RDES1_RCHM | (uint32_t)ENET_RXBUF_SIZE;

            /* configure DMA Rx descriptor table address register */
            ENET_DMA_RDTADDR(enet_periph) = (uint32_t)desc_tab;
            dma_current_rxdesc0 = desc_tab;
        }

        /* configure each descriptor */
        for(num = 0U; num < count; num++) {
            /* get the pointer to the next descriptor of the descriptor table */
            if(ENET_DMA_TX == direction) {
                desc = &txdesc_tab0[num];
                desc->buffer1_addr = (uint32_t)(&tx_buff0[num][0]);
            } else {
                desc = &rxdesc_tab0[num];
                desc->buffer1_addr = (uint32_t)(&rx_buff0[num][0]);
            }

            /* configure descriptors */
            desc->status = desc_status;
            desc->control_buffer_size = desc_bufsize;

            /* if is not the last descriptor */
            if(num < (count - 1U)) {
                /* configure the next descriptor address */
                if(ENET_DMA_TX == direction) {
                    desc->buffer2_next_desc_addr = (uint32_t)(&txdesc_tab0[num + 1U]);
                } else {
                    desc->buffer2_next_desc_addr = (uint32_t)(&rxdesc_tab0[num + 1U]);
                }
            } else {
                /* when it is the last descriptor, the next descriptor address
                equals to first descriptor address in descriptor table */
                desc->buffer2_next_desc_addr = (uint32_t)desc_tab;
            }
        }
    } else {
        /* if want to initialize DMA Tx descriptors */
        if(ENET_DMA_TX == direction) {
            /* save a copy of the DMA Tx descriptors */
            desc_tab = txdesc_tab1;
            count = ENET_TXBUF_NUM;

            /* select chain mode */
            desc_status = ENET_TDES0_TCHM;

            /* configure DMA Tx descriptor table address register */
            ENET_DMA_TDTADDR(enet_periph) = (uint32_t)desc_tab;
            dma_current_txdesc1 = desc_tab;
        } else {
            /* if want to initialize DMA Rx descriptors */
            /* save a copy of the DMA Rx descriptors */
            desc_tab = rxdesc_tab1;
            count = ENET_RXBUF_NUM;

            /* enable receiving */
            desc_status = ENET_RDES0_DAV;
            /* select receive chained mode and set buffer1 size */
            desc_bufsize = ENET_RDES1_RCHM | (uint32_t)ENET_RXBUF_SIZE;

            /* configure DMA Rx descriptor table address register */
            ENET_DMA_RDTADDR(enet_periph) = (uint32_t)desc_tab;
            dma_current_rxdesc1 = desc_tab;
        }

        /* configure each descriptor */
        for(num = 0U; num < count; num++) {
            /* get the pointer to the next descriptor of the descriptor table */
            if(ENET_DMA_TX == direction) {
                desc = &txdesc_tab1[num];
                desc->buffer1_addr = (uint32_t)(&tx_buff1[num][0]);
            } else {
                desc = &rxdesc_tab1[num];
                desc->buffer1_addr = (uint32_t)(&rx_buff1[num][0]);
            }

            /* configure descriptors */
            desc->status = desc_status;
            desc->control_buffer_size = desc_bufsize;

            /* if is not the last descriptor */
            if(num < (count - 1U)) {
                /* configure the next descriptor address */
                if(ENET_DMA_TX == direction) {
                    desc->buffer2_next_desc_addr = (uint32_t)(&txdesc_tab1[num + 1U]);
                } else {
                    desc->buffer2_next_desc_addr = (uint32_t)(&rxdesc_tab1[num + 1U]);
                }
            } else {
                /* when it is the last descriptor, the next descriptor address
                equals to first descriptor address in descriptor table */
                desc->buffer2_next_desc_addr = (uint32_t) desc_tab;
            }
        }
    }
}

/*!
    \brief      initialize the DMA Tx/Rx descriptors's parameters in ring mode, API ID: 0x0003
    \param[in]  enet_periph: ENETx(x=0,1)
    \param[in]  direction: the descriptors which users want to init, refer to enet_dmadirection_enum,
                only one parameter can be selected which is shown as below
      \arg        ENET_DMA_TX: DMA Tx descriptors
      \arg        ENET_DMA_RX: DMA Rx descriptors
    \param[out] none
    \retval     none
*/
void enet_descriptors_ring_init(uint32_t enet_periph, enet_dmadirection_enum direction)
{
    uint32_t num = 0U, count = 0U;
    uint32_t desc_status = 0U, desc_bufsize = 0U;
    volatile enet_descriptors_struct *desc;
    volatile enet_descriptors_struct *desc_tab;

    /* configure descriptor skip length */
    ENET_DMA_BCTL(enet_periph) &= ~ENET_DMA_BCTL_DPSL;
    ENET_DMA_BCTL(enet_periph) |= DMA_BCTL_DPSL(0);

    if(ENET0 == enet_periph) {
        /* if want to initialize DMA Tx descriptors */
        if(ENET_DMA_TX == direction) {
            /* save a copy of the DMA Tx descriptors */
            desc_tab = txdesc_tab0;
            count = ENET_TXBUF_NUM;

            /* configure DMA Tx descriptor table address register */
            ENET_DMA_TDTADDR(enet_periph) = (uint32_t)desc_tab;
            dma_current_txdesc0 = desc_tab;
        } else {
            /* if want to initialize DMA Rx descriptors */
            /* save a copy of the DMA Rx descriptors */
            desc_tab = rxdesc_tab0;
            count = ENET_RXBUF_NUM;

            /* enable receiving */
            desc_status = ENET_RDES0_DAV;
            /* set buffer1 size */
            desc_bufsize = ENET_RXBUF_SIZE;

            /* configure DMA Rx descriptor table address register */
            ENET_DMA_RDTADDR(enet_periph) = (uint32_t)desc_tab;
            dma_current_rxdesc0 = desc_tab;
        }

        /* configure each descriptor */
        for(num = 0U; num < count; num++) {
            /* get the pointer to the next descriptor of the descriptor table */
            if(ENET_DMA_TX == direction) {
                desc = &txdesc_tab0[num];
                desc->buffer1_addr = (uint32_t)(&tx_buff0[num][0]);
            } else {
                desc = &rxdesc_tab0[num];
                desc->buffer1_addr = (uint32_t)(&rx_buff0[num][0]);
            }

            /* configure descriptors */
            desc->status = desc_status;
            desc->control_buffer_size = desc_bufsize;

            /* when it is the last descriptor */
            if(num == (count - 1U)) {
                if(ENET_DMA_TX == direction) {
                    /* configure transmit end of ring mode */
                    desc->status |= ENET_TDES0_TERM;
                } else {
                    /* configure receive end of ring mode */
                    desc->control_buffer_size |= ENET_RDES1_RERM;
                }
            }
        }
    } else {
        /* if want to initialize DMA Tx descriptors */
        if(ENET_DMA_TX == direction) {
            /* save a copy of the DMA Tx descriptors */
            desc_tab = txdesc_tab1;
            count = ENET_TXBUF_NUM;

            /* configure DMA Tx descriptor table address register */
            ENET_DMA_TDTADDR(enet_periph) = (uint32_t)desc_tab;
            dma_current_txdesc1 = desc_tab;
        } else {
            /* if want to initialize DMA Rx descriptors */
            /* save a copy of the DMA Rx descriptors */
            desc_tab = rxdesc_tab1;
            count = ENET_RXBUF_NUM;

            /* enable receiving */
            desc_status = ENET_RDES0_DAV;
            /* set buffer1 size */
            desc_bufsize = ENET_RXBUF_SIZE;

            /* configure DMA Rx descriptor table address register */
            ENET_DMA_RDTADDR(enet_periph) = (uint32_t)desc_tab;
            dma_current_rxdesc1 = desc_tab;
        }

        /* configure each descriptor */
        for(num = 0U; num < count; num++) {
            /* get the pointer to the next descriptor of the descriptor table */
            if(ENET_DMA_TX == direction) {
                desc = &txdesc_tab1[num];
                desc->buffer1_addr = (uint32_t)(&tx_buff1[num][0]);
            } else {
                desc = &rxdesc_tab1[num];
                desc->buffer1_addr = (uint32_t)(&rx_buff1[num][0]);
            }

            /* configure descriptors */
            desc->status = desc_status;
            desc->control_buffer_size = desc_bufsize;

            /* when it is the last descriptor */
            if(num == (count - 1U)) {
                if(ENET_DMA_TX == direction) {
                    /* configure transmit end of ring mode */
                    desc->status |= ENET_TDES0_TERM;
                } else {
                    /* configure receive end of ring mode */
                    desc->control_buffer_size |= ENET_RDES1_RERM;
                }
            }
        }
    }
}

/*!
    \brief      handle current received frame data to application buffer, API ID: 0x0004
    \param[in]  enet_periph: ENETx(x=0,1)
    \param[in]  bufsize: the size of buffer which is the parameter in function
    \param[out] buffer: pointer to the received frame data
                note -- if the input is NULL, user should copy data in application by himself
    \retval     ErrStatus: SUCCESS or ERROR
*/
ErrStatus enet_frame_receive(uint32_t enet_periph, uint8_t buffer[], uint32_t bufsize)
{
    uint32_t offset = 0U, size = 0U;
    ErrStatus status = ERROR;

    if(ENET0 == enet_periph) {
        /* the descriptor is busy due to own by the DMA */
        if((uint32_t)RESET == (dma_current_rxdesc0->status & ENET_RDES0_DAV)) {
            /* if buffer pointer is null, indicates that users has copied data in application */
            if(NULL != buffer) {
                /* if no error occurs, and the frame uses only one descriptor */
                if((((uint32_t)RESET) == (dma_current_rxdesc0->status & ENET_RDES0_ERRS)) &&
                        (((uint32_t)RESET) != (dma_current_rxdesc0->status & ENET_RDES0_LDES)) &&
                        (((uint32_t)RESET) != (dma_current_rxdesc0->status & ENET_RDES0_FDES))) {
                    /* get the frame length except CRC */
                    size = GET_RDES0_FRML(dma_current_rxdesc0->status);
                    size = size - 4U;

                    /* if is a type frame, and CRC is not included in forwarding frame */
                    if((RESET != (ENET_MAC_CFG(enet_periph) & ENET_MAC_CFG_TFCD)) && (RESET != (dma_current_rxdesc0->status & ENET_RDES0_FRMT))) {
                        size = size + 4U;
                    }

                    /* to avoid situation that the frame size exceeds the buffer length */
                    if(size <= bufsize) {
                        /* copy data from Rx buffer to application buffer */
                        for(offset = 0U; offset < size; offset++) {
                            buffer[offset] = (*(__IO uint8_t *)(uint32_t)((dma_current_rxdesc0->buffer1_addr) + offset));
                        }
                        status = SUCCESS;
                    }
                }
            } else {
                /* If buffer is NULL, user will handle data in application */
                status = SUCCESS;
            }

            /* enable reception, descriptor is owned by DMA */
            dma_current_rxdesc0->status = ENET_RDES0_DAV;

            /* check Rx buffer unavailable flag status */
            if((uint32_t)RESET != (ENET_DMA_STAT(enet_periph) & ENET_DMA_STAT_RBU)) {
                /* clear RBU flag */
                ENET_DMA_STAT(enet_periph) = ENET_DMA_STAT_RBU;
                /* resume DMA reception by writing to the RPEN register*/
                ENET_DMA_RPEN(enet_periph) = 0U;
            }

            /* update the current RxDMA descriptor pointer to the next descriptor in RxDMA descriptor table */
            /* chained mode */
            if((uint32_t)RESET != (dma_current_rxdesc0->control_buffer_size & ENET_RDES1_RCHM)) {
                dma_current_rxdesc0 = (enet_descriptors_struct *)(dma_current_rxdesc0->buffer2_next_desc_addr);
            } else {
                /* ring mode */
                if((uint32_t)RESET != (dma_current_rxdesc0->control_buffer_size & ENET_RDES1_RERM)) {
                    /* if is the last descriptor in table, the next descriptor is the table header */
                    dma_current_rxdesc0 = (enet_descriptors_struct *)(ENET_DMA_RDTADDR(enet_periph));
                } else {
                    /* the next descriptor is the current address, add the descriptor size, and descriptor skip length */
                    dma_current_rxdesc0 = (enet_descriptors_struct *)(uint32_t)((uint32_t)dma_current_rxdesc0 + ETH_DMARXDESC_SIZE + (GET_DMA_BCTL_DPSL(ENET_DMA_BCTL(
                                              enet_periph))));
                }
            }
        }
    } else {
        /* the descriptor is busy due to own by the DMA */
        if((uint32_t)RESET == (dma_current_rxdesc1->status & ENET_RDES0_DAV)) {
            /* if buffer pointer is null, indicates that users has copied data in application */
            if(NULL != buffer) {
                /* if no error occurs, and the frame uses only one descriptor */
                if((((uint32_t)RESET) == (dma_current_rxdesc1->status & ENET_RDES0_ERRS)) &&
                        (((uint32_t)RESET) != (dma_current_rxdesc1->status & ENET_RDES0_LDES)) &&
                        (((uint32_t)RESET) != (dma_current_rxdesc1->status & ENET_RDES0_FDES))) {
                    /* get the frame length except CRC */
                    size = GET_RDES0_FRML(dma_current_rxdesc1->status);
                    size = size - 4U;

                    /* if is a type frame, and CRC is not included in forwarding frame */
                    if((RESET != (ENET_MAC_CFG(enet_periph) & ENET_MAC_CFG_TFCD)) && (RESET != (dma_current_rxdesc1->status & ENET_RDES0_FRMT))) {
                        size = size + 4U;
                    }

                    /* to avoid situation that the frame size exceeds the buffer length */
                    if(size <= bufsize) {
                        /* copy data from Rx buffer to application buffer */
                        for(offset = 0U; offset < size; offset++) {
                            buffer[offset] = (*(__IO uint8_t *)(uint32_t)((dma_current_rxdesc1->buffer1_addr) + offset));
                        }
                        status = SUCCESS;
                    }
                }
            } else {
                /* If buffer is NULL, user will handle data in application */
                status = SUCCESS;
            }

            /* enable reception, descriptor is owned by DMA */
            dma_current_rxdesc1->status = ENET_RDES0_DAV;

            /* check Rx buffer unavailable flag status */
            if((uint32_t)RESET != (ENET_DMA_STAT(enet_periph) & ENET_DMA_STAT_RBU)) {
                /* clear RBU flag */
                ENET_DMA_STAT(enet_periph) = ENET_DMA_STAT_RBU;
                /* resume DMA reception by writing to the RPEN register*/
                ENET_DMA_RPEN(enet_periph) = 0U;
            }

            /* update the current RxDMA descriptor pointer to the next descriptor in RxDMA descriptor table */
            /* chained mode */
            if((uint32_t)RESET != (dma_current_rxdesc1->control_buffer_size & ENET_RDES1_RCHM)) {
                dma_current_rxdesc1 = (enet_descriptors_struct *)(dma_current_rxdesc1->buffer2_next_desc_addr);
            } else {
                /* ring mode */
                if((uint32_t)RESET != (dma_current_rxdesc1->control_buffer_size & ENET_RDES1_RERM)) {
                    /* if is the last descriptor in table, the next descriptor is the table header */
                    dma_current_rxdesc1 = (enet_descriptors_struct *)(ENET_DMA_RDTADDR(enet_periph));
                } else {
                    /* the next descriptor is the current address, add the descriptor size, and descriptor skip length */
                    dma_current_rxdesc1 = (enet_descriptors_struct *)(uint32_t)((uint32_t)dma_current_rxdesc1 + ETH_DMARXDESC_SIZE + (GET_DMA_BCTL_DPSL(ENET_DMA_BCTL(
                                              enet_periph))));
                }
            }
        }
    }

    return status;
}

/*!
    \brief      handle application buffer data to transmit it, API ID: 0x0005
    \param[in]  enet_periph: ENETx(x=0,1)
    \param[in]  buffer: pointer to the frame data to be transmitted,
                note -- if the input is NULL, user should handle the data in application by himself
    \param[in]  length: the length of frame data to be transmitted
    \param[out] none
    \retval     ErrStatus: SUCCESS or ERROR
*/
ErrStatus enet_frame_transmit(uint32_t enet_periph, uint8_t buffer[], uint32_t length)
{
    uint32_t offset = 0U;
    uint32_t dma_tbu_flag, dma_tu_flag;
    ErrStatus status = SUCCESS;

    if(ENET0 == enet_periph) {
        /* the descriptor is busy due to own by the DMA */
        if(((uint32_t)RESET == (dma_current_txdesc0->status & ENET_TDES0_DAV)) &&
                (length <= ENET_MAX_FRAME_SIZE)) {

            /* if buffer pointer is null, indicates that users has handled data in application */
            if(NULL != buffer) {
                /* copy frame data from application buffer to Tx buffer */
                for(offset = 0U; offset < length; offset++) {
                    (*(__IO uint8_t *)(uint32_t)((dma_current_txdesc0->buffer1_addr) + offset)) = buffer[offset];
                }
            }

            /* set the frame length */
            dma_current_txdesc0->control_buffer_size = length;
            /* set the segment of frame, frame is transmitted in one descriptor */
            dma_current_txdesc0->status |= ENET_TDES0_LSG | ENET_TDES0_FSG;
            /* enable the DMA transmission */
            dma_current_txdesc0->status |= ENET_TDES0_DAV;

            /* check Tx buffer unavailable flag status */
            dma_tbu_flag = (ENET_DMA_STAT(enet_periph) & ENET_DMA_STAT_TBU);
            dma_tu_flag = (ENET_DMA_STAT(enet_periph) & ENET_DMA_STAT_TU);

            if((RESET != dma_tbu_flag) || (RESET != dma_tu_flag)) {
                /* clear TBU and TU flag */
                ENET_DMA_STAT(enet_periph) = (dma_tbu_flag | dma_tu_flag);
                /* resume DMA transmission by writing to the TPEN register*/
                ENET_DMA_TPEN(enet_periph) = 0U;
            }

            /* update the current TxDMA descriptor pointer to the next descriptor in TxDMA descriptor table*/
            /* chained mode */
            if((uint32_t)RESET != (dma_current_txdesc0->status & ENET_TDES0_TCHM)) {
                dma_current_txdesc0 = (enet_descriptors_struct *)(dma_current_txdesc0->buffer2_next_desc_addr);
            } else {
                /* ring mode */
                if((uint32_t)RESET != (dma_current_txdesc0->status & ENET_TDES0_TERM)) {
                    /* if is the last descriptor in table, the next descriptor is the table header */
                    dma_current_txdesc0 = (enet_descriptors_struct *)(ENET_DMA_TDTADDR(enet_periph));
                } else {
                    /* the next descriptor is the current address, add the descriptor size, and descriptor skip length */
                    dma_current_txdesc0 = (enet_descriptors_struct *)(uint32_t)((uint32_t)dma_current_txdesc0 + ETH_DMATXDESC_SIZE + (GET_DMA_BCTL_DPSL(ENET_DMA_BCTL(
                                              enet_periph))));
                }
            }
        } else {
            status = ERROR;
        }
    } else {
        /* the descriptor is busy due to own by the DMA */
        if(((uint32_t)RESET == (dma_current_txdesc1->status & ENET_TDES0_DAV)) &&
                (length <= ENET_MAX_FRAME_SIZE)) {

            /* if buffer pointer is null, indicates that users has handled data in application */
            if(NULL != buffer) {
                /* copy frame data from application buffer to Tx buffer */
                for(offset = 0U; offset < length; offset++) {
                    (*(__IO uint8_t *)(uint32_t)((dma_current_txdesc1->buffer1_addr) + offset)) = buffer[offset];
                }
            }

            /* set the frame length */
            dma_current_txdesc1->control_buffer_size = length;
            /* set the segment of frame, frame is transmitted in one descriptor */
            dma_current_txdesc1->status |= ENET_TDES0_LSG | ENET_TDES0_FSG;
            /* enable the DMA transmission */
            dma_current_txdesc1->status |= ENET_TDES0_DAV;

            /* check Tx buffer unavailable flag status */
            dma_tbu_flag = (ENET_DMA_STAT(enet_periph) & ENET_DMA_STAT_TBU);
            dma_tu_flag = (ENET_DMA_STAT(enet_periph) & ENET_DMA_STAT_TU);

            if((RESET != dma_tbu_flag) || (RESET != dma_tu_flag)) {
                /* clear TBU and TU flag */
                ENET_DMA_STAT(enet_periph) = (dma_tbu_flag | dma_tu_flag);
                /* resume DMA transmission by writing to the TPEN register*/
                ENET_DMA_TPEN(enet_periph) = 0U;
            }

            /* update the current TxDMA descriptor pointer to the next descriptor in TxDMA descriptor table*/
            /* chained mode */
            if((uint32_t)RESET != (dma_current_txdesc1->status & ENET_TDES0_TCHM)) {
                dma_current_txdesc1 = (enet_descriptors_struct *)(dma_current_txdesc1->buffer2_next_desc_addr);
            } else {
                /* ring mode */
                if((uint32_t)RESET != (dma_current_txdesc1->status & ENET_TDES0_TERM)) {
                    /* if is the last descriptor in table, the next descriptor is the table header */
                    dma_current_txdesc1 = (enet_descriptors_struct *)(ENET_DMA_TDTADDR(enet_periph));
                } else {
                    /* the next descriptor is the current address, add the descriptor size, and descriptor skip length */
                    dma_current_txdesc1 = (enet_descriptors_struct *)(uint32_t)((uint32_t)dma_current_txdesc1 + ETH_DMATXDESC_SIZE + (GET_DMA_BCTL_DPSL(ENET_DMA_BCTL(
                                              enet_periph))));
                }
            }
        } else {
            status = ERROR;
        }
    }

    return status;
}

/*!
    \brief      configure the transmit IP frame checksum offload calculation and insertion, API ID: 0x0006
    \param[in]  desc: the descriptor pointer which users want to configure
    \param[in]  checksum: IP frame checksum configuration
                only one parameter can be selected which is shown as below
      \arg        ENET_CHECKSUM_DISABLE: checksum insertion disabled
      \arg        ENET_CHECKSUM_IPV4HEADER: only IP header checksum calculation and insertion are enabled
      \arg        ENET_CHECKSUM_TCPUDPICMP_SEGMENT: TCP/UDP/ICMP checksum insertion calculated but pseudo-header
      \arg        ENET_CHECKSUM_TCPUDPICMP_FULL: TCP/UDP/ICMP checksum insertion fully calculated
    \param[out] none
    \retval     ErrStatus: ERROR, SUCCESS
*/
ErrStatus enet_transmit_checksum_config(enet_descriptors_struct *desc, uint32_t checksum)
{
    ErrStatus ret = ERROR;

    desc->status &= ~ENET_TDES0_CM;
    desc->status |= (checksum & ENET_TDES0_CM);
    ret = SUCCESS;

    return ret;
}

/*!
    \brief      ENET Tx and Rx function enable (include MAC and DMA module), API ID: 0x0007
    \param[in]  enet_periph: ENETx(x=0,1)
    \param[out] none
    \retval     none
*/
void enet_enable(uint32_t enet_periph)
{
    enet_tx_enable(enet_periph);
    enet_rx_enable(enet_periph);
}

/*!
    \brief      ENET Tx and Rx function disable (include MAC and DMA module), API ID: 0x0008
    \param[in]  enet_periph: ENETx(x=0,1)
    \param[out] none
    \retval     none
*/
void enet_disable(uint32_t enet_periph)
{
    enet_tx_disable(enet_periph);
    enet_rx_disable(enet_periph);
}

/*!
    \brief      enable the loopback function of PHY chip, API ID: 0x0009
    \param[in]  enet_periph: ENETx(x=0,1)
    \param[out] none
    \retval     ErrStatus: ERROR or SUCCESS
*/
ErrStatus enet_phyloopback_enable(uint32_t enet_periph, uint16_t phy_address)
{
    uint16_t temp_phy = 0U;
    ErrStatus phy_state = ERROR;

    /* get the PHY configuration to update it */
    enet_phy_write_read(enet_periph, ENET_PHY_READ, phy_address, PHY_REG_BCR, &temp_phy);

    /* enable the PHY loopback mode */
    temp_phy |= PHY_LOOPBACK;

    /* update the PHY control register with the new configuration */
    phy_state = enet_phy_write_read(enet_periph, ENET_PHY_WRITE, phy_address, PHY_REG_BCR, &temp_phy);

    return phy_state;
}

/*!
    \brief      disable the loopback function of PHY chip, API ID: 0x000A
    \param[in]  enet_periph: ENETx(x=0,1)
    \param[out] none
    \retval     ErrStatus: ERROR or SUCCESS
*/
ErrStatus enet_phyloopback_disable(uint32_t enet_periph, uint16_t phy_address)
{
    uint16_t temp_phy = 0U;
    ErrStatus phy_state = ERROR;

    /* get the PHY configuration to update it */
    enet_phy_write_read(enet_periph, ENET_PHY_READ, phy_address, PHY_REG_BCR, &temp_phy);

    /* disable the PHY loopback mode */
    temp_phy &= (uint16_t)~PHY_LOOPBACK;

    /* update the PHY control register with the new configuration */
    phy_state = enet_phy_write_read(enet_periph, ENET_PHY_WRITE, phy_address, PHY_REG_BCR, &temp_phy);

    return phy_state;
}

/*!
    \brief      check and recover the Rx process, API ID: 0x000B
    \param[in]  enet_periph: ENETx(x=0,1)
    \param[out] none
    \retval     none
*/
void enet_rxprocess_check_recovery(uint32_t enet_periph)
{
    uint32_t status;

    if(ENET0 == enet_periph) {
        /* get DAV information of current RxDMA descriptor */
        status = dma_current_rxdesc0->status;
        status &= ENET_RDES0_DAV;

        /* if current descriptor is owned by DMA, but the descriptor address mismatches with
        receive descriptor address pointer updated by RxDMA controller */
        if((ENET_DMA_CRDADDR(enet_periph) != ((uint32_t)dma_current_rxdesc0)) && (ENET_RDES0_DAV == status)) {
            dma_current_rxdesc0 = (enet_descriptors_struct *)ENET_DMA_CRDADDR(enet_periph);
        }
    } else {
        /* get DAV information of current RxDMA descriptor */
        status = dma_current_rxdesc1->status;
        status &= ENET_RDES0_DAV;

        /* if current descriptor is owned by DMA, but the descriptor address mismatches with
        receive descriptor address pointer updated by RxDMA controller */
        if((ENET_DMA_CRDADDR(enet_periph) != ((uint32_t)dma_current_rxdesc1)) && (ENET_RDES0_DAV == status)) {
            dma_current_rxdesc1 = (enet_descriptors_struct *)ENET_DMA_CRDADDR(enet_periph);
        }
    }
}

/*!
    \brief      get the Tx or Rx descriptor information, API ID: 0x000C
    \param[in]  enet_periph: ENETx(x=0,1)
    \param[in]  desc: the descriptor pointer which users want to get information
    \param[in]  info_get: the descriptor information type which is selected,
                only one parameter can be selected which is shown as below
      \arg        RXDESC_BUFFER_1_SIZE: receive buffer 1 size
      \arg        RXDESC_BUFFER_2_SIZE: receive buffer 2 size
      \arg        RXDESC_FRAME_LENGTH: the byte length of the received frame that was transferred to the buffer
      \arg        TXDESC_COLLISION_COUNT: the number of collisions occurred before the frame was transmitted
      \arg        RXDESC_BUFFER_1_ADDR: the buffer1 address of the Rx frame
      \arg        TXDESC_BUFFER_1_ADDR: the buffer1 address of the Tx frame
    \param[out] none
    \retval     descriptor information, if value is 0xFFFFFFFFU, means the false input parameter
*/
uint32_t enet_desc_information_get(uint32_t enet_periph, enet_descriptors_struct *desc, enet_descstate_enum info_get)
{
    uint32_t reval = 0xFFFFFFFFU;

    switch(info_get) {
    case RXDESC_BUFFER_1_SIZE:
        reval = GET_RDES1_RB1S(desc->control_buffer_size);
        break;
    case RXDESC_BUFFER_2_SIZE:
        reval = GET_RDES1_RB2S(desc->control_buffer_size);
        break;
    case RXDESC_FRAME_LENGTH:
        reval = GET_RDES0_FRML(desc->status);
        if(reval > 4U) {
            reval = reval - 4U;

            /* if is a type frame, and CRC is not included in forwarding frame */
            if((RESET != (ENET_MAC_CFG(enet_periph) & ENET_MAC_CFG_TFCD)) && (RESET != (desc->status & ENET_RDES0_FRMT))) {
                reval = reval + 4U;
            }
        } else {
            reval = 0U;
        }

        break;
    case RXDESC_BUFFER_1_ADDR:
        reval = desc->buffer1_addr;
        break;
    case TXDESC_BUFFER_1_ADDR:
        reval = desc->buffer1_addr;
        break;
    case TXDESC_COLLISION_COUNT:
        reval = GET_TDES0_COCNT(desc->status);
        break;
    default:
        break;
    }
    return reval;
}

/*!
    \brief      get the bit flag of ENET DMA descriptor, API ID: 0x000D
    \param[in]  desc: the descriptor pointer which users want to get flag
    \param[in]  desc_flag: the bit flag of ENET DMA descriptor,
                only one parameter can be selected which is shown as below
      \arg        ENET_TDES0_DB: deferred
      \arg        ENET_TDES0_UFE: underflow error
      \arg        ENET_TDES0_EXD: excessive deferral
      \arg        ENET_TDES0_VFRM: VLAN frame
      \arg        ENET_TDES0_ECO: excessive collision
      \arg        ENET_TDES0_LCO: late collision
      \arg        ENET_TDES0_NCA: no carrier
      \arg        ENET_TDES0_LCA: loss of carrier
      \arg        ENET_TDES0_IPPE: IP payload error
      \arg        ENET_TDES0_FRMF: frame flushed
      \arg        ENET_TDES0_JT: jabber timeout
      \arg        ENET_TDES0_ES: error summary
      \arg        ENET_TDES0_IPHE: IP header error
      \arg        ENET_TDES0_TTMSS: transmit timestamp status
      \arg        ENET_TDES0_TCHM: the second address chained mode
      \arg        ENET_TDES0_TERM: transmit end of ring mode
      \arg        ENET_TDES0_TTSEN: transmit timestamp function enable
      \arg        ENET_TDES0_DPAD: disable adding pad
      \arg        ENET_TDES0_DCRC: disable CRC
      \arg        ENET_TDES0_FSG: first segment
      \arg        ENET_TDES0_LSG: last segment
      \arg        ENET_TDES0_INTC: interrupt on completion
      \arg        ENET_TDES0_DAV: DAV bit

      \arg        ENET_RDES0_PCERR: payload checksum error
      \arg        ENET_RDES0_EXSV: extended status valid
      \arg        ENET_RDES0_CERR: CRC error
      \arg        ENET_RDES0_DBERR: dribble bit error
      \arg        ENET_RDES0_RERR: receive error
      \arg        ENET_RDES0_RWDT: receive watchdog timeout
      \arg        ENET_RDES0_FRMT: frame type
      \arg        ENET_RDES0_LCO: late collision
      \arg        ENET_RDES0_IPHERR: IP frame header error
      \arg        ENET_RDES0_TSV: timestamp valid
      \arg        ENET_RDES0_LDES: last descriptor
      \arg        ENET_RDES0_FDES: first descriptor
      \arg        ENET_RDES0_VTAG: VLAN tag
      \arg        ENET_RDES0_OERR: overflow error
      \arg        ENET_RDES0_LERR: length error
      \arg        ENET_RDES0_SAFF: SA filter fail
      \arg        ENET_RDES0_DERR: descriptor error
      \arg        ENET_RDES0_ERRS: error summary
      \arg        ENET_RDES0_DAFF: destination address filter fail
      \arg        ENET_RDES0_DAV: descriptor available
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus enet_desc_flag_get(enet_descriptors_struct *desc, uint32_t desc_flag)
{
    FlagStatus enet_flag = RESET;

    if((uint32_t)RESET != (desc->status & desc_flag)) {
        enet_flag = SET;
    }

    return enet_flag;
}

/*!
    \brief      set the bit flag of ENET DMA descriptor, API ID: 0x000E
    \param[in]  desc: the descriptor pointer which users want to set flag
    \param[in]  desc_flag: the bit flag of ENET DMA descriptor,
                only one parameter can be selected which is shown as below
      \arg        ENET_TDES0_VFRM: VLAN frame
      \arg        ENET_TDES0_FRMF: frame flushed
      \arg        ENET_TDES0_TCHM: the second address chained mode
      \arg        ENET_TDES0_TERM: transmit end of ring mode
      \arg        ENET_TDES0_TTSEN: transmit timestamp function enable
      \arg        ENET_TDES0_DPAD: disable adding pad
      \arg        ENET_TDES0_DCRC: disable CRC
      \arg        ENET_TDES0_FSG: first segment
      \arg        ENET_TDES0_LSG: last segment
      \arg        ENET_TDES0_INTC: interrupt on completion
      \arg        ENET_TDES0_DAV: DAV bit
      \arg        ENET_RDES0_DAV: descriptor available
    \param[out] none
    \retval     none
*/
void enet_desc_flag_set(enet_descriptors_struct *desc, uint32_t desc_flag)
{
    desc->status |= (desc_flag & DESC_FLAG_SET_MASK);
}

/*!
    \brief      clear the bit flag of ENET DMA descriptor, API ID: 0x000F
    \param[in]  desc: the descriptor pointer which users want to clear flag
    \param[in]  desc_flag: the bit flag of ENET DMA descriptor,
                only one parameter can be selected which is shown as below
      \arg        ENET_TDES0_VFRM: VLAN frame
      \arg        ENET_TDES0_FRMF: frame flushed
      \arg        ENET_TDES0_TCHM: the second address chained mode
      \arg        ENET_TDES0_TERM: transmit end of ring mode
      \arg        ENET_TDES0_TTSEN: transmit timestamp function enable
      \arg        ENET_TDES0_DPAD: disable adding pad
      \arg        ENET_TDES0_DCRC: disable CRC
      \arg        ENET_TDES0_FSG: first segment
      \arg        ENET_TDES0_LSG: last segment
      \arg        ENET_TDES0_INTC: interrupt on completion
      \arg        ENET_TDES0_DAV: DAV bit
      \arg        ENET_RDES0_DAV: descriptor available
    \param[out] none
    \retval     none
*/
void enet_desc_flag_clear(enet_descriptors_struct *desc, uint32_t desc_flag)
{
    desc->status &= ~(desc_flag & DESC_FLAG_SET_MASK);

}

/*!
    \brief      when receiving completed, set RS bit in ENET_DMA_STAT register will immediately set, API ID: 0x0010
    \param[in]  desc: the descriptor pointer which users want to configure
    \param[out] none
    \retval     none
*/
void enet_rx_desc_immediate_receive_complete_interrupt(enet_descriptors_struct *desc)
{
    desc->control_buffer_size &= ~ENET_RDES1_DINTC;
}

/*!
    \brief      when receiving completed, set RS bit in ENET_DMA_STAT register will be set after a configurable delay time, API ID: 0x0011
    \param[in]  enet_periph: ENETx(x=0,1)
    \param[in]  desc: the descriptor pointer which users want to configure
    \param[in]  delay_time: delay a time of 256*delay_time HCLK, this value must be between 0 and 0xFF
    \param[out] none
    \retval     none
*/
void enet_rx_desc_delay_receive_complete_interrupt(uint32_t enet_periph, enet_descriptors_struct *desc, uint32_t delay_time)
{
    desc->control_buffer_size |= ENET_RDES1_DINTC;
    ENET_DMA_RSWDC(enet_periph) = DMA_RSWDC_WDCFRS(delay_time);
}

/*!
    \brief      drop current receive frame, API ID: 0x0012
    \param[in]  enet_periph: ENETx(x=0,1)
    \param[out] none
    \retval     none
*/
void enet_rxframe_drop(uint32_t enet_periph)
{

    if(ENET0 == enet_periph) {
        /* enable reception, descriptor is owned by DMA */
        dma_current_rxdesc0->status = ENET_RDES0_DAV;

        /* chained mode */
        if((uint32_t)RESET != (dma_current_rxdesc0->control_buffer_size & ENET_RDES1_RCHM)) {
            if(NULL != dma_current_ptp_rxdesc0) {
                dma_current_rxdesc0 = (enet_descriptors_struct *)(dma_current_ptp_rxdesc0->buffer2_next_desc_addr);
                /* if it is the last ptp descriptor */
                if(0U != dma_current_ptp_rxdesc0->status) {
                    /* pointer back to the first ptp descriptor address in the desc_ptptab list address */
                    dma_current_ptp_rxdesc0 = (enet_descriptors_struct *)(dma_current_ptp_rxdesc0->status);
                } else {
                    /* pointer to the next ptp descriptor */
                    dma_current_ptp_rxdesc0++;
                }
            } else {
                dma_current_rxdesc0 = (enet_descriptors_struct *)(dma_current_rxdesc0->buffer2_next_desc_addr);
            }

        } else {
            /* ring mode */
            if((uint32_t)RESET != (dma_current_rxdesc0->control_buffer_size & ENET_RDES1_RERM)) {
                /* if is the last descriptor in table, the next descriptor is the table header */
                dma_current_rxdesc0 = (enet_descriptors_struct *)(ENET_DMA_RDTADDR(enet_periph));
                if(NULL != dma_current_ptp_rxdesc0) {
                    dma_current_ptp_rxdesc0 = (enet_descriptors_struct *)(dma_current_ptp_rxdesc0->status);
                }
            } else {
                /* the next descriptor is the current address, add the descriptor size, and descriptor skip length */
                dma_current_rxdesc0 = (enet_descriptors_struct *)(uint32_t)((uint32_t)dma_current_rxdesc0 + ETH_DMARXDESC_SIZE + GET_DMA_BCTL_DPSL(ENET_DMA_BCTL(
                                          enet_periph)));
                if(NULL != dma_current_ptp_rxdesc0) {
                    dma_current_ptp_rxdesc0++;
                }
            }
        }
    } else {
        /* enable reception, descriptor is owned by DMA */
        dma_current_rxdesc1->status = ENET_RDES0_DAV;

        /* chained mode */
        if((uint32_t)RESET != (dma_current_rxdesc1->control_buffer_size & ENET_RDES1_RCHM)) {
            if(NULL != dma_current_ptp_rxdesc1) {
                dma_current_rxdesc1 = (enet_descriptors_struct *)(dma_current_ptp_rxdesc1->buffer2_next_desc_addr);
                /* if it is the last ptp descriptor */
                if(0U != dma_current_ptp_rxdesc1->status) {
                    /* pointer back to the first ptp descriptor address in the desc_ptptab list address */
                    dma_current_ptp_rxdesc1 = (enet_descriptors_struct *)(dma_current_ptp_rxdesc1->status);
                } else {
                    /* pointer to the next ptp descriptor */
                    dma_current_ptp_rxdesc1++;
                }
            } else {
                dma_current_rxdesc1 = (enet_descriptors_struct *)(dma_current_rxdesc1->buffer2_next_desc_addr);
            }

        } else {
            /* ring mode */
            if((uint32_t)RESET != (dma_current_rxdesc1->control_buffer_size & ENET_RDES1_RERM)) {
                /* if is the last descriptor in table, the next descriptor is the table header */
                dma_current_rxdesc1 = (enet_descriptors_struct *)(ENET_DMA_RDTADDR(enet_periph));
                if(NULL != dma_current_ptp_rxdesc1) {
                    dma_current_ptp_rxdesc1 = (enet_descriptors_struct *)(dma_current_ptp_rxdesc1->status);
                }
            } else {
                /* the next descriptor is the current address, add the descriptor size, and descriptor skip length */
                dma_current_rxdesc1 = (enet_descriptors_struct *)(uint32_t)((uint32_t)dma_current_rxdesc1 + ETH_DMARXDESC_SIZE + GET_DMA_BCTL_DPSL(ENET_DMA_BCTL(
                                          enet_periph)));
                if(NULL != dma_current_ptp_rxdesc1) {
                    dma_current_ptp_rxdesc1++;
                }
            }
        }
    }
}

#if SELECT_DESCRIPTORS_ENHANCED_MODE
/*!
    \brief      get the bit of extended status flag in ENET DMA descriptor, API ID: 0x0012
    \param[in]  desc: the descriptor pointer which users want to get the extended status flag
    \param[in]  desc_status: the extended status want to get,
                only one parameter can be selected which is shown as below
      \arg        ENET_RDES4_IPPLDT: IP frame payload type
      \arg        ENET_RDES4_IPHERR: IP frame header error
      \arg        ENET_RDES4_IPPLDERR: IP frame payload error
      \arg        ENET_RDES4_IPCKSB: IP frame checksum bypassed
      \arg        ENET_RDES4_IPF4: IP frame in version 4
      \arg        ENET_RDES4_IPF6: IP frame in version 6
      \arg        ENET_RDES4_PTPMT: PTP message type
      \arg        ENET_RDES4_PTPOEF: PTP on ethernet frame
      \arg        ENET_RDES4_PTPVF: PTP version format
    \param[out] none
    \retval     value of extended status
*/
uint32_t enet_rx_desc_enhanced_status_get(enet_descriptors_struct *desc, uint32_t desc_status)
{
    uint32_t reval = 0xFFFFFFFFU;

    switch(desc_status) {
    case ENET_RDES4_IPPLDT:
        reval = GET_RDES4_IPPLDT(desc->extended_status);
        break;
    case ENET_RDES4_PTPMT:
        reval = GET_RDES4_PTPMT(desc->extended_status);
        break;
    default:
        if((uint32_t)RESET != (desc->extended_status & desc_status)) {
            reval = 1U;
        } else {
            reval = 0U;
        }
    }


    return reval;
}

/*!
    \brief      initialize the DMA Tx/Rx descriptors's parameters in enhanced chain mode with ptp function, API ID: 0x0013
    \param[in]  enet_periph: ENETx(x=0,1)
    \param[in]  direction: the descriptors which users want to init, refer to enet_dmadirection_enum,
                only one parameter can be selected which is shown as below
      \arg        ENET_DMA_TX: DMA Tx descriptors
      \arg        ENET_DMA_RX: DMA Rx descriptors
    \param[out] none
    \retval     none
*/
void enet_ptp_enhanced_descriptors_chain_init(uint32_t enet_periph, enet_dmadirection_enum direction)
{
    uint32_t num = 0U, count = 0U, maxsize = 0U;
    uint32_t desc_status = 0U, desc_bufsize = 0U;
    volatile enet_descriptors_struct *desc;
    volatile enet_descriptors_struct *desc_tab;
    volatile uint8_t *buf;

    if(ENET0 == enet_periph) {
        /* if want to initialize DMA Tx descriptors */
        if(ENET_DMA_TX == direction) {
            /* save a copy of the DMA Tx descriptors */
            desc_tab = txdesc_tab0;
            buf = &tx_buff0[0][0];
            count = ENET_TXBUF_NUM;
            maxsize = ENET_TXBUF_SIZE;

            /* select chain mode, and enable transmit timestamp function */
            desc_status = ENET_TDES0_TCHM | ENET_TDES0_TTSEN;

            /* configure DMA Tx descriptor table address register */
            ENET_DMA_TDTADDR(enet_periph) = (uint32_t)desc_tab;
            dma_current_txdesc0 = desc_tab;
        } else {
            /* if want to initialize DMA Rx descriptors */
            /* save a copy of the DMA Rx descriptors */
            desc_tab = rxdesc_tab0;
            buf = &rx_buff0[0][0];
            count = ENET_RXBUF_NUM;
            maxsize = ENET_RXBUF_SIZE;

            /* enable receiving */
            desc_status = ENET_RDES0_DAV;
            /* select receive chained mode and set buffer1 size */
            desc_bufsize = ENET_RDES1_RCHM | (uint32_t)ENET_RXBUF_SIZE;

            /* configure DMA Rx descriptor table address register */
            ENET_DMA_RDTADDR(enet_periph) = (uint32_t)desc_tab;
            dma_current_rxdesc0 = desc_tab;
        }

        /* configuration each descriptor */
        for(num = 0U; num < count; num++) {
            /* get the pointer to the next descriptor of the descriptor table */
            desc = desc_tab + num;

            /* configure descriptors */
            desc->status = desc_status;
            desc->control_buffer_size = desc_bufsize;
            desc->buffer1_addr = (uint32_t)(&buf[num * maxsize]);

            /* if is not the last descriptor */
            if(num < (count - 1U)) {
                /* configure the next descriptor address */
                desc->buffer2_next_desc_addr = (uint32_t)(desc_tab + num + 1U);
            } else {
                /* when it is the last descriptor, the next descriptor address
                equals to first descriptor address in descriptor table */
                desc->buffer2_next_desc_addr = (uint32_t)desc_tab;
            }
        }
    } else {
        /* if want to initialize DMA Tx descriptors */
        if(ENET_DMA_TX == direction) {
            /* save a copy of the DMA Tx descriptors */
            desc_tab = txdesc_tab1;
            buf = &tx_buff1[0][0];
            count = ENET_TXBUF_NUM;
            maxsize = ENET_TXBUF_SIZE;

            /* select chain mode, and enable transmit timestamp function */
            desc_status = ENET_TDES0_TCHM | ENET_TDES0_TTSEN;

            /* configure DMA Tx descriptor table address register */
            ENET_DMA_TDTADDR(enet_periph) = (uint32_t)desc_tab;
            dma_current_txdesc1 = desc_tab;
        } else {
            /* if want to initialize DMA Rx descriptors */
            /* save a copy of the DMA Rx descriptors */
            desc_tab = rxdesc_tab1;
            buf = &rx_buff1[0][0];
            count = ENET_RXBUF_NUM;
            maxsize = ENET_RXBUF_SIZE;

            /* enable receiving */
            desc_status = ENET_RDES0_DAV;
            /* select receive chained mode and set buffer1 size */
            desc_bufsize = ENET_RDES1_RCHM | (uint32_t)ENET_RXBUF_SIZE;

            /* configure DMA Rx descriptor table address register */
            ENET_DMA_RDTADDR(enet_periph) = (uint32_t)desc_tab;
            dma_current_rxdesc1 = desc_tab;
        }

        /* configuration each descriptor */
        for(num = 0U; num < count; num++) {
            /* get the pointer to the next descriptor of the descriptor table */
            desc = desc_tab + num;

            /* configure descriptors */
            desc->status = desc_status;
            desc->control_buffer_size = desc_bufsize;
            desc->buffer1_addr = (uint32_t)(&buf[num * maxsize]);

            /* if is not the last descriptor */
            if(num < (count - 1U)) {
                /* configure the next descriptor address */
                desc->buffer2_next_desc_addr = (uint32_t)(desc_tab + num + 1U);
            } else {
                /* when it is the last descriptor, the next descriptor address
                equals to first descriptor address in descriptor table */
                desc->buffer2_next_desc_addr = (uint32_t)desc_tab;
            }
        }
    }
}

/*!
    \brief      initialize the DMA Tx/Rx descriptors's parameters in enhanced ring mode with ptp function, API ID: 0x0014
    \param[in]  enet_periph: ENETx(x=0,1)
    \param[in]  direction: the descriptors which users want to init, refer to enet_dmadirection_enum,
                only one parameter can be selected which is shown as below
      \arg        ENET_DMA_TX: DMA Tx descriptors
      \arg        ENET_DMA_RX: DMA Rx descriptors
    \param[out] none
    \retval     none
*/
void enet_ptp_enhanced_descriptors_ring_init(uint32_t enet_periph, enet_dmadirection_enum direction)
{
    uint32_t num = 0U, count = 0U, maxsize = 0U;
    uint32_t desc_status = 0U, desc_bufsize = 0U;
    volatile enet_descriptors_struct *desc;
    volatile enet_descriptors_struct *desc_tab;
    volatile uint8_t *buf;

    if(ENET0 == enet_periph) {
        /* configure descriptor skip length */
        ENET_DMA_BCTL(enet_periph) &= ~ENET_DMA_BCTL_DPSL;
        ENET_DMA_BCTL(enet_periph) |= DMA_BCTL_DPSL(0);

        /* if want to initialize DMA Tx descriptors */
        if(ENET_DMA_TX == direction) {
            /* save a copy of the DMA Tx descriptors */
            desc_tab = txdesc_tab0;
            buf = &tx_buff0[0][0];
            count = ENET_TXBUF_NUM;
            maxsize = ENET_TXBUF_SIZE;

            /* select ring mode, and enable transmit timestamp function */
            desc_status = ENET_TDES0_TTSEN;

            /* configure DMA Tx descriptor table address register */
            ENET_DMA_TDTADDR(enet_periph) = (uint32_t)desc_tab;
            dma_current_txdesc0 = desc_tab;
        } else {
            /* if want to initialize DMA Rx descriptors */
            /* save a copy of the DMA Rx descriptors */
            desc_tab = rxdesc_tab0;
            buf = &rx_buff0[0][0];
            count = ENET_RXBUF_NUM;
            maxsize = ENET_RXBUF_SIZE;

            /* enable receiving */
            desc_status = ENET_RDES0_DAV;
            /* set buffer1 size */
            desc_bufsize = ENET_RXBUF_SIZE;

            /* configure DMA Rx descriptor table address register */
            ENET_DMA_RDTADDR(enet_periph) = (uint32_t)desc_tab;
            dma_current_rxdesc0 = desc_tab;
        }

        /* configure each descriptor */
        for(num = 0U; num < count; num++) {
            /* get the pointer to the next descriptor of the descriptor table */
            desc = desc_tab + num;

            /* configure descriptors */
            desc->status = desc_status;
            desc->control_buffer_size = desc_bufsize;
            desc->buffer1_addr = (uint32_t)(&buf[num * maxsize]);

            /* when it is the last descriptor */
            if(num == (count - 1U)) {
                if(ENET_DMA_TX == direction) {
                    /* configure transmit end of ring mode */
                    desc->status |= ENET_TDES0_TERM;
                } else {
                    /* configure receive end of ring mode */
                    desc->control_buffer_size |= ENET_RDES1_RERM;
                }
            }
        }
    } else {
        /* configure descriptor skip length */
        ENET_DMA_BCTL(enet_periph) &= ~ENET_DMA_BCTL_DPSL;
        ENET_DMA_BCTL(enet_periph) |= DMA_BCTL_DPSL(0);

        /* if want to initialize DMA Tx descriptors */
        if(ENET_DMA_TX == direction) {
            /* save a copy of the DMA Tx descriptors */
            desc_tab = txdesc_tab1;
            buf = &tx_buff1[0][0];
            count = ENET_TXBUF_NUM;
            maxsize = ENET_TXBUF_SIZE;

            /* select ring mode, and enable transmit timestamp function */
            desc_status = ENET_TDES0_TTSEN;

            /* configure DMA Tx descriptor table address register */
            ENET_DMA_TDTADDR(enet_periph) = (uint32_t)desc_tab;
            dma_current_txdesc1 = desc_tab;
        } else {
            /* if want to initialize DMA Rx descriptors */
            /* save a copy of the DMA Rx descriptors */
            desc_tab = rxdesc_tab1;
            buf = &rx_buff1[0][0];
            count = ENET_RXBUF_NUM;
            maxsize = ENET_RXBUF_SIZE;

            /* enable receiving */
            desc_status = ENET_RDES0_DAV;
            /* set buffer1 size */
            desc_bufsize = ENET_RXBUF_SIZE;

            /* configure DMA Rx descriptor table address register */
            ENET_DMA_RDTADDR(enet_periph) = (uint32_t)desc_tab;
            dma_current_rxdesc1 = desc_tab;
        }

        /* configure each descriptor */
        for(num = 0U; num < count; num++) {
            /* get the pointer to the next descriptor of the descriptor table */
            desc = desc_tab + num;

            /* configure descriptors */
            desc->status = desc_status;
            desc->control_buffer_size = desc_bufsize;
            desc->buffer1_addr = (uint32_t)(&buf[num * maxsize]);

            /* when it is the last descriptor */
            if(num == (count - 1U)) {
                if(ENET_DMA_TX == direction) {
                    /* configure transmit end of ring mode */
                    desc->status |= ENET_TDES0_TERM;
                } else {
                    /* configure receive end of ring mode */
                    desc->control_buffer_size |= ENET_RDES1_RERM;
                }
            }
        }
    }
}

/*!
    \brief      receive a packet data with timestamp values to application buffer, when the DMA is in enhanced mode, API ID: 0x0015
    \param[in]  enet_periph: ENETx(x=0,1)
    \param[in]  bufsize: the size of buffer which is the parameter in function
    \param[out] buffer: pointer to the application buffer
                note -- if the input is NULL, user should copy data in application by himself
    \param[out] timestamp: pointer to the table which stores the timestamp high and low
                note -- if the input is NULL, timestamp is ignored
    \retval     ErrStatus: SUCCESS or ERROR
*/
ErrStatus enet_ptpframe_receive_enhanced_mode(uint32_t enet_periph, uint8_t buffer[], uint32_t bufsize, uint32_t timestamp[])
{
    uint32_t offset = 0U, size = 0U;
    uint32_t timeout = 0U;
    uint32_t rdes0_tsv_flag;
    ErrStatus status = ERROR;

    if(ENET0 == enet_periph) {
        /* the descriptor is busy due to own by the DMA */
        if((uint32_t)RESET == (dma_current_rxdesc0->status & ENET_RDES0_DAV)) {
            /* if no error occurs, and the frame uses only one descriptor */
            if(((uint32_t)RESET == (dma_current_rxdesc0->status & ENET_RDES0_ERRS)) &&
                    ((uint32_t)RESET != (dma_current_rxdesc0->status & ENET_RDES0_LDES)) &&
                    ((uint32_t)RESET != (dma_current_rxdesc0->status & ENET_RDES0_FDES))) {

                /* get the frame length except CRC */
                size = GET_RDES0_FRML(dma_current_rxdesc0->status) - 4U;

                /* if is a type frame, and CRC is not included in forwarding frame */
                if((RESET != (ENET_MAC_CFG(enet_periph) & ENET_MAC_CFG_TFCD)) && (RESET != (dma_current_rxdesc0->status & ENET_RDES0_FRMT))) {
                    size = size + 4U;
                }

                /* copy data from Rx buffer to application buffer if provided and size is valid */
                if((NULL != buffer) && (size <= bufsize)) {
                    for(offset = 0U; offset < size; offset++) {
                        (*(buffer + offset)) = (*(__IO uint8_t *)((dma_current_rxdesc0->buffer1_addr) + offset));
                    }

                    /* get timestamp if the pointer is not NULL */
                    if(NULL != timestamp) {
                        /* wait for ENET_RDES0_TSV flag to be set, the timestamp value is taken and
                        write to the RDES6 and RDES7 */
                        do {
                            rdes0_tsv_flag = (dma_current_rxdesc0->status & ENET_RDES0_TSV);
                            timeout++;
                        } while((RESET == rdes0_tsv_flag) && (timeout < ENET_DELAY_TO));

                        /* proceed only if timeout not reached */
                        if(ENET_DELAY_TO != timeout) {
                            /* clear the ENET_RDES0_TSV flag */
                            dma_current_rxdesc0->status &= ~ENET_RDES0_TSV;
                            /* get the timestamp value of the received frame */
                            timestamp[0] = dma_current_rxdesc0->timestamp_low;
                            timestamp[1] = dma_current_rxdesc0->timestamp_high;

                            status = SUCCESS;
                        }
                    } else {
                        /* if timestamp is not needed, operation is successful */
                        status = SUCCESS;
                    }
                } else if(NULL == buffer) {
                    /* if buffer is NULL, user will handle data in application */
                    /* get timestamp if the pointer is not NULL */
                    if(NULL != timestamp) {
                        /* wait for ENET_RDES0_TSV flag to be set */
                        do {
                            rdes0_tsv_flag = (dma_current_rxdesc0->status & ENET_RDES0_TSV);
                            timeout++;
                        } while((RESET == rdes0_tsv_flag) && (timeout < ENET_DELAY_TO));

                        /* proceed only if timeout not reached */
                        if(ENET_DELAY_TO != timeout) {
                            /* clear the ENET_RDES0_TSV flag */
                            dma_current_rxdesc0->status &= ~ENET_RDES0_TSV;
                            /* get the timestamp value of the received frame */
                            timestamp[0] = dma_current_rxdesc0->timestamp_low;
                            timestamp[1] = dma_current_rxdesc0->timestamp_high;
                        }
                    }
                    status = SUCCESS;
                }
            }

            /* regardless of success or failure, we need to restore DMA reception */
            /* enable reception, descriptor is owned by DMA */
            dma_current_rxdesc0->status = ENET_RDES0_DAV;

            /* check Rx buffer unavailable flag status */
            if((uint32_t)RESET != (ENET_DMA_STAT(enet_periph) & ENET_DMA_STAT_RBU)) {
                /* Clear RBU flag */
                ENET_DMA_STAT(enet_periph) = ENET_DMA_STAT_RBU;
                /* resume DMA reception by writing to the RPEN register*/
                ENET_DMA_RPEN(enet_periph) = 0U;
            }

            /* update the current RxDMA descriptor pointer to the next descriptor in RxDMA descriptor table */
            /* chained mode */
            if((uint32_t)RESET != (dma_current_rxdesc0->control_buffer_size & ENET_RDES1_RCHM)) {
                dma_current_rxdesc0 = (enet_descriptors_struct *)(dma_current_rxdesc0->buffer2_next_desc_addr);
            } else {
                /* ring mode */
                if((uint32_t)RESET != (dma_current_rxdesc0->control_buffer_size & ENET_RDES1_RERM)) {
                    /* if is the last descriptor in table, the next descriptor is the table header */
                    dma_current_rxdesc0 = (enet_descriptors_struct *)(ENET_DMA_RDTADDR(enet_periph));
                } else {
                    /* the next descriptor is the current address, add the descriptor size, and descriptor skip length */
                    dma_current_rxdesc0 = (enet_descriptors_struct *)((uint32_t)dma_current_rxdesc0 + ETH_DMARXDESC_SIZE + GET_DMA_BCTL_DPSL(ENET_DMA_BCTL(enet_periph)));
                }
            }
        }
    } else {
        /* the descriptor is busy due to own by the DMA */
        if((uint32_t)RESET == (dma_current_rxdesc1->status & ENET_RDES0_DAV)) {
            /* if no error occurs, and the frame uses only one descriptor */
            if(((uint32_t)RESET == (dma_current_rxdesc1->status & ENET_RDES0_ERRS)) &&
                    ((uint32_t)RESET != (dma_current_rxdesc1->status & ENET_RDES0_LDES)) &&
                    ((uint32_t)RESET != (dma_current_rxdesc1->status & ENET_RDES0_FDES))) {

                /* get the frame length except CRC */
                size = GET_RDES0_FRML(dma_current_rxdesc1->status) - 4U;

                /* if is a type frame, and CRC is not included in forwarding frame */
                if((RESET != (ENET_MAC_CFG(enet_periph) & ENET_MAC_CFG_TFCD)) && (RESET != (dma_current_rxdesc1->status & ENET_RDES0_FRMT))) {
                    size = size + 4U;
                }

                /* copy data from Rx buffer to application buffer if provided and size is valid */
                if((NULL != buffer) && (size <= bufsize)) {
                    for(offset = 0U; offset < size; offset++) {
                        (*(buffer + offset)) = (*(__IO uint8_t *)((dma_current_rxdesc1->buffer1_addr) + offset));
                    }

                    /* get timestamp if the pointer is not NULL */
                    if(NULL != timestamp) {
                        /* wait for ENET_RDES0_TSV flag to be set, the timestamp value is taken and
                        write to the RDES6 and RDES7 */
                        do {
                            rdes0_tsv_flag = (dma_current_rxdesc1->status & ENET_RDES0_TSV);
                            timeout++;
                        } while((RESET == rdes0_tsv_flag) && (timeout < ENET_DELAY_TO));

                        /* proceed only if timeout not reached */
                        if(ENET_DELAY_TO != timeout) {
                            /* clear the ENET_RDES0_TSV flag */
                            dma_current_rxdesc1->status &= ~ENET_RDES0_TSV;
                            /* get the timestamp value of the received frame */
                            timestamp[0] = dma_current_rxdesc1->timestamp_low;
                            timestamp[1] = dma_current_rxdesc1->timestamp_high;

                            status = SUCCESS;
                        }
                    } else {
                        /* if timestamp is not needed, operation is successful */
                        status = SUCCESS;
                    }
                } else if(NULL == buffer) {
                    /* if buffer is NULL, user will handle data in application */
                    /* get timestamp if the pointer is not NULL */
                    if(NULL != timestamp) {
                        /* wait for ENET_RDES0_TSV flag to be set */
                        do {
                            rdes0_tsv_flag = (dma_current_rxdesc1->status & ENET_RDES0_TSV);
                            timeout++;
                        } while((RESET == rdes0_tsv_flag) && (timeout < ENET_DELAY_TO));

                        /* proceed only if timeout not reached */
                        if(ENET_DELAY_TO != timeout) {
                            /* clear the ENET_RDES0_TSV flag */
                            dma_current_rxdesc1->status &= ~ENET_RDES0_TSV;
                            /* get the timestamp value of the received frame */
                            timestamp[0] = dma_current_rxdesc1->timestamp_low;
                            timestamp[1] = dma_current_rxdesc1->timestamp_high;
                        }
                    }
                    status = SUCCESS;
                }
            }

            /* regardless of success or failure, we need to restore DMA reception */
            /* enable reception, descriptor is owned by DMA */
            dma_current_rxdesc1->status = ENET_RDES0_DAV;

            /* check Rx buffer unavailable flag status */
            if((uint32_t)RESET != (ENET_DMA_STAT(enet_periph) & ENET_DMA_STAT_RBU)) {
                /* Clear RBU flag */
                ENET_DMA_STAT(enet_periph) = ENET_DMA_STAT_RBU;
                /* resume DMA reception by writing to the RPEN register*/
                ENET_DMA_RPEN(enet_periph) = 0U;
            }

            /* update the current RxDMA descriptor pointer to the next descriptor in RxDMA descriptor table */
            /* chained mode */
            if((uint32_t)RESET != (dma_current_rxdesc1->control_buffer_size & ENET_RDES1_RCHM)) {
                dma_current_rxdesc1 = (enet_descriptors_struct *)(dma_current_rxdesc1->buffer2_next_desc_addr);
            } else {
                /* ring mode */
                if((uint32_t)RESET != (dma_current_rxdesc1->control_buffer_size & ENET_RDES1_RERM)) {
                    /* if is the last descriptor in table, the next descriptor is the table header */
                    dma_current_rxdesc1 = (enet_descriptors_struct *)(ENET_DMA_RDTADDR(enet_periph));
                } else {
                    /* the next descriptor is the current address, add the descriptor size, and descriptor skip length */
                    dma_current_rxdesc1 = (enet_descriptors_struct *)((uint32_t)dma_current_rxdesc1 + ETH_DMARXDESC_SIZE + GET_DMA_BCTL_DPSL(ENET_DMA_BCTL(enet_periph)));
                }
            }
        }
    }

    return status;
}

/*!
    \brief      send data with timestamp values in application buffer as a transmit packet, when the DMA is in enhanced mode, API ID: 0x0016
    \param[in]  enet_periph: ENETx(x=0,1)
    \param[in]  buffer: pointer on the application buffer
                note -- if the input is NULL, user should copy data in application by himself
    \param[in]  length: the length of frame data to be transmitted
    \param[out] timestamp: pointer to the table which stores the timestamp high and low
                note -- if the input is NULL, timestamp is ignored
    \param[out] none
    \retval     ErrStatus: SUCCESS or ERROR
*/
ErrStatus enet_ptpframe_transmit_enhanced_mode(uint32_t enet_periph, uint8_t buffer[], uint32_t length, uint32_t timestamp[])
{
    uint32_t offset = 0U;
    uint32_t dma_tbu_flag, dma_tu_flag;
    uint32_t tdes0_ttmss_flag;
    uint32_t timeout = 0U;
    ErrStatus status = SUCCESS;

    if(ENET0 == enet_periph) {
        /* Check if the descriptor is busy or frame length is too long */
        if(((uint32_t)RESET != (dma_current_txdesc0->status & ENET_TDES0_DAV)) ||
                (length > ENET_MAX_FRAME_SIZE)) {
            status = ERROR;
        } else {
            /* if buffer pointer is not null, copy frame data from application buffer to Tx buffer */
            if(NULL != buffer) {
                /* copy frame data from application buffer to Tx buffer */
                for(offset = 0U; offset < length; offset++) {
                    (*(__IO uint8_t *)((dma_current_txdesc0->buffer1_addr) + offset)) = (*(buffer + offset));
                }
            }

            /* set the frame length */
            dma_current_txdesc0->control_buffer_size = length;
            /* set the segment of frame, frame is transmitted in one descriptor */
            dma_current_txdesc0->status |= ENET_TDES0_LSG | ENET_TDES0_FSG;
            /* enable the DMA transmission */
            dma_current_txdesc0->status |= ENET_TDES0_DAV;

            /* check Tx buffer unavailable flag status */
            dma_tbu_flag = (ENET_DMA_STAT(enet_periph) & ENET_DMA_STAT_TBU);
            dma_tu_flag = (ENET_DMA_STAT(enet_periph) & ENET_DMA_STAT_TU);

            if((RESET != dma_tbu_flag) || (RESET != dma_tu_flag)) {
                /* Clear TBU and TU flag */
                ENET_DMA_STAT(enet_periph) = (dma_tbu_flag | dma_tu_flag);
                /* resume DMA transmission by writing to the TPEN register*/
                ENET_DMA_TPEN(enet_periph) = 0U;
            }

            /* if timestamp pointer is not null, wait for and get timestamp */
            if((NULL != timestamp) && (status == SUCCESS)) {
                /* wait for ENET_TDES0_TTMSS flag to be set, a timestamp was captured */
                do {
                    tdes0_ttmss_flag = (dma_current_txdesc0->status & ENET_TDES0_TTMSS);
                    timeout++;
                } while((RESET == tdes0_ttmss_flag) && (timeout < ENET_DELAY_TO));

                /* Check for timeout error */
                if(ENET_DELAY_TO == timeout) {
                    status = ERROR;
                } else {
                    /* clear the ENET_TDES0_TTMSS flag */
                    dma_current_txdesc0->status &= ~ENET_TDES0_TTMSS;
                    /* get the timestamp value of the transmit frame */
                    timestamp[0] = dma_current_txdesc0->timestamp_low;
                    timestamp[1] = dma_current_txdesc0->timestamp_high;
                }
            }

            /* Only update descriptor pointers if no error occurred */
            if(SUCCESS == status) {
                /* update the current TxDMA descriptor pointer to the next descriptor in TxDMA descriptor table*/
                /* chained mode */
                if((uint32_t)RESET != (dma_current_txdesc0->status & ENET_TDES0_TCHM)) {
                    dma_current_txdesc0 = (enet_descriptors_struct *)(dma_current_txdesc0->buffer2_next_desc_addr);
                } else {
                    /* ring mode */
                    if((uint32_t)RESET != (dma_current_txdesc0->status & ENET_TDES0_TERM)) {
                        /* if is the last descriptor in table, the next descriptor is the table header */
                        dma_current_txdesc0 = (enet_descriptors_struct *)(ENET_DMA_TDTADDR(enet_periph));
                    } else {
                        /* the next descriptor is the current address, add the descriptor size, and descriptor skip length */
                        dma_current_txdesc0 = (enet_descriptors_struct *)((uint32_t)dma_current_txdesc0 + ETH_DMATXDESC_SIZE + GET_DMA_BCTL_DPSL(ENET_DMA_BCTL(enet_periph)));
                    }
                }
            }
        }
    } else {
        /* Check if the descriptor is busy or frame length is too long */
        if(((uint32_t)RESET != (dma_current_txdesc1->status & ENET_TDES0_DAV)) ||
                (length > ENET_MAX_FRAME_SIZE)) {
            status = ERROR;
        } else {
            /* if buffer pointer is not null, copy frame data from application buffer to Tx buffer */
            if(NULL != buffer) {
                /* copy frame data from application buffer to Tx buffer */
                for(offset = 0U; offset < length; offset++) {
                    (*(__IO uint8_t *)((dma_current_txdesc1->buffer1_addr) + offset)) = (*(buffer + offset));
                }
            }

            /* set the frame length */
            dma_current_txdesc1->control_buffer_size = length;
            /* set the segment of frame, frame is transmitted in one descriptor */
            dma_current_txdesc1->status |= ENET_TDES0_LSG | ENET_TDES0_FSG;
            /* enable the DMA transmission */
            dma_current_txdesc1->status |= ENET_TDES0_DAV;

            /* check Tx buffer unavailable flag status */
            dma_tbu_flag = (ENET_DMA_STAT(enet_periph) & ENET_DMA_STAT_TBU);
            dma_tu_flag = (ENET_DMA_STAT(enet_periph) & ENET_DMA_STAT_TU);

            if((RESET != dma_tbu_flag) || (RESET != dma_tu_flag)) {
                /* Clear TBU and TU flag */
                ENET_DMA_STAT(enet_periph) = (dma_tbu_flag | dma_tu_flag);
                /* resume DMA transmission by writing to the TPEN register*/
                ENET_DMA_TPEN(enet_periph) = 0U;
            }

            /* if timestamp pointer is not null, wait for and get timestamp */
            if((NULL != timestamp) && (status == SUCCESS)) {
                /* wait for ENET_TDES0_TTMSS flag to be set, a timestamp was captured */
                do {
                    tdes0_ttmss_flag = (dma_current_txdesc1->status & ENET_TDES0_TTMSS);
                    timeout++;
                } while((RESET == tdes0_ttmss_flag) && (timeout < ENET_DELAY_TO));

                /* Check for timeout error */
                if(ENET_DELAY_TO == timeout) {
                    status = ERROR;
                } else {
                    /* clear the ENET_TDES0_TTMSS flag */
                    dma_current_txdesc1->status &= ~ENET_TDES0_TTMSS;
                    /* get the timestamp value of the transmit frame */
                    timestamp[0] = dma_current_txdesc1->timestamp_low;
                    timestamp[1] = dma_current_txdesc1->timestamp_high;
                }
            }

            /* Only update descriptor pointers if no error occurred */
            if(SUCCESS == status) {
                /* update the current TxDMA descriptor pointer to the next descriptor in TxDMA descriptor table*/
                /* chained mode */
                if((uint32_t)RESET != (dma_current_txdesc1->status & ENET_TDES0_TCHM)) {
                    dma_current_txdesc1 = (enet_descriptors_struct *)(dma_current_txdesc1->buffer2_next_desc_addr);
                } else {
                    /* ring mode */
                    if((uint32_t)RESET != (dma_current_txdesc1->status & ENET_TDES0_TERM)) {
                        /* if is the last descriptor in table, the next descriptor is the table header */
                        dma_current_txdesc1 = (enet_descriptors_struct *)(ENET_DMA_TDTADDR(enet_periph));
                    } else {
                        /* the next descriptor is the current address, add the descriptor size, and descriptor skip length */
                        dma_current_txdesc1 = (enet_descriptors_struct *)((uint32_t)dma_current_txdesc1 + ETH_DMATXDESC_SIZE + GET_DMA_BCTL_DPSL(ENET_DMA_BCTL(enet_periph)));
                    }
                }
            }
        }
    }

    return status;
}

#else

/*!
    \brief      initialize the DMA Tx/Rx descriptors's parameters in normal chain mode with PTP function, API ID: 0x0017
    \param[in]  enet_periph: ENETx(x=0,1)
    \param[in]  direction: the descriptors which users want to init, refer to enet_dmadirection_enum,
                only one parameter can be selected which is shown as below
      \arg        ENET_DMA_TX: DMA Tx descriptors
      \arg        ENET_DMA_RX: DMA Rx descriptors
    \param[in]  desc_ptptab: pointer to the first descriptor address of PTP descriptor table
    \param[out] none
    \retval     none
*/
void enet_ptp_normal_descriptors_chain_init(uint32_t enet_periph, enet_dmadirection_enum direction, enet_descriptors_struct desc_ptptab[])
{
    uint32_t num = 0U, count = 0U;
    uint32_t desc_status = 0U, desc_bufsize = 0U;
    volatile enet_descriptors_struct *desc;
    volatile enet_descriptors_struct *desc_tab;

    if(ENET0 == enet_periph) {
        /* if want to initialize DMA Tx descriptors */
        if(ENET_DMA_TX == direction) {
            /* save a copy of the DMA Tx descriptors */
            desc_tab = txdesc_tab0;
            count = ENET_TXBUF_NUM;

            /* select chain mode, and enable transmit timestamp function */
            desc_status = ENET_TDES0_TCHM | ENET_TDES0_TTSEN;

            /* configure DMA Tx descriptor table address register */
            ENET_DMA_TDTADDR(enet_periph) = (uint32_t)desc_tab;
            dma_current_txdesc0 = desc_tab;
            dma_current_ptp_txdesc0 = desc_ptptab;
        } else {
            /* if want to initialize DMA Rx descriptors */
            /* save a copy of the DMA Rx descriptors */
            desc_tab = rxdesc_tab0;
            count = ENET_RXBUF_NUM;

            /* enable receiving */
            desc_status = ENET_RDES0_DAV;
            /* select receive chained mode and set buffer1 size */
            desc_bufsize = ENET_RDES1_RCHM | (uint32_t)ENET_RXBUF_SIZE;

            /* configure DMA Rx descriptor table address register */
            ENET_DMA_RDTADDR(enet_periph) = (uint32_t)desc_tab;
            dma_current_rxdesc0 = desc_tab;
            dma_current_ptp_rxdesc0 = desc_ptptab;
        }

        /* configure each descriptor */
        for(num = 0U; num < count; num++) {
            /* get the pointer to the next descriptor of the descriptor table */
            if(ENET_DMA_TX == direction) {
                desc = &txdesc_tab0[num];
                desc->buffer1_addr = (uint32_t)(&tx_buff0[num][0]);
            } else {
                desc = &rxdesc_tab0[num];
                desc->buffer1_addr = (uint32_t)(&rx_buff0[num][0]);
            }

            /* configure descriptors */
            desc->status = desc_status;
            desc->control_buffer_size = desc_bufsize;

            /* if is not the last descriptor */
            if(num < (count - 1U)) {
                /* configure the next descriptor address */
                if(ENET_DMA_TX == direction) {
                    desc->buffer2_next_desc_addr = (uint32_t)(&txdesc_tab0[num + 1U]);
                } else {
                    desc->buffer2_next_desc_addr = (uint32_t)(&rxdesc_tab0[num + 1U]);
                }
            } else {
                /* when it is the last descriptor, the next descriptor address
                equals to first descriptor address in descriptor table */
                desc->buffer2_next_desc_addr = (uint32_t)desc_tab;
            }
            /* set desc_ptptab equal to desc_tab */
            (&desc_ptptab[num])->buffer1_addr = desc->buffer1_addr;
            (&desc_ptptab[num])->buffer2_next_desc_addr = desc->buffer2_next_desc_addr;
        }
        /* when it is the last ptp descriptor, preserve the first descriptor
        address of desc_ptptab in ptp descriptor status */
        (&desc_ptptab[num - 1U])->status = (uint32_t)desc_ptptab;
    } else {
        /* if want to initialize DMA Tx descriptors */
        if(ENET_DMA_TX == direction) {
            /* save a copy of the DMA Tx descriptors */
            desc_tab = txdesc_tab1;
            count = ENET_TXBUF_NUM;

            /* select chain mode, and enable transmit timestamp function */
            desc_status = ENET_TDES0_TCHM | ENET_TDES0_TTSEN;

            /* configure DMA Tx descriptor table address register */
            ENET_DMA_TDTADDR(enet_periph) = (uint32_t)desc_tab;
            dma_current_txdesc1 = desc_tab;
            dma_current_ptp_txdesc1 = desc_ptptab;
        } else {
            /* if want to initialize DMA Rx descriptors */
            /* save a copy of the DMA Rx descriptors */
            desc_tab = rxdesc_tab1;
            count = ENET_RXBUF_NUM;

            /* enable receiving */
            desc_status = ENET_RDES0_DAV;
            /* select receive chained mode and set buffer1 size */
            desc_bufsize = ENET_RDES1_RCHM | (uint32_t)ENET_RXBUF_SIZE;

            /* configure DMA Rx descriptor table address register */
            ENET_DMA_RDTADDR(enet_periph) = (uint32_t)desc_tab;
            dma_current_rxdesc1 = desc_tab;
            dma_current_ptp_rxdesc1 = desc_ptptab;
        }

        /* configure each descriptor */
        for(num = 0U; num < count; num++) {
            /* get the pointer to the next descriptor of the descriptor table */
            if(ENET_DMA_TX == direction) {
                desc = &txdesc_tab1[num];
                desc->buffer1_addr = (uint32_t)(&tx_buff1[num][0]);
            } else {
                desc = &rxdesc_tab1[num];
                desc->buffer1_addr = (uint32_t)(&rx_buff1[num][0]);
            }

            /* configure descriptors */
            desc->status = desc_status;
            desc->control_buffer_size = desc_bufsize;

            /* if is not the last descriptor */
            if(num < (count - 1U)) {
                /* configure the next descriptor address */
                if(ENET_DMA_TX == direction) {
                    desc->buffer2_next_desc_addr = (uint32_t)(&txdesc_tab1[num + 1U]);
                } else {
                    desc->buffer2_next_desc_addr = (uint32_t)(&rxdesc_tab1[num + 1U]);
                }
            } else {
                /* when it is the last descriptor, the next descriptor address
                equals to first descriptor address in descriptor table */
                desc->buffer2_next_desc_addr = (uint32_t)desc_tab;
            }
            /* set desc_ptptab equal to desc_tab */
            (&desc_ptptab[num])->buffer1_addr = desc->buffer1_addr;
            (&desc_ptptab[num])->buffer2_next_desc_addr = desc->buffer2_next_desc_addr;
        }
        /* when it is the last ptp descriptor, preserve the first descriptor
        address of desc_ptptab in ptp descriptor status */
        (&desc_ptptab[num - 1U])->status = (uint32_t)desc_ptptab;
    }
}

/*!
    \brief      initialize the DMA Tx/Rx descriptors's parameters in normal ring mode with PTP function, API ID: 0x0018
    \param[in]  enet_periph: ENETx(x=0,1)
    \param[in]  direction: the descriptors which users want to init, refer to enet_dmadirection_enum,
                only one parameter can be selected which is shown as below
      \arg        ENET_DMA_TX: DMA Tx descriptors
      \arg        ENET_DMA_RX: DMA Rx descriptors
    \param[in]  desc_ptptab: pointer to the first descriptor address of PTP descriptor table
    \param[out] none
    \retval     none
*/
void enet_ptp_normal_descriptors_ring_init(uint32_t enet_periph, enet_dmadirection_enum direction, enet_descriptors_struct desc_ptptab[])
{
    uint32_t num = 0U, count = 0U;
    uint32_t desc_status = 0U, desc_bufsize = 0U;
    volatile enet_descriptors_struct *desc;
    volatile enet_descriptors_struct *desc_tab;

    if(ENET0 == enet_periph) {
        /* configure descriptor skip length */
        ENET_DMA_BCTL(enet_periph) &= ~ENET_DMA_BCTL_DPSL;
        ENET_DMA_BCTL(enet_periph) |= DMA_BCTL_DPSL(0);

        /* if want to initialize DMA Tx descriptors */
        if(ENET_DMA_TX == direction) {
            /* save a copy of the DMA Tx descriptors */
            desc_tab = txdesc_tab0;
            count = ENET_TXBUF_NUM;

            /* select ring mode, and enable transmit timestamp function */
            desc_status = ENET_TDES0_TTSEN;

            /* configure DMA Tx descriptor table address register */
            ENET_DMA_TDTADDR(enet_periph) = (uint32_t)desc_tab;
            dma_current_txdesc0 = desc_tab;
            dma_current_ptp_txdesc0 = desc_ptptab;
        } else {
            /* if want to initialize DMA Rx descriptors */
            /* save a copy of the DMA Rx descriptors */
            desc_tab = rxdesc_tab0;
            count = ENET_RXBUF_NUM;

            /* enable receiving */
            desc_status = ENET_RDES0_DAV;
            /* select receive ring mode and set buffer1 size */
            desc_bufsize = (uint32_t)ENET_RXBUF_SIZE;

            /* configure DMA Rx descriptor table address register */
            ENET_DMA_RDTADDR(enet_periph) = (uint32_t)desc_tab;
            dma_current_rxdesc0 = desc_tab;
            dma_current_ptp_rxdesc0 = desc_ptptab;
        }

        /* configure each descriptor */
        for(num = 0U; num < count; num++) {
            /* get the pointer to the next descriptor of the descriptor table */
            if(ENET_DMA_TX == direction) {
                desc = &txdesc_tab0[num];
                desc->buffer1_addr = (uint32_t)(&tx_buff0[num][0]);
            } else {
                desc = &rxdesc_tab0[num];
                desc->buffer1_addr = (uint32_t)(&rx_buff0[num][0]);
            }

            /* configure descriptors */
            desc->status = desc_status;
            desc->control_buffer_size = desc_bufsize;

            /* when it is the last descriptor */
            if(num == (count - 1U)) {
                if(ENET_DMA_TX == direction) {
                    /* configure transmit end of ring mode */
                    desc->status |= ENET_TDES0_TERM;
                } else {
                    /* configure receive end of ring mode */
                    desc->control_buffer_size |= ENET_RDES1_RERM;
                }
            }
            /* set desc_ptptab equal to desc_tab */
            (&desc_ptptab[num])->buffer1_addr = desc->buffer1_addr;
            (&desc_ptptab[num])->buffer2_next_desc_addr = desc->buffer2_next_desc_addr;
        }
        /* when it is the last ptp descriptor, preserve the first descriptor
        address of desc_ptptab in ptp descriptor status */
        (&desc_ptptab[num - 1U])->status = (uint32_t)desc_ptptab;
    } else {
        /* configure descriptor skip length */
        ENET_DMA_BCTL(enet_periph) &= ~ENET_DMA_BCTL_DPSL;
        ENET_DMA_BCTL(enet_periph) |= DMA_BCTL_DPSL(0);

        /* if want to initialize DMA Tx descriptors */
        if(ENET_DMA_TX == direction) {
            /* save a copy of the DMA Tx descriptors */
            desc_tab = txdesc_tab1;
            count = ENET_TXBUF_NUM;

            /* select ring mode, and enable transmit timestamp function */
            desc_status = ENET_TDES0_TTSEN;

            /* configure DMA Tx descriptor table address register */
            ENET_DMA_TDTADDR(enet_periph) = (uint32_t)desc_tab;
            dma_current_txdesc1 = desc_tab;
            dma_current_ptp_txdesc1 = desc_ptptab;
        } else {
            /* if want to initialize DMA Rx descriptors */
            /* save a copy of the DMA Rx descriptors */
            desc_tab = rxdesc_tab1;
            count = ENET_RXBUF_NUM;

            /* enable receiving */
            desc_status = ENET_RDES0_DAV;
            /* select receive ring mode and set buffer1 size */
            desc_bufsize = (uint32_t)ENET_RXBUF_SIZE;

            /* configure DMA Rx descriptor table address register */
            ENET_DMA_RDTADDR(enet_periph) = (uint32_t)desc_tab;
            dma_current_rxdesc1 = desc_tab;
            dma_current_ptp_rxdesc1 = desc_ptptab;
        }

        /* configure each descriptor */
        for(num = 0U; num < count; num++) {
            /* get the pointer to the next descriptor of the descriptor table */
            if(ENET_DMA_TX == direction) {
                desc = &txdesc_tab1[num];
                desc->buffer1_addr = (uint32_t)(&tx_buff1[num][0]);
            } else {
                desc = &rxdesc_tab1[num];
                desc->buffer1_addr = (uint32_t)(&rx_buff1[num][0]);
            }

            /* configure descriptors */
            desc->status = desc_status;
            desc->control_buffer_size = desc_bufsize;

            /* when it is the last descriptor */
            if(num == (count - 1U)) {
                if(ENET_DMA_TX == direction) {
                    /* configure transmit end of ring mode */
                    desc->status |= ENET_TDES0_TERM;
                } else {
                    /* configure receive end of ring mode */
                    desc->control_buffer_size |= ENET_RDES1_RERM;
                }
            }
            /* set desc_ptptab equal to desc_tab */
            (&desc_ptptab[num])->buffer1_addr = desc->buffer1_addr;
            (&desc_ptptab[num])->buffer2_next_desc_addr = desc->buffer2_next_desc_addr;
        }
        /* when it is the last ptp descriptor, preserve the first descriptor
        address of desc_ptptab in ptp descriptor status */
        (&desc_ptptab[num - 1U])->status = (uint32_t)desc_ptptab;
    }
}

/*!
    \brief      receive a packet data with timestamp values to application buffer, when the DMA is in normal mode, API ID: 0x0019
    \param[in]  enet_periph: ENETx(x=0,1)
    \param[in]  bufsize: the size of buffer which is the parameter in function
    \param[out] timestamp: pointer to the table which stores the timestamp high and low
    \param[out] buffer: pointer to the application buffer
                note -- if the input is NULL, user should copy data in application by himself
    \retval     ErrStatus: SUCCESS or ERROR
*/
ErrStatus enet_ptpframe_receive_normal_mode(uint32_t enet_periph, uint8_t buffer[], uint32_t bufsize, uint32_t timestamp[])
{
    uint32_t offset = 0U, size = 0U;
    ErrStatus status = ERROR;

    if(ENET0 == enet_periph) {
        /* the descriptor is busy due to own by the DMA */
        if((uint32_t)RESET == (dma_current_rxdesc0->status & ENET_RDES0_DAV)) {
            /* if buffer pointer is null, indicates that users has copied data in application */
            if(NULL != buffer) {
                /* if no error occurs, and the frame uses only one descriptor */
                if(((uint32_t)RESET == (dma_current_rxdesc0->status & ENET_RDES0_ERRS)) &&
                        ((uint32_t)RESET != (dma_current_rxdesc0->status & ENET_RDES0_LDES)) &&
                        ((uint32_t)RESET != (dma_current_rxdesc0->status & ENET_RDES0_FDES))) {

                    /* get the frame length except CRC */
                    size = GET_RDES0_FRML(dma_current_rxdesc0->status) - 4U;
                    /* if is a type frame, and CRC is not included in forwarding frame */
                    if((RESET != (ENET_MAC_CFG(enet_periph) & ENET_MAC_CFG_TFCD)) && (RESET != (dma_current_rxdesc0->status & ENET_RDES0_FRMT))) {
                        size = size + 4U;
                    }

                    /* to avoid situation that the frame size exceeds the buffer length */
                    if(size <= bufsize) {
                        /* copy data from Rx buffer to application buffer */
                        for(offset = 0U; offset < size; offset++) {
                            buffer[offset] = (*(__IO uint8_t *)(uint32_t)((dma_current_ptp_rxdesc0->buffer1_addr) + offset));
                        }
                        status = SUCCESS;
                    }
                }
            } else {
                status = SUCCESS;
            }

            if(status == SUCCESS) {
                /* copy timestamp value from Rx descriptor to application array */
                if(NULL != timestamp) {
                    timestamp[0] = dma_current_rxdesc0->buffer1_addr;
                    timestamp[1] = dma_current_rxdesc0->buffer2_next_desc_addr;
                }

                dma_current_rxdesc0->buffer1_addr = dma_current_ptp_rxdesc0->buffer1_addr;
                dma_current_rxdesc0->buffer2_next_desc_addr = dma_current_ptp_rxdesc0->buffer2_next_desc_addr;

                /* enable reception, descriptor is owned by DMA */
                dma_current_rxdesc0->status = ENET_RDES0_DAV;

                /* check Rx buffer unavailable flag status */
                if((uint32_t)RESET != (ENET_DMA_STAT(enet_periph) & ENET_DMA_STAT_RBU)) {
                    /* clear RBU flag */
                    ENET_DMA_STAT(enet_periph) = ENET_DMA_STAT_RBU;
                    /* resume DMA reception by writing to the RPEN register*/
                    ENET_DMA_RPEN(enet_periph) = 0U;
                }

                /* update the current RxDMA descriptor pointer to the next descriptor in RxDMA descriptor table */
                /* chained mode */
                if((uint32_t)RESET != (dma_current_rxdesc0->control_buffer_size & ENET_RDES1_RCHM)) {
                    dma_current_rxdesc0 = (enet_descriptors_struct *)(dma_current_ptp_rxdesc0->buffer2_next_desc_addr);
                    /* if it is the last ptp descriptor */
                    if(0U != dma_current_ptp_rxdesc0->status) {
                        /* pointer back to the first ptp descriptor address in the desc_ptptab list address */
                        dma_current_ptp_rxdesc0 = (enet_descriptors_struct *)(dma_current_ptp_rxdesc0->status);
                    } else {
                        /* pointer to the next ptp descriptor */
                        dma_current_ptp_rxdesc0++;
                    }
                } else {
                    /* ring mode */
                    if((uint32_t)RESET != (dma_current_rxdesc0->control_buffer_size & ENET_RDES1_RERM)) {
                        /* if is the last descriptor in table, the next descriptor is the table header */
                        dma_current_rxdesc0 = (enet_descriptors_struct *)(ENET_DMA_RDTADDR(enet_periph));
                        /* RDES2 and RDES3 will not be covered by buffer address, so do not need to preserve a new table,
                        use the same table with RxDMA descriptor */
                        dma_current_ptp_rxdesc0 = (enet_descriptors_struct *)(dma_current_ptp_rxdesc0->status);
                    } else {
                        /* the next descriptor is the current address, add the descriptor size, and descriptor skip length */
                        dma_current_rxdesc0 = (enet_descriptors_struct *)(uint32_t)((uint32_t)dma_current_rxdesc0 + ETH_DMARXDESC_SIZE + GET_DMA_BCTL_DPSL(ENET_DMA_BCTL(
                                                  enet_periph)));
                        dma_current_ptp_rxdesc0++;
                    }
                }
            }
        }
    } else {
        /* the descriptor is busy due to own by the DMA */
        if((uint32_t)RESET == (dma_current_rxdesc1->status & ENET_RDES0_DAV)) {
            /* if buffer pointer is null, indicates that users has copied data in application */
            if(NULL != buffer) {
                /* if no error occurs, and the frame uses only one descriptor */
                if(((uint32_t)RESET == (dma_current_rxdesc1->status & ENET_RDES0_ERRS)) &&
                        ((uint32_t)RESET != (dma_current_rxdesc1->status & ENET_RDES0_LDES)) &&
                        ((uint32_t)RESET != (dma_current_rxdesc1->status & ENET_RDES0_FDES))) {

                    /* get the frame length except CRC */
                    size = GET_RDES0_FRML(dma_current_rxdesc1->status) - 4U;
                    /* if is a type frame, and CRC is not included in forwarding frame */
                    if((RESET != (ENET_MAC_CFG(enet_periph) & ENET_MAC_CFG_TFCD)) && (RESET != (dma_current_rxdesc1->status & ENET_RDES0_FRMT))) {
                        size = size + 4U;
                    }

                    /* to avoid situation that the frame size exceeds the buffer length */
                    if(size <= bufsize) {
                        /* copy data from Rx buffer to application buffer */
                        for(offset = 0U; offset < size; offset++) {
                            buffer[offset] = (*(__IO uint8_t *)(uint32_t)((dma_current_ptp_rxdesc1->buffer1_addr) + offset));
                        }
                        status = SUCCESS;
                    }
                }
            } else {
                status = SUCCESS;
            }

            if(status == SUCCESS) {
                /* copy timestamp value from Rx descriptor to application array */
                if(NULL != timestamp) {
                    timestamp[0] = dma_current_rxdesc1->buffer1_addr;
                    timestamp[1] = dma_current_rxdesc1->buffer2_next_desc_addr;
                }

                dma_current_rxdesc1->buffer1_addr = dma_current_ptp_rxdesc1->buffer1_addr;
                dma_current_rxdesc1->buffer2_next_desc_addr = dma_current_ptp_rxdesc1->buffer2_next_desc_addr;

                /* enable reception, descriptor is owned by DMA */
                dma_current_rxdesc1->status = ENET_RDES0_DAV;

                /* check Rx buffer unavailable flag status */
                if((uint32_t)RESET != (ENET_DMA_STAT(enet_periph) & ENET_DMA_STAT_RBU)) {
                    /* clear RBU flag */
                    ENET_DMA_STAT(enet_periph) = ENET_DMA_STAT_RBU;
                    /* resume DMA reception by writing to the RPEN register*/
                    ENET_DMA_RPEN(enet_periph) = 0U;
                }

                /* update the current RxDMA descriptor pointer to the next descriptor in RxDMA descriptor table */
                /* chained mode */
                if((uint32_t)RESET != (dma_current_rxdesc1->control_buffer_size & ENET_RDES1_RCHM)) {
                    dma_current_rxdesc1 = (enet_descriptors_struct *)(dma_current_ptp_rxdesc1->buffer2_next_desc_addr);
                    /* if it is the last ptp descriptor */
                    if(0U != dma_current_ptp_rxdesc1->status) {
                        /* pointer back to the first ptp descriptor address in the desc_ptptab list address */
                        dma_current_ptp_rxdesc1 = (enet_descriptors_struct *)(dma_current_ptp_rxdesc1->status);
                    } else {
                        /* pointer to the next ptp descriptor */
                        dma_current_ptp_rxdesc1++;
                    }
                } else {
                    /* ring mode */
                    if((uint32_t)RESET != (dma_current_rxdesc1->control_buffer_size & ENET_RDES1_RERM)) {
                        /* if is the last descriptor in table, the next descriptor is the table header */
                        dma_current_rxdesc1 = (enet_descriptors_struct *)(ENET_DMA_RDTADDR(enet_periph));
                        /* RDES2 and RDES3 will not be covered by buffer address, so do not need to preserve a new table,
                        use the same table with RxDMA descriptor */
                        dma_current_ptp_rxdesc1 = (enet_descriptors_struct *)(dma_current_ptp_rxdesc1->status);
                    } else {
                        /* the next descriptor is the current address, add the descriptor size, and descriptor skip length */
                        dma_current_rxdesc1 = (enet_descriptors_struct *)(uint32_t)((uint32_t)dma_current_rxdesc1 + ETH_DMARXDESC_SIZE + GET_DMA_BCTL_DPSL(ENET_DMA_BCTL(
                                                  enet_periph)));
                        dma_current_ptp_rxdesc1++;
                    }
                }
            }
        }
    }

    return status;
}

/*!
    \brief      send data with timestamp values in application buffer as a transmit packet, when the DMA is in normal mode, API ID: 0x001A
    \param[in]  enet_periph: ENETx(x=0,1)
    \param[in]  buffer: pointer on the application buffer
                note -- if the input is NULL, user should copy data in application by himself
    \param[in]  length: the length of frame data to be transmitted
    \param[out] timestamp: pointer to the table which stores the timestamp high and low
                note -- if the input is NULL, timestamp is ignored
    \retval     ErrStatus: SUCCESS or ERROR
*/
ErrStatus enet_ptpframe_transmit_normal_mode(uint32_t enet_periph, uint8_t buffer[], uint32_t length, uint32_t timestamp[])
{
    uint32_t offset = 0U, timeout = 0U;
    uint32_t dma_tbu_flag, dma_tu_flag, tdes0_ttmss_flag;
    ErrStatus status = SUCCESS;

    /* initialize flag for timestamp availability */
    tdes0_ttmss_flag = 0U;

    if(ENET0 == enet_periph) {
        /* check if the descriptor is available */
        if(((uint32_t)RESET != (dma_current_txdesc0->status & ENET_TDES0_DAV)) ||
                (length > ENET_MAX_FRAME_SIZE)) {
            status = ERROR;
        } else {
            /* if buffer pointer is not null, copy frame data */
            if(NULL != buffer) {
                /* copy frame data from application buffer to Tx buffer */
                for(offset = 0U; offset < length; offset++) {
                    (*(__IO uint8_t *)(uint32_t)((dma_current_ptp_txdesc0->buffer1_addr) + offset)) = buffer[offset];
                }
            }

            /* set the frame length */
            dma_current_txdesc0->control_buffer_size = (length & (uint32_t)0x00001FFFU);
            /* set the segment of frame, frame is transmitted in one descriptor */
            dma_current_txdesc0->status |= ENET_TDES0_LSG | ENET_TDES0_FSG;
            /* enable the DMA transmission */
            dma_current_txdesc0->status |= ENET_TDES0_DAV;

            /* check Tx buffer unavailable flag status */
            dma_tbu_flag = (ENET_DMA_STAT(enet_periph) & ENET_DMA_STAT_TBU);
            dma_tu_flag = (ENET_DMA_STAT(enet_periph) & ENET_DMA_STAT_TU);

            if((RESET != dma_tbu_flag) || (RESET != dma_tu_flag)) {
                /* clear TBU and TU flag */
                ENET_DMA_STAT(enet_periph) = (dma_tbu_flag | dma_tu_flag);
                /* resume DMA transmission by writing to the TPEN register*/
                ENET_DMA_TPEN(enet_periph) = 0U;
            }

            /* if timestamp pointer is not null, get timestamp */
            if((NULL != timestamp) && (SUCCESS == status)) {
                /* wait for ENET_TDES0_TTMSS flag to be set, a timestamp was captured */
                do {
                    tdes0_ttmss_flag = (dma_current_txdesc0->status & ENET_TDES0_TTMSS);
                    timeout++;
                } while((RESET == tdes0_ttmss_flag) && (timeout < ENET_DELAY_TO));

                /* Check for timeout error */
                if(ENET_DELAY_TO == timeout) {
                    status = ERROR;
                } else {
                    /* clear the ENET_TDES0_TTMSS flag */
                    dma_current_txdesc0->status &= ~ENET_TDES0_TTMSS;
                    /* get the timestamp value of the transmit frame */
                    timestamp[0] = dma_current_txdesc0->buffer1_addr;
                    timestamp[1] = dma_current_txdesc0->buffer2_next_desc_addr;
                }
            }

            /* Only update descriptor pointers if no error occurred */
            if(SUCCESS == status) {
                dma_current_txdesc0->buffer1_addr = dma_current_ptp_txdesc0->buffer1_addr;
                dma_current_txdesc0->buffer2_next_desc_addr = dma_current_ptp_txdesc0->buffer2_next_desc_addr;

                /* update the current TxDMA descriptor pointer to the next descriptor in TxDMA descriptor table */
                /* chained mode */
                if((uint32_t)RESET != (dma_current_txdesc0->status & ENET_TDES0_TCHM)) {
                    dma_current_txdesc0 = (enet_descriptors_struct *)(dma_current_ptp_txdesc0->buffer2_next_desc_addr);
                    /* if it is the last ptp descriptor */
                    if(0U != dma_current_ptp_txdesc0->status) {
                        /* pointer back to the first ptp descriptor address in the desc_ptptab list address */
                        dma_current_ptp_txdesc0 = (enet_descriptors_struct *)(dma_current_ptp_txdesc0->status);
                    } else {
                        /* pointer to the next ptp descriptor */
                        dma_current_ptp_txdesc0++;
                    }
                } else {
                    /* ring mode */
                    if((uint32_t)RESET != (dma_current_txdesc0->status & ENET_TDES0_TERM)) {
                        /* if is the last descriptor in table, the next descriptor is the table header */
                        dma_current_txdesc0 = (enet_descriptors_struct *)(ENET_DMA_TDTADDR(enet_periph));
                        /* TDES2 and TDES3 will not be covered by buffer address, so do not need to preserve a new table,
                        use the same table with TxDMA descriptor */
                        dma_current_ptp_txdesc0 = (enet_descriptors_struct *)(dma_current_ptp_txdesc0->status);
                    } else {
                        /* the next descriptor is the current address, add the descriptor size, and descriptor skip length */
                        dma_current_txdesc0 = (enet_descriptors_struct *)(uint32_t)((uint32_t)dma_current_txdesc0 + ETH_DMATXDESC_SIZE + GET_DMA_BCTL_DPSL(ENET_DMA_BCTL(
                                                  enet_periph)));
                        dma_current_ptp_txdesc0++;
                    }
                }
            }
        }
    } else {
        /* check if the descriptor is available */
        if(((uint32_t)RESET != (dma_current_txdesc1->status & ENET_TDES0_DAV)) ||
                (length > ENET_MAX_FRAME_SIZE)) {
            status = ERROR;
        } else {
            /* if buffer pointer is not null, copy frame data */
            if(NULL != buffer) {
                /* copy frame data from application buffer to Tx buffer */
                for(offset = 0U; offset < length; offset++) {
                    (*(__IO uint8_t *)(uint32_t)((dma_current_ptp_txdesc1->buffer1_addr) + offset)) = buffer[offset];
                }
            }

            /* set the frame length */
            dma_current_txdesc1->control_buffer_size = (length & (uint32_t)0x00001FFFU);
            /* set the segment of frame, frame is transmitted in one descriptor */
            dma_current_txdesc1->status |= ENET_TDES0_LSG | ENET_TDES0_FSG;
            /* enable the DMA transmission */
            dma_current_txdesc1->status |= ENET_TDES0_DAV;

            /* check Tx buffer unavailable flag status */
            dma_tbu_flag = (ENET_DMA_STAT(enet_periph) & ENET_DMA_STAT_TBU);
            dma_tu_flag = (ENET_DMA_STAT(enet_periph) & ENET_DMA_STAT_TU);

            if((RESET != dma_tbu_flag) || (RESET != dma_tu_flag)) {
                /* clear TBU and TU flag */
                ENET_DMA_STAT(enet_periph) = (dma_tbu_flag | dma_tu_flag);
                /* resume DMA transmission by writing to the TPEN register*/
                ENET_DMA_TPEN(enet_periph) = 0U;
            }

            /* if timestamp pointer is not null, get timestamp */
            if((NULL != timestamp) && (SUCCESS == status)) {
                /* wait for ENET_TDES0_TTMSS flag to be set, a timestamp was captured */
                do {
                    tdes0_ttmss_flag = (dma_current_txdesc1->status & ENET_TDES0_TTMSS);
                    timeout++;
                } while((RESET == tdes0_ttmss_flag) && (timeout < ENET_DELAY_TO));

                /* Check for timeout error */
                if(ENET_DELAY_TO == timeout) {
                    status = ERROR;
                } else {
                    /* clear the ENET_TDES0_TTMSS flag */
                    dma_current_txdesc1->status &= ~ENET_TDES0_TTMSS;
                    /* get the timestamp value of the transmit frame */
                    timestamp[0] = dma_current_txdesc1->buffer1_addr;
                    timestamp[1] = dma_current_txdesc1->buffer2_next_desc_addr;
                }
            }

            /* Only update descriptor pointers if no error occurred */
            if(SUCCESS == status) {
                dma_current_txdesc1->buffer1_addr = dma_current_ptp_txdesc1->buffer1_addr;
                dma_current_txdesc1->buffer2_next_desc_addr = dma_current_ptp_txdesc1->buffer2_next_desc_addr;

                /* update the current TxDMA descriptor pointer to the next descriptor in TxDMA descriptor table */
                /* chained mode */
                if((uint32_t)RESET != (dma_current_txdesc1->status & ENET_TDES0_TCHM)) {
                    dma_current_txdesc1 = (enet_descriptors_struct *)(dma_current_ptp_txdesc1->buffer2_next_desc_addr);
                    /* if it is the last ptp descriptor */
                    if(0U != dma_current_ptp_txdesc1->status) {
                        /* pointer back to the first ptp descriptor address in the desc_ptptab list address */
                        dma_current_ptp_txdesc1 = (enet_descriptors_struct *)(dma_current_ptp_txdesc1->status);
                    } else {
                        /* pointer to the next ptp descriptor */
                        dma_current_ptp_txdesc1++;
                    }
                } else {
                    /* ring mode */
                    if((uint32_t)RESET != (dma_current_txdesc1->status & ENET_TDES0_TERM)) {
                        /* if is the last descriptor in table, the next descriptor is the table header */
                        dma_current_txdesc1 = (enet_descriptors_struct *)(ENET_DMA_TDTADDR(enet_periph));
                        /* TDES2 and TDES3 will not be covered by buffer address, so do not need to preserve a new table,
                        use the same table with TxDMA descriptor */
                        dma_current_ptp_txdesc1 = (enet_descriptors_struct *)(dma_current_ptp_txdesc1->status);
                    } else {
                        /* the next descriptor is the current address, add the descriptor size, and descriptor skip length */
                        dma_current_txdesc1 = (enet_descriptors_struct *)(uint32_t)((uint32_t)dma_current_txdesc1 + ETH_DMATXDESC_SIZE + GET_DMA_BCTL_DPSL(ENET_DMA_BCTL(
                                                  enet_periph)));
                        dma_current_ptp_txdesc1++;
                    }
                }
            }
        }
    }

    return status;
}
#endif /* SELECT_DESCRIPTORS_ENHANCED_MODE */

/*!
    \brief      configure and start PTP timestamp counter, API ID: 0x001B
    \param[in]  enet_periph: ENETx(x=0,1)
    \param[in]  updatemethod: method for updating
      \arg        ENET_PTP_FINEMODE: fine correction method
      \arg        ENET_PTP_COARSEMODE: coarse correction method
    \param[in]  init_sec: second value for initializing system time
    \param[in]  init_subsec: subsecond value for initializing system time
    \param[in]  carry_cfg: the value to be added to the accumulator register (in fine method is used)
    \param[in]  accuracy_cfg: the value to be added to the subsecond value of system time
    \param[out] none
    \retval     none
*/
void enet_ptp_start(uint32_t enet_periph, int32_t updatemethod, uint32_t init_sec, uint32_t init_subsec, uint32_t carry_cfg, uint32_t accuracy_cfg)
{
    /* mask the timestamp trigger interrupt */
    enet_interrupt_disable(enet_periph, ENET_MAC_INT_TMSTIM);

    /* enable timestamp */
    enet_ptp_feature_enable(enet_periph, ENET_ALL_RX_TIMESTAMP | ENET_RXTX_TIMESTAMP);

    /* configure system time subsecond increment based on the PTP clock frequency */
    enet_ptp_subsecond_increment_config(enet_periph, accuracy_cfg);

    if(ENET_PTP_FINEMODE == updatemethod) {
        /* fine correction method: configure the timestamp addend, then update */
        enet_ptp_timestamp_addend_config(enet_periph, carry_cfg);
        enet_ptp_timestamp_function_config(enet_periph, ENET_PTP_ADDEND_UPDATE);
        /* wait until update is completed */
        while(SET == enet_ptp_flag_get(enet_periph, (uint32_t)ENET_PTP_ADDEND_UPDATE)) {
        }
    }

    /* choose the fine correction method */
    enet_ptp_timestamp_function_config(enet_periph, (enet_ptp_function_enum)updatemethod);

    /* initialize the system time */
    enet_ptp_timestamp_update_config(enet_periph, ENET_PTP_ADD_TO_TIME, init_sec, init_subsec);
    enet_ptp_timestamp_function_config(enet_periph, ENET_PTP_SYSTIME_INIT);

#if SELECT_DESCRIPTORS_ENHANCED_MODE
    enet_desc_select_enhanced_mode(enet_periph);
#endif /* SELECT_DESCRIPTORS_ENHANCED_MODE */
}

/*!
    \brief      adjust frequency in fine method by configure addend register, API ID: 0x001C
    \param[in]  enet_periph: ENETx(x=0,1)
    \param[in]  carry_cfg: the value to be added to the accumulator register
    \param[out] none
    \retval     none
*/
void enet_ptp_finecorrection_adjfreq(uint32_t enet_periph, int32_t carry_cfg)
{
    /* re-configure the timestamp addend, then update */
    enet_ptp_timestamp_addend_config(enet_periph, (uint32_t)carry_cfg);
    enet_ptp_timestamp_function_config(enet_periph, ENET_PTP_ADDEND_UPDATE);
}

/*!
    \brief      update system time in coarse method, API ID: 0x001D
    \param[in]  enet_periph: ENETx(x=0,1)
    \param[in]  systime_struct: pointer to a enet_ptp_systime_struct structure which contains
                parameters of PTP system time
                members of the structure and the member values are shown as below:
                  second: 0x0 - 0xFFFF FFFF
                  nanosecond: 0x0 - 0x7FFF FFFF * 10^9 / 2^31
                  sign: ENET_PTP_TIME_POSITIVE, ENET_PTP_TIME_NEGATIVE
    \param[out] none
    \retval     none
*/
void enet_ptp_coarsecorrection_systime_update(uint32_t enet_periph, enet_ptp_systime_struct *systime_struct)
{
    uint32_t subsecond_val;
    uint32_t carry_cfg;

    subsecond_val = enet_ptp_nanosecond_2_subsecond(systime_struct->nanosecond);

    /* save the carry_cfg value */
    carry_cfg = ENET_PTP_TSADDEND_TMSA;

    /* update the system time */
    enet_ptp_timestamp_update_config(enet_periph, systime_struct->sign, systime_struct->second, subsecond_val);
    enet_ptp_timestamp_function_config(enet_periph, ENET_PTP_SYSTIME_UPDATE);

    /* wait until the update is completed */
    while(SET == enet_ptp_flag_get(enet_periph, (uint32_t)ENET_PTP_SYSTIME_UPDATE)) {
    }

    /* write back the carry_cfg value, then update */
    enet_ptp_timestamp_addend_config(enet_periph, carry_cfg);
    enet_ptp_timestamp_function_config(enet_periph, ENET_PTP_ADDEND_UPDATE);
}

/*!
    \brief      set system time in fine method, API ID: 0x001E
    \param[in]  enet_periph: ENETx(x=0,1)
    \param[in]  systime_struct: pointer to a enet_ptp_systime_struct structure which contains
                parameters of PTP system time
                members of the structure and the member values are shown as below:
                  second: 0x0 - 0xFFFF FFFF
                  nanosecond: 0x0 - 0x7FFF FFFF * 10^9 / 2^31
                  sign: ENET_PTP_TIME_POSITIVE, ENET_PTP_TIME_NEGATIVE
    \param[out] none
    \retval     none
*/
void enet_ptp_finecorrection_settime(uint32_t enet_periph, enet_ptp_systime_struct *systime_struct)
{
    uint32_t subsecond_val;
    subsecond_val = enet_ptp_nanosecond_2_subsecond(systime_struct->nanosecond);

    /* initialize the system time */
    enet_ptp_timestamp_update_config(enet_periph, systime_struct->sign, systime_struct->second, subsecond_val);
    enet_ptp_timestamp_function_config(enet_periph, ENET_PTP_SYSTIME_INIT);

    /* wait until the system time initialization finished */
    while(SET == enet_ptp_flag_get(enet_periph, (uint32_t)ENET_PTP_SYSTIME_INIT)) {
    }
}

/*!
    \brief      change nanosecond to subsecond
    \param[in]  nanosecond: nanosecond value
    \param[out] none
    \retval     the subsecond value
*/
static uint32_t enet_ptp_nanosecond_2_subsecond(uint32_t nanosecond)
{
    uint64_t val = nanosecond * 0x80000000ULL;
    val /= 1000000000U;
    return (uint32_t)val;
}
