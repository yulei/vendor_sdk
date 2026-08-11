/*!
    \file    gd32h73x_75x_xM_flash.ld
    \brief   linker script file

    \version 2026-03-24, V1.6.0, firmware for GD32H73x_75x
*/

/*
 * Copyright (c) 2009-2020 Arm Limited. All rights reserved.
 * Copyright (c) 2026, GigaDevice Semiconductor Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/* This file refers the CMSIS standard, some adjustments are made according to GigaDevice chips */

/* memory map */
MEMORY
{
  FLASH    (rx): ORIGIN = 0x08000000, LENGTH = 3840K
  AXISRAM  (xrw): ORIGIN = 0x24000000, LENGTH = 832K
  ITCMRAM  (xrw): ORIGIN = 0x00000000, LENGTH = 64K
  DTCMRAM  (xrw): ORIGIN = 0x20000000, LENGTH = 128K
  SRAM0    (xrw): ORIGIN = 0x30000000, LENGTH = 16K
  SRAM1    (xrw): ORIGIN = 0x30004000, LENGTH = 16K
}

ENTRY(Reset_Handler)

SECTIONS
{
  __stack_size = DEFINED(__stack_size) ? __stack_size : 2K;
  __heap_size = DEFINED(__heap_size) ? __heap_size : 1K;

  /* ISR vectors */
  .vectors :
  {
    . = ALIGN(4);
    KEEP(*(.vectors))
    . = ALIGN(4);
    __Vectors_End = .;
    __Vectors_Size = __Vectors_End - __gVectors;
  } >FLASH

  /* text section, such as program code */
  .text :
  {
    . = ALIGN(4);
    *(.text)
    *(.text*)
    *(.glue_7)
    *(.glue_7t)
    *(.eh_frame)
    KEEP (*(.init))
    KEEP (*(.fini))
    . = ALIGN(4);
    /* the symbol '_etext' will be defined at the end of code section */
    _etext = .;
  } >FLASH

  /* ro-data section, such as constant data */
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)
    *(.rodata*)
    . = ALIGN(4);
  } >FLASH

  /* exception process table */
  .ARM.extab :
  {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
  } >FLASH

  /* exception process table index */
  .ARM :
  {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } >FLASH

  /* ARM attributes */
  .ARM.attributes :
  {
   *(.ARM.attributes)
  } >FLASH

  .preinit_array :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >FLASH

  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >FLASH

  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(.fini_array*))
    KEEP (*(SORT(.fini_array.*)))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >FLASH

  /* provide some necessary symbols for initialized data */
  _sidata = LOADADDR(.data);
  .data :
  {
    . = ALIGN(4);
    /* the symbol '_sdata' will be defined at the data section end start */
    _sdata = .;
    *(.data)
    *(.data*)
    . = ALIGN(4);
    /* the symbol '_edata' will be defined at the data section end */
    _edata = .;
  } > AXISRAM AT> FLASH

  /* provide some necessary symbols for uninitialized data */
  . = ALIGN(4);
  .bss :
  {
    . = ALIGN(4);
    /* the symbol '_sbss' will be defined at the bss section start */
    _sbss = .;
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    /* the symbol '_ebss' will be defined at the bss section end */
    _ebss = .;
    __bss_end__ = _ebss;
  } > AXISRAM

  /* heap and stack space */
  .heap_stack :
  {
    . = ALIGN(8);
    PROVIDE ( end = _ebss );
    PROVIDE ( _end = _ebss );
    . = . + __heap_size;
    PROVIDE( _heap_end = . );
    . = . + __stack_size;
    PROVIDE( _sp = . );
    . = ALIGN(8);
  } > AXISRAM

  _siitcmram = LOADADDR(.itcmram);
  .itcmram :
  {
    . = ALIGN(4);
    _sitcmram = .;
    *(.itcmram)
    *(.itcmram*)
    . = ALIGN(4);
    _eitcmram = .;
  } > ITCMRAM AT> FLASH

  _sidtcmram = LOADADDR(.dtcmram);
  .dtcmram :
  {
    . = ALIGN(4);
    _sdtcmram = .;
    *(.dtcmram)
    *(.dtcmram*)
    . = ALIGN(4);
    _edtcmram = .;
  } > DTCMRAM AT> FLASH

  _sisram0 = LOADADDR(.sram0);
  .sram0 :
  {
    . = ALIGN(4);
    _ssram0 = .;
    *(.sram0)
    *(.sram0*)
    . = ALIGN(4);
    _esram0 = .;
  } > SRAM0 AT> FLASH

  _sisram1 = LOADADDR(.sram1);
  .sram1 :
  {
    . = ALIGN(4);
    _ssram1 = .;
    *(.sram1)
    *(.sram1*)
    . = ALIGN(4);
    _esram1 = .;
  } > SRAM1 AT> FLASH
}

/* input sections */
GROUP(libgcc.a libc.a libm.a libnosys.a)
