/*!
    \file    main.c
    \brief   RAMECCMU single ECC error handle demo

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
#include "main.h"

/* AXI_SRAM address range: 0x24000000 ~ 0x2407FFFF */
#define AXI_SRAM_BEGIN_ADDRESS                 ((uint32_t)0x24000000U)
#define AXI_SRAM_END_ADDRESS                   ((uint32_t)0x2407FFFFU)

#define AXI_SRAM_SIZE                          (AXI_SRAM_END_ADDRESS - AXI_SRAM_BEGIN_ADDRESS + 1U)

__IO uint32_t ecc_fail_address, ecc_fail_data_msb, ecc_fail_data_lsb;
__IO uint64_t data, tmp;

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* initialize AXI_SRAM after power on */
    for(i = 0U; i <= AXI_SRAM_SIZE; i += 8U) {
        *(__IO uint64_t *)(AXI_SRAM_BEGIN_ADDRESS + i) = 0U;
    }

    nvic_irq_enable(RAMECCMU_IRQn, 0, 0);
    /* clear AXI_SRAM ECC status register flags */
    rameccmu_deinit(RAMECCMU0);
    single_eccerr_corret_flag = 0U;
    single_eccerr_occur_times = 0U;

    /* enable ECC error latching function for RAMECCMU0 monitor0 */
    rameccmu_monitor_interrupt_enable(RAMECCMU0_MONITOR0, RAMECCMU_INT_ECC_ERROR_LATCHING);
    /* enable ECC single error interrupt for RAMECCMU0 monitor0 */
    rameccmu_monitor_interrupt_enable(RAMECCMU0_MONITOR0, RAMECCMU_INT_ECC_SINGLE_ERROR);
    /* enable global ECC single error interrupt for RAMECCMU0 */
    rameccmu_global_interrupt_enable(RAMECCMU0, RAMECCMU_INT_ECC_SINGLE_ERROR);

    /* write AXI_SRAM with a certain pattern */
    for(i = 0U; i <= AXI_SRAM_SIZE; i += 8U) {
        *(__IO uint64_t *)(AXI_SRAM_BEGIN_ADDRESS + i) = 0xAAAAAAAAAAAAAAAAU;
    }

    /* read AXI_SRAM and check ECC status */
    for(i = 0U; i <= AXI_SRAM_SIZE; i += 8U) {
        data = REG64(AXI_SRAM_BEGIN_ADDRESS + i);
        /* single ECC error has been detected */
        while(1U == single_eccerr_corret_flag) {
            /* get ECC failing address */
            ecc_fail_address = rameccmu_monitor_failing_address_get(RAMECCMU0_MONITOR0);
            /* get ECC failing data low 32 bits */
            ecc_fail_data_lsb = rameccmu_monitor_failing_data_low_bits_get(RAMECCMU0_MONITOR0);
            /* get ECC failing data high 32 bits */
            ecc_fail_data_msb = rameccmu_monitor_failing_data_high_bits_get(RAMECCMU0_MONITOR0);
            /* write the corrected data back to the ECC failed address */
            *(__IO uint64_t *)(ecc_fail_address) = 0xAAAAAAAAAAAAAAAAU;
            single_eccerr_corret_flag = 0U;
            single_eccerr_occur_times++;
            /* read the ECC fail address again to confirm error data has been corrected */
            tmp = REG64(ecc_fail_address);
        }
    }

    while(1) {
    }
}
