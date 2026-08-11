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

#define DATA_SIZE             64U

/* key : 192 bits */
__attribute__((aligned(32)))
uint8_t tdes_key[24] = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70,
    0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78
};//(ASCII)abcdefghijklmnopqrstuvwx

/* plaintext */
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
const uint8_t exp_tdes_ecb[64] = {
    0x96, 0xBD, 0x1A, 0x9E, 0x8D, 0x93, 0xA6, 0xF8,
    0xC1, 0x21, 0xA5, 0x4D, 0x79, 0x8C, 0x5C, 0x64,
    0x69, 0x0F, 0x61, 0x68, 0xD5, 0x5C, 0x4F, 0xD2,
    0x9E, 0x54, 0x99, 0x8A, 0x11, 0xF6, 0xA7, 0x2F,
    0x59, 0x6B, 0xE1, 0x38, 0x60, 0x60, 0xCD, 0xAB,
    0x7A, 0x35, 0x11, 0xE4, 0xCA, 0x2B, 0x19, 0xFA,
    0x26, 0xF0, 0xCF, 0x79, 0x46, 0xC7, 0x1F, 0xF1,
    0xA3, 0x83, 0x15, 0x6C, 0x80, 0x90, 0xC8, 0xB2
};

#endif /* MAIN_H */
