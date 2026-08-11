/*!
    \file    readme.txt
    \brief   description of the TIMER0 6-steps demo for GD32H73x_75x
    
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

  This demo is based on the GD32H759I-EVAL-V2.0 board, it shows how to configure the
TIMER0 peripheral to generate three complementary signals(for BLDC) with dead time.

  TIMER0 frequency is fixed to systemcoreclock, TIMER0 prescaler is equal to (299+1), so 
TIMER0 counter frequency is 1MHz.

  Configure the break feature, BREAK0 is active at high level, and using the automatic
output enable feature.

  Channel change sequence: AB`->AC`->BC`->BA`->CA`->CB`
    - step1:1-0  0-1  0-0  (CH0-MCH0 CH1-MCH1 CH2-MCH2)
    - step2:1-0  0-0  0-1  (CH0-MCH0 CH1-MCH1 CH2-MCH2)
    - step3:0-0  1-0  0-1  (CH0-MCH0 CH1-MCH1 CH2-MCH2)
    - step4:0-1  1-0  0-0  (CH0-MCH0 CH1-MCH1 CH2-MCH2)
    - step5:0-1  0-0  1-0  (CH0-MCH0 CH1-MCH1 CH2-MCH2)
    - step6:0-0  0-1  1-0  (CH0-MCH0 CH1-MCH1 CH2-MCH2)

  Connect the TIMER0 pins to an oscilloscope to monitor the different waveforms:
    - TIMER0_CH0  pin (PA8)
    - TIMER0_MCH0 pin (PA7)
    - TIMER0_CH1  pin (PA9)
    - TIMER0_MCH1 pin (PB0)
    - TIMER0_CH2  pin (PA10)
    - TIMER0_MCH2 pin (PB1)
