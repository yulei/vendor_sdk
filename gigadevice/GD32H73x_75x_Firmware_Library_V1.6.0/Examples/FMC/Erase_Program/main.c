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

void cache_enable(void);
void fmc_all_flags_clear(void);
void fmc_erase_sectors(void);
void fmc_program(void);
void fmc_erase_sectors_check(void);
void fmc_program_check(void);

#define FMC_WRITE_START_ADDR    ((uint32_t)0x08040000U)
#define FMC_WRITE_END_ADDR      ((uint32_t)0x08060000U)
#define MAIN_FLASH_SECTOR_SIZE  ((uint32_t)0x00001000U)

uint32_t *ptrd;
uint32_t address = 0x00000000U;
uint32_t data = 0x01234567U;
led_typedef_enum led_num;

/* calculate the count of sector to be programmed/erased */
uint32_t sector_cnt = (FMC_WRITE_END_ADDR - FMC_WRITE_START_ADDR) / MAIN_FLASH_SECTOR_SIZE;
/* calculate the count of word to be programmed/erased */
uint32_t word_cnt = ((FMC_WRITE_END_ADDR - FMC_WRITE_START_ADDR) >> 2U);

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
    \brief      erase fmc sectors from FMC_WRITE_START_ADDR to FMC_WRITE_END_ADDR
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_erase_sectors(void)
{
    uint32_t erase_sector_count;

    /* unlock register FMC_CTL */
    fmc_unlock();

    /* clear all pending flags */
    fmc_all_flags_clear();
    /* erase the flash sectors */
    for(erase_sector_count = 0U; erase_sector_count < sector_cnt; erase_sector_count++) {
        fmc_sector_erase(FMC_WRITE_START_ADDR + (MAIN_FLASH_SECTOR_SIZE * erase_sector_count));
        fmc_all_flags_clear();
    }

    /* lock register FMC_CTL after the erase operation */
    fmc_lock();
}

/*!
    \brief      program fmc word by word from FMC_WRITE_START_ADDR to FMC_WRITE_END_ADDR
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_program(void)
{
    /* unlock register FMC_CTL */
    fmc_unlock();

    /* clear all pending flags */
    fmc_all_flags_clear();

    address = FMC_WRITE_START_ADDR;

    /* program flash */
    while(address < FMC_WRITE_END_ADDR) {
        fmc_word_program(address, data);
        address += 4U;
        fmc_all_flags_clear();
    }

    /* lock register FMC_CTL after the program operation */
    fmc_lock();
}

/*!
    \brief      check fmc erase result
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_erase_sectors_check(void)
{
    uint32_t i;

    ptrd = (uint32_t *)FMC_WRITE_START_ADDR;

    /* check flash whether has been erased */
    for(i = 0U; i < word_cnt; i++) {
        if(0xFFFFFFFFU != (*ptrd)) {
            led_num = LED1;
            gd_eval_led_on(led_num);
            while(1);
        } else {
            ptrd++;
        }
    }
}

/*!
    \brief      check fmc program result
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_program_check(void)
{
    uint32_t i;

    ptrd = (uint32_t *)FMC_WRITE_START_ADDR;

    /* check flash whether has been programmed */
    for(i = 0U; i < word_cnt; i++) {
        if((*ptrd) != data) {
            led_num = LED2;
            gd_eval_led_on(led_num);
            while(1);
        } else {
            ptrd++;
        }
    }
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

    /* initialize led and USART on the board */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_com_init(EVAL_COM);

    /* step1: erase pages */
    fmc_erase_sectors();

    /* step2: program and check if it is successful. If not, light the LED2. */
    fmc_program();
    SCB_CleanInvalidateDCache();
    fmc_program_check();

    /* step3: erase pages and check if it is successful. If not, light the LED1. */
    fmc_erase_sectors();
    SCB_CleanInvalidateDCache();
    fmc_erase_sectors_check();

    printf("\r\n flash program and erase successful!\r\n");

    while(1) {
    }
}
