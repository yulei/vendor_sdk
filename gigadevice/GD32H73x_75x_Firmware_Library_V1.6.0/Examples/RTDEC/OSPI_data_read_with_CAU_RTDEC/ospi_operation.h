/*!
    \file    opsi_operation.h
    \brief   the header file of opsi_operation
    
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

#ifndef OSPI_OPERATION_H
#define OSPI_OPERATION_H

#include "gd32h73x_75x.h"

/* GD25LX512ME size configuration */
#define GD25LX512ME_BLOCK_64K                    (uint32_t)(16*4*1024)                 /* 16 sectors of 64KBytes */
#define GD25LX512ME_SECTOR_4K                    (uint32_t)(4 * 1024)                  /* 1 sectors of 4KBytes */

#define GD25LX512ME_FLASH_SIZE                   (uint32_t)(512*1024*1024/8)           /* 512 Mbits => 64MBytes */
#define GD25LX512ME_PAGE_SIZE                    (uint32_t)256                         /* 262144 pages of 256 Bytes */

/* GD25LX512ME timing configuration */
#define GD25LX512ME_BULK_ERASE_MAX_TIME          460000U
#define GD25LX512ME_SECTOR_ERASE_MAX_TIME        1000U
#define GD25LX512ME_SUBSECTOR_4K_ERASE_MAX_TIME  400U
#define GD25LX512ME_WRITE_REG_MAX_TIME           40U

#define GD25LX512ME_RESET_MAX_TIME               100U                                  /* when software reset during erase operation */

#define GD25LX512ME_AUTOPOLLING_INTERVAL_TIME    (uint8_t)0x10U

/*  GD25LX512ME Error codes */
#define GD25LX512ME_OK                           (0)
#define GD25LX512ME_ERROR                        (-1)

/* GD25LX512ME commands */
/* read/write memory operations with 3-bytes address */
#define GD25LX512ME_READ_CMD                                   (uint8_t)0x03U       /* normal read 3 bytes address */
#define GD25LX512ME_FAST_READ_CMD                              (uint8_t)0x0BU       /* fast read 3 bytes address */
#define GD25LX512ME_OCTAL_OUTPUT_FAST_READ_CMD                 (uint8_t)0x8BU       /* octal output fast read 3 bytes address */
#define GD25LX512ME_OCTAL_IO_FAST_READ_CMD                     (uint8_t)0xCBU       /* octal I/O fast read 3 bytes address */

#define GD25LX512ME_PAGE_PROG_CMD                              (uint8_t)0x02U       /* page program 3 bytes address */
#define GD25LX512ME_OCTAL_PAGE_PROG_CMD                        (uint8_t)0x82U       /* octal page program 3 bytes address */
#define GD25LX512ME_EXT_OCTAL_PAGE_PROG_CMD                    (uint8_t)0xC2U       /* extended octal page program 3 bytes address */

#define GD25LX512ME_SECTOR_ERASE_4K_CMD                        (uint8_t)0x20U       /* sector erase 4KB 3 bytes address */
#define GD25LX512ME_BLOCK_ERASE_32K_CMD                        (uint8_t)0x52U       /* block erase 32KB 3 bytes address */
#define GD25LX512ME_BLOCK_ERASE_64K_CMD                        (uint8_t)0xD8U       /* block erase 64KB 3 bytes address */
#define GD25LX512ME_CHIP_ERASE_CMD                             (uint8_t)0x60U       /* chip erase */

/* read/write memory operations with 4-bytes address */
#define GD25LX512ME_4_BYTE_ADDR_READ_CMD                       (uint8_t)0x13U       /* normal read 4 bytes address */
#define GD25LX512ME_4_BYTE_ADDR_FAST_READ_CMD                  (uint8_t)0x0CU       /* fast read 4 bytes address */
#define GD25LX512ME_4_BYTE_ADDR_OCTAL_OUTPUT_FAST_READ_CMD     (uint8_t)0x7CU       /* octal output fast read 4 bytes address */
#define GD25LX512ME_4_BYTE_ADDR_OCTAL_IO_FAST_READ_CMD         (uint8_t)0xCCU       /* octal I/O fast read 4 bytes address */
#define GD25LX512ME_4_BYTE_ADDR_OCTAL_IO_DTR_FAST_READ_CMD     (uint8_t)0xFDU       /* octal I/O DTR fast read 4 bytes address */

#define GD25LX512ME_4_BYTE_PAGE_PROG_CMD                       (uint8_t)0x12U       /* page program 4 bytes address */
#define GD25LX512ME_4_BYTE_OCTAL_PAGE_PROG_CMD                 (uint8_t)0x84U       /* octal page program 4 bytes address */
#define GD25LX512ME_4_BYTE_EXT_OCTAL_PAGE_PROG_CMD             (uint8_t)0x8EU       /* extended octal page program 4 byte address */

#define GD25LX512ME_4_BYTE_SECTOR_ERASE_4K_CMD                 (uint8_t)0x21U       /* sector erase 4KB 4 bytes address */
#define GD25LX512ME_4_BYTE_BLOCK_ERASE_32K_CMD                 (uint8_t)0x5CU       /* block erase 32KB 4 byte address */
#define GD25LX512ME_4_BYTE_BLOCK_ERASE_64K_CMD                 (uint8_t)0xDCU       /* block erase 64KB 4 byte address */

/* setting commands */
#define GD25LX512ME_WRITE_ENABLE_CMD                           (uint8_t)0x06U       /* write enable */
#define GD25LX512ME_WRITE_DISABLE_CMD                          (uint8_t)0x04U       /* write disable */
#define GD25LX512ME_PROG_ERASE_SUSPEND_CMD                     (uint8_t)0x75U       /* program/erase suspend */
#define GD25LX512ME_PROG_ERASE_RESUME_CMD                      (uint8_t)0x7AU       /* program/erase resume */
#define GD25LX512ME_ENTER_DEEP_POWER_DOWN_CMD                  (uint8_t)0xB9U       /* enter deep power down */
#define GD25LX512ME_RELEASE_FROM_DEEP_POWER_DOWN_CMD           (uint8_t)0xABU       /* release form deep power down */
#define GD25LX512ME_ENABLE_4_BYTE_ADDR_CMD                     (uint8_t)0xB7U       /* enable 4 byte address mode */
#define GD25LX512ME_DISABLE_4_BYTE_ADDR_CMD                    (uint8_t)0xE9U       /* disable 4 byte address mode */

/* advanced sector protection commands */
#define GD25LX512ME_INDIVIDUAL_BLOCK_SECTOR_LOCK_CMD           (uint8_t)0x36U       /* individual block/sector lock */
#define GD25LX512ME_INDIVIDUAL_BLOCK_SECTOR_UNLOCK_CMD         (uint8_t)0x39U       /* individual block/sector unlock */
#define GD25LX512ME_READ_INDIVIDUAL_BLOCK_SECTOR_LOCK_CMD      (uint8_t)0x3DU       /* read individual block/sector lock */
#define GD25LX512ME_GLOBAL_BLOCK_SECTOR_LOCK_CMD               (uint8_t)0x7EU       /* global block/sector lock */
#define GD25LX512ME_GLOBAL_BLOCK_SECTOR_UNLOCK_CMD             (uint8_t)0x98U       /* global block/sector unlock */

/* reset commands */
#define GD25LX512ME_RESET_ENABLE_CMD                           (uint8_t)0x66U       /*!< reset enable */
#define GD25LX512ME_RESET_MEMORY_CMD                           (uint8_t)0x99U       /*!< reset memory */

/* register commands (SPI) */
#define GD25LX512ME_READ_ID_CMD                                (uint8_t)0x9FU       /* read identification */
#define GD25LX512ME_READ_SERIAL_FLASH_DISCO_PARAM_CMD          (uint8_t)0x5AU       /* read serial flash discoverable parameter */
#define GD25LX512ME_READ_UNIQUE_ID_CMD                         (uint8_t)0x4BU       /* read unique identification */

#define GD25LX512ME_READ_STATUS_REG_CMD                        (uint8_t)0x05U       /* read status register */
#define GD25LX512ME_READ_FLAG_STATUS_REG_CMD                   (uint8_t)0x70U       /* read flag status register */
#define GD25LX512ME_READ_EXT_ADDR_REG_CMD                      (uint8_t)0xC8U       /* read extended address register */
#define GD25LX512ME_READ_NONVOLATILE_CFG_REG_CMD               (uint8_t)0xB5U       /* read nonvolatile configuration register */
#define GD25LX512ME_READ_VOLATILE_CFG_REG_CMD                  (uint8_t)0x85U       /* read volatile configuration register */

#define GD25LX512ME_WRITE_STATUS_REG_CMD                       (uint8_t)0x01U       /* write status register */
#define GD25LX512ME_WRITE_EXT_ADDR_REG_CMD                     (uint8_t)0xC5U       /* write extended address register */
#define GD25LX512ME_WRITE_ENABLE_VOLATILE_STATUS_CFG_CMD       (uint8_t)0x50U       /* write enable for volatile status register */
#define GD25LX512ME_WRITE_NONVOLATILE_CFG_REG_CMD              (uint8_t)0xB1U       /* write nonvolatile configuration register */
#define GD25LX512ME_WRITE_VOLATILE_CFG_REG_CMD                 (uint8_t)0x81U       /* write volatile configuration register */

#define GD25LX512ME_READ_SECURITY_REG_CMD                      (uint8_t)0x48U       /* read security register */
#define GD25LX512ME_PROG_SECURITY_REG_CMD                      (uint8_t)0x42U       /* program security register */
#define GD25LX512ME_ERASE_SECURITY_REG_CMD                     (uint8_t)0x44U       /* erase Security register */


/* GD25LX512ME registers */
/* status register */
#define GD25LX512ME_SR_WIP                                     (uint8_t)0x01U       /* write in progress */
#define GD25LX512ME_SR_WEL                                     (uint8_t)0x02U       /* write enable latch */
#define GD25LX512ME_SR_PB                                      (uint8_t)0x3CU       /* block protected against program and erase operations */

/* flag status register */
#define GD25LX512ME_FSR_ADS                                    (uint8_t)0x01U       /* current address mode */
#define GD25LX512ME_FSR_PTE                                    (uint8_t)0x02U       /* protection error bit */
#define GD25LX512ME_FSR_SUS2                                   (uint8_t)0x04U       /* program suspend */
#define GD25LX512ME_FSR_PE                                     (uint8_t)0x10U       /* program error bit */
#define GD25LX512ME_FSR_EE                                     (uint8_t)0x20U       /* erase error bit */
#define GD25LX512ME_FSR_SUS1                                   (uint8_t)0x40U       /* erase suspend */
#define GD25LX512ME_FSR_RY_BY                                  (uint8_t)0x80U       /* ready/busy */

/* extended address register */
#define GD25LX512ME_EAR_A24                                    (uint8_t)0x01U       /* address bit 24 */
#define GD25LX512ME_EAR_A25                                    (uint8_t)0x02U       /* address bit 25 */
#define GD25LX512ME_EAR_SEC                                    (uint8_t)0x80U       /* single error correction bit */

/* configuration register */
/* address : 0x00000000 */
#define GD25LX512ME_CFG_REG0_ADDR                              (uint32_t)0x00000000U  /*!< CFG register address 0x00000000 */
#define GD25LX512ME_CFG_REG0_ADDR                              (uint32_t)0x00000000U  /*!< CFG register address 0x00000000                    */
#define GD25LX512ME_CFG_SPI                                    (uint8_t)0xFFU         /*!< SPI with DQS(Default)                              */
#define GD25LX512ME_CFG_SPI_WO                                 (uint8_t)0xDFU         /*!< SPI W/O DQS                                        */
#define GD25LX512ME_CFG_OCTAL_DTR                              (uint8_t)0xE7U         /*!< Octal DTR with DQS                                 */
#define GD25LX512ME_CFG_OCTAL_DTR_WO                           (uint8_t)0xC7U         /*!< Octal DTR W/O DQS                                  */
#define GD25LX512ME_CFG_OCTAL_STR                              (uint8_t)0xB7U         /*!< Octal STR with DQS                                 */
#define GD25LX512ME_CFG_OCTAL_STR_WO                           (uint8_t)0x97U         /*!< Octal STR W/O DQS                                  */

/* address : 0x00000001 */
#define GD25LX512ME_CFG_REG1_ADDR                              (uint32_t)0x00000001U               /* CFG register address 0x00000001 */
#define GD25LX512ME_CFG_DUMMY_CYCLE(regval)                    (BITS(0,5) & (uint32_t)(regval))    /* 5~30 dummy cycle */
#define GD25LX512ME_CFG_6_DUMMY_CYCLES                         (uint8_t)0x06U                      /* 6 dummy cycles */
#define GD25LX512ME_CFG_8_DUMMY_CYCLES                         (uint8_t)0x08U                      /* 8 dummy cycles */
#define GD25LX512ME_CFG_10_DUMMY_CYCLES                        (uint8_t)0x0AU                      /* 10 dummy cycles */
#define GD25LX512ME_CFG_12_DUMMY_CYCLES                        (uint8_t)0x0CU                      /* 12 dummy cycles */
#define GD25LX512ME_CFG_14_DUMMY_CYCLES                        (uint8_t)0x0EU                      /* 14 dummy cycles */
#define GD25LX512ME_CFG_16_DUMMY_CYCLES                        (uint8_t)0x10U                      /* 16 dummy cycles(default) */
#define GD25LX512ME_CFG_18_DUMMY_CYCLES                        (uint8_t)0x12U                      /* 18 dummy cycles */
#define GD25LX512ME_CFG_20_DUMMY_CYCLES                        (uint8_t)0x14U                      /* 20 dummy cycles */

/* address : 0x00000002 */
#define GD25LX512ME_CFG_REG2_ADDR                              (uint32_t)0x00000002U               /* CFG register address 0x00000002 */
#define GD25LX512ME_CFG_SECURITY_REG_UNLOCKED                  (uint8_t)0xF0U                      /* security register unlocked(default) */
#define GD25LX512ME_CFG_SECURITY_REG_LOCKED                    (uint8_t)0xF1U                      /* security register locked */
#define GD25LX512ME_CFG_SRP1_UNLOCKED                          (uint8_t)0x0FU                      /* SRP1 unlocked(default) */
#define GD25LX512ME_CFG_SRP1_LOCKED                            (uint8_t)0x1FU                      /* SRP1 locked */

/* address : 0x00000003 */
#define GD25LX512ME_CFG_REG3_ADDR                              (uint32_t)0x00000003U               /* CFG register address 0x00000003 */
#define GD25LX512ME_CFG_50_OHM                                 (uint8_t)0xFFU                      /* driver strength configuration 50 Ohm(default) */
#define GD25LX512ME_CFG_35_OHM                                 (uint8_t)0xFEU                      /* driver strength configuration 35 Ohm */
#define GD25LX512ME_CFG_25_OHM                                 (uint8_t)0xFDU                      /* driver strength configuration 25 Ohm */
#define GD25LX512ME_CFG_18_OHM                                 (uint8_t)0xFCU                      /* driver strength configuration 18 Ohm */

/* address : 0x00000004 */
#define GD25LX512ME_CFG_REG4_ADDR                              (uint32_t)0x00000004U               /* CFG register address 0x00000004 */
#define GD25LX512ME_CFG_REG4(regval)                           (BITS(0,1) & (uint32_t)(regval))    /* follow below table */

/* address : 0x00000005 */
#define GD25LX512ME_CFG_REG5_ADDR                              (uint32_t)0x00000005U               /* CFG register address 0x00000005 */
#define GD25LX512ME_CFG_3_BYTE_ADDR                            (uint8_t)0xFFU                      /* 3-bytes address(default) */
#define GD25LX512ME_CFG_4_BYTE_ADDR                            (uint8_t)0xFEU                      /* 4-bytes address */

/* address : 0x00000006 */
#define GD25LX512ME_CFG_REG6_ADDR                              (uint32_t)0x00000006U               /* CFG register address 0x00000006 */
#define GD25LX512ME_CFG_XIP_DISABLE                            (uint8_t)0xFFU                      /* XIP disable(default) */
#define GD25LX512ME_CFG_XIP_ENABLE                             (uint8_t)0xFEU                      /*!< XIP enable */

/* address : 0x00000007 */
#define GD25LX512ME_CFG_REG7_ADDR                              (uint32_t)0x00000007U               /*!< CFG register address 0x00000007 */
#define GD25LX512ME_CFG_WRAP_DISABLE                           (uint8_t)0xFFU                      /*!< Wrap disable(default) */
#define GD25LX512ME_CFG_WRAP_64_BYTE                           (uint8_t)0xFEU                      /*!< 64-Byte wrap */
#define GD25LX512ME_CFG_WRAP_32_BYTE                           (uint8_t)0xFDU                      /*!< 32-Byte wrap */
#define GD25LX512ME_CFG_WRAP_16_BYTE                           (uint8_t)0xFCU                      /*!< 16-Byte wrap */

typedef enum {
  SPI_MODE = 0,                             /* 1-1-1 commands, power on H/W default setting  */
  OSPI_MODE                                 /* 8-8-8 commands */
}interface_mode;

/* configure OSPI/OSPIM and GPIO */
void ospi_config(uint32_t ospi_periph, ospi_parameter_struct *ospi_struct);
/* read the flah id */
uint32_t ospi_flash_read_id(uint32_t ospi_periph, ospi_parameter_struct * ospi_struct);
/* enable flash memory reset */
void ospi_flash_reset_enable(uint32_t ospi_periph, ospi_parameter_struct *ospi_struct, interface_mode mode);
/* reset the flash */
void ospi_flash_reset_memory(uint32_t ospi_periph, ospi_parameter_struct *ospi_struct, interface_mode mode);
/* polling WIP(write in progress) bit become to 0 */
void ospi_flash_autopolling_memory_ready(uint32_t ospi_periph, ospi_parameter_struct *ospi_struct, interface_mode mode);
/* enable flash write */
void ospi_flash_write_enable(uint32_t ospi_periph, ospi_parameter_struct *ospi_struct);
/* erase flash sector */
void ospi_flash_sector_erase(uint32_t ospi_periph, ospi_parameter_struct *ospi_struct, uint32_t sector_addr);
/* program flash page */
void ospi_flash_page_program(uint32_t ospi_periph, ospi_parameter_struct *ospi_struct, uint32_t write_addr, uint32_t datasize, uint8_t *pdata);
/* read flash data */
void ospi_flash_data_read(uint32_t ospi_periph, ospi_parameter_struct *ospi_struct, uint32_t read_addr, uint32_t datasize, uint8_t *pdata);
/* write flash volatile configuration register */
void ospi_flash_write_volatile_cfg_reg(uint32_t ospi_periph, ospi_parameter_struct *ospi_struct, uint32_t write_addr, uint8_t value);
/* enable memory mapped mode for the OSPI memory */
void ospi_memory_map_enable(uint32_t ospi_periph, ospi_parameter_struct *ospi_struct);

#endif /* OSPI_OPERATION_H */
