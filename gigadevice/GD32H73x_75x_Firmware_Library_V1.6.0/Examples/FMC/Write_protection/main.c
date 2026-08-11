/*!
    \file    main.c
    \brief   main flash program, erase

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
#include "gd32h759i_eval.h"
#include <stdio.h>

#define FLASH_SECTOR_PROGRAM
#define WRITE_PROTECTION_ENABLE
//#define WRITE_PROTECTION_DISABLE

typedef enum {FAILED = 0, PASSED = !FAILED} test_state;
#define FLASH_SECTOR_SIZE       ((uint16_t)0x1000)
#define FMC_SECTORS_TO_PROTECT   (OB_WP_15)

#define FMC_WRITE_START_ADDR    ((uint32_t)0x080F0000U)    /* sector 240 */
#define FMC_WRITE_END_ADDR      ((uint32_t)0x080F0200U)

uint32_t erase_counter = 0x0, address = 0x0;
uint32_t data = 0x12345678;
uint32_t wp_value = 0x3FFFFFFF, protected_sectors = 0x0;
uint32_t sector_number = 1U;
__IO fmc_state_enum fmc_state = FMC_READY;
__IO test_state program_state = PASSED;

void fmc_all_flags_clear(void);

/*!
    \brief      enable the CPU cache
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
    \brief      clear all FMC flag status
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_all_flags_clear(void)
{
    fmc_flag_clear(FMC_FLAG_END);
    fmc_flag_clear(FMC_FLAG_WPERR);
    fmc_flag_clear(FMC_FLAG_PGSERR);
    fmc_flag_clear(FMC_FLAG_RPERR);
    fmc_flag_clear(FMC_FLAG_RSERR);
    fmc_flag_clear(FMC_FLAG_ECCCOR);
    fmc_flag_clear(FMC_FLAG_ECCDET);
    fmc_flag_clear(FMC_FLAG_OBMERR);
}


/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* enable the CPU Cache */
    cache_enable();
    /* initialize led on the board */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    /* unlock the flash program/erase controller */
    fmc_unlock();
    ob_unlock();

    /* clear all pending flags */
    fmc_all_flags_clear();

    /* get sectors write protection status */
    wp_value = ob_write_protection_get() & 0x3FFFFFFF;

#ifdef WRITE_PROTECTION_DISABLE
    /* get sectors already write protected */
    protected_sectors = ~(wp_value | FMC_SECTORS_TO_PROTECT) & 0x3FFFFFFF;
    /* check if desired sectors are already write protected */
    if(((wp_value | (~FMC_SECTORS_TO_PROTECT)) & 0x3FFFFFFF) != 0x3FFFFFFF) {
        /* already write protected */
        ob_write_protection_disable(OB_WP_ALL);
        
        /* clear all pending flags */
        fmc_all_flags_clear();
        
        /* check if there is write protected sectors */
        if(protected_sectors != 0x0) {
            /* Restore write protected sectors */
            FMC_WP_MDF = (~protected_sectors & 0x3FFFFFFF);
        }
        ob_start();
        ob_lock();
        fmc_lock();
        /* generate system reset to load the new option byte values */
        NVIC_SystemReset();
    }
#elif defined WRITE_PROTECTION_ENABLE
    /* get current write protected sectors and the new sectors to be protected */
    protected_sectors = ((~wp_value) | FMC_SECTORS_TO_PROTECT) & 0x3FFFFFFF;

    /* check if desired sectors are not yet write protected */
    if(((~wp_value) & FMC_SECTORS_TO_PROTECT) != FMC_SECTORS_TO_PROTECT) {

        /* enable the sectors write protection */
        fmc_state = ob_write_protection_enable(protected_sectors);
        ob_start();
        ob_lock();
        fmc_lock();
        /* generate system reset to load the new option byte values */
        NVIC_SystemReset();
    }
#endif /* WRITE_PROTECTION_DISABLE */

#ifdef FLASH_SECTOR_PROGRAM

    /* the selected sectors are not write protected */
    if(0x00 != (wp_value & FMC_SECTORS_TO_PROTECT)) {
        /* clear all pending flags */
        fmc_all_flags_clear();

        /* erase the flash sectors */
        for(erase_counter = 0; (erase_counter < sector_number) && (FMC_READY == fmc_state); erase_counter++) {
            fmc_state = fmc_sector_erase(FMC_WRITE_START_ADDR + (FLASH_SECTOR_SIZE * erase_counter));
        }

        /* clear all pending flags */
        fmc_all_flags_clear();

        /* flash word program of data 0x12345678 at addresses defined by FMC_WRITE_START_ADDR and FMC_WRITE_END_ADDR */
        address = FMC_WRITE_START_ADDR;

        while((address < FMC_WRITE_END_ADDR) && (FMC_READY == fmc_state)) {
            fmc_state = fmc_word_program(address, data);
            address = address + 4U;
        }
        /* check the correctness of written data */
        address = FMC_WRITE_START_ADDR;

        while((address < FMC_WRITE_END_ADDR) && (FAILED != program_state)) {
            if(REG32(address) != data) {
                program_state = FAILED;
            }
            address += 4U;
        }
        gd_eval_led_on(LED2);
    } else {
        /* error to program the flash: the desired sectors are write protected */
        program_state = FAILED;
        gd_eval_led_on(LED1);
    }
#endif /* FLASH_SECTOR_PROGRAM */
    while(1) {
    }
}
