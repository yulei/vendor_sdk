/*!
    \file    readme.txt
    \brief   description of the master and slave fullduplex communication use polling demo

    \version 2023-12-31, V1.4.0, firmware for GD32E50x
*/

/*
    Copyright (c) 2023, GigaDevice Semiconductor Inc.

    All rights reserved.

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

  This example is based on the GD32E507Z-EVAL-V1.0 board, it shows SPI0 and SPI2 fullduplex
communication use polling mode.After the communicate is complete,if receive data equal 
to send data, led2 and led3 turn on. If not, led2 and led3 turn off.

  1.Modify file: Examples/SPI/SPI_master_slave_fullduplex_polling/main.c, choose to disable
CRC function.
  #define SPI_CRC_ENABLE       0
  After the communication is completed, if the data received equal to the data sent, LED1
and LED2 turn on, and if not, LED3 and LED2 turn off.

  2.Modify file: Examples/SPI/SPI_master_slave_fullduplex_polling/main.c, choose to enable
CRC function.
  #define SPI_CRC_ENABLE       1
  After the communication is completed, if the CRC checking is passed, LED1 and LED2 turn
on, and if not, LED3 and LED2 turn off.

  Connect SPI0 NSS  PIN(PA3) to SPI2 NSS  PIN(PA4).
  Connect SPI0 SCK  PIN(PA5) to SPI2 SCK  PIN(PC10).
  Connect SPI0 MISO PIN(PA6) to SPI2 MISO PIN(PC11).
  Connect SPI0 MOSI PIN(PA7) to SPI2 MOSI PIN(PC12).