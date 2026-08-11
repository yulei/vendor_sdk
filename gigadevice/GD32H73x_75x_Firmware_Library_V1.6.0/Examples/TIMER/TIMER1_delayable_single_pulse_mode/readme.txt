/*!
    \file    readme.txt
    \brief   description of the TIMER1 output delayable single pulse demo for GD32H73x_75x
    
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
TIMER1 peripheral in delayable single pulse mode.

  The external signal is connected to TIMER1 CH0 pin (PA5) and the rising edge reset
and start counter. The delayable single pulse waveform is output on TIMER1 CH1 pin(PA1).

  The TIMER1 frequency is set to 300MHz, the prescaler is 30000, so TIMER1 counter 
frequency is 10KHz. The delayable single pulse waveform is output on TIMER1 CH1 pin(PA1), 
the delayable single pulse width is determined by the TIMERx_CAR register, and width is
10000/10KHz = 1s.

  Before the delayable single pulse ends, the external trigger signal arrives again, and 
the pulse width will be extended.

  Connect the TIMER1 CH0 pin (PA5) to TAMPER KEY(PC13), When TAMPER KEY is pressed, a 
external signal is generated.
