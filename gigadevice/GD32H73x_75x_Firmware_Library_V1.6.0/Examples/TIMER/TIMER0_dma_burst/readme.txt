/*!
    \file    readme.txt
    \brief   description of the TIMER0 dma burst demo for GD32H73x_75x

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

  This demo is based on the GD32H759I-EVAL-V2.0 board, it shows how to use DMA with 
TIMER0 update request to transfer data from memory to TIMER0 capture compare register
0~3.

  TIMER0 frequency is fixed to 300MHz, TIMER0 prescaler is equal to 300, so TIMER0 
counter frequency is 1MHz.

  The objective is to configure TIMER0 channel 0~3(PA8/PA9/PA10/PC7) to generate PWM 
signals. Capture compare register 0~3 are to be updated twice per circle.

  On the first update DMA request, data1 is transferred to CH0CV, data2 is transferred
to CH1CV, data3 is transferred to CH2CV,data4 is transferred to CH3CV and duty cycles
are (10%, 20%, 30%, 40%).

  On the second update DMA request, data5 is transferred to CH0CV, data6 is transferred
to CH1CV, data7 is transferred to CH2CV, data8 is transferred to CH3CV and duty cycles
are (50%, 60%, 70%, 80%).
