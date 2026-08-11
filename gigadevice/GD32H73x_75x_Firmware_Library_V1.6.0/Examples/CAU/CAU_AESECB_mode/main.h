/*!
    \file    main.h
    \brief   the header file of main 

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

#ifndef MAIN_H
#define MAIN_H

/* plain text size in bytes, it must be a multiple of 16(128 bits = 16 bytes) for AES algorithm */
#define DATA_SIZE             ((uint32_t)64)

__attribute__((aligned(32)))
uint8_t key_128[16] = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70
}; //(ASCII)abcdefghijklmnop

__attribute__((aligned(32)))
uint8_t plaintext[64] = {
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46,
    0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E,
    0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56,
    0x57, 0x58, 0x59, 0x5A, 0x61, 0x62, 0x63, 0x64,
    0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C,
    0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74,
    0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7A, 0x7A
};//(ASCII)0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzzz

__attribute__((aligned(32)))
uint8_t exp_ecb_128[64] = {
    0xFB, 0xA1, 0x6E, 0x97, 0x43, 0x27, 0x33, 0x25,
    0x0B, 0x9E, 0xCB, 0xC3, 0x41, 0x65, 0x0C, 0x2B,
    0x4C, 0xB0, 0xB4, 0x65, 0x01, 0xC0, 0x3E, 0x60,
    0x51, 0x7A, 0x37, 0x5E, 0x1D, 0x27, 0x12, 0x85,
    0x45, 0xEF, 0x64, 0x99, 0xB6, 0x32, 0x75, 0xCB,
    0xC6, 0x0E, 0xCE, 0x8B, 0xCD, 0x69, 0xA9, 0x9A,
    0x26, 0xBE, 0x34, 0xCA, 0xE9, 0x64, 0xDD, 0x8D,
    0x6D, 0x5B, 0x21, 0xD7, 0x51, 0xE9, 0xB0, 0xA4
};

#endif /* MAIN_H */
