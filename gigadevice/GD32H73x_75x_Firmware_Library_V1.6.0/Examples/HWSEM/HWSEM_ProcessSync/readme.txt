/*!
    \file    readme.txt
    \brief   description of HWSEM Process Sync example
    
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

  This example is based on the GD32H759I-EVAL-V2.0 board, it provides a description of 
HESEM semaphore lock and release.
  In this demo, the semaphore 1 equals to the resource LED1.
  If semaphore 1 is successfully locked by process_1, LED1 will be on, and in the later
5 seconds, the semaphore 1 is own by process_1, if WAKEUP key is pressed in this period to 
try to lock the semaphore 1 for process_2, it will be failed, but the release notice will
be set if process_1 release semaphore 1. 
  5 seconds later, semaphore 1 is released by process_1, and if release notice is set, 
LED2 will be lighted for 0.5 seconds. Users can try to lock semapgore 1 by press WAKEUP key
for process_2 within later 5 seconds. If lock successfully, LED1 will be turned off,  only when
users press WAKEUP key again to release the semaphore 1, the semaphore 1 can be locked again by
process_1. 
  Repeat the operations as described.
