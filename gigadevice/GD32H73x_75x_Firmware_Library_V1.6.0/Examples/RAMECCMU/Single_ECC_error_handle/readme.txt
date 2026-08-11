/*!
    \file    readme.txt
    \brief   description of RAMECCMU example

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

  This example is based on the GD32H759I-EVAL-V2.0 board, it shows how to use RAMECCMU 
to deal with single ECC error in AXI_SRAM. 
  
  The main steps are shown as below: 
  (1) AXI_SRAM is memset to all zeros.
  (2) AXI_SRAM's corresponding RAMECCMU peripheral RAMECCMU0 is deinitialized to reset status.
  (3) Enable ECC error latching and ECC single error interrupt for RAMECCMU0 Monitor0,
      enable global ECC single error interrupt.
  (4) Write AXI_SRAM in double word unit with a certain data pattern '0xAAAAAAAAAAAAAAAA'.
  (5) Read AXI_SRAM in double word unit and check ECC status.
  (6) If a single ECC error has been monitored, it will trigger an ECC single error interrupt
      and 'single_ecc_error_corret_flag' will be set to '1' in 'ECC_IRQHandler' interrupt service
      routine, user can get ECC failing address and ECC failing data by reading RAMECCMU0 Monitor0
      register as well.
  (7) If none single ECC error monitored, the value of 'single_ecc_error_occur_times' will be zero.
