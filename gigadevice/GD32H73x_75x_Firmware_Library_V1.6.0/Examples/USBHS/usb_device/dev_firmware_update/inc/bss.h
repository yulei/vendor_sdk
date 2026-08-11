/*!
    \file    bss.h
    \brief   bss functions definitions
    
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
#ifndef BSS_H
#define BSS_H
#include "inttypes.h"

typedef uint32_t BSS_ErrStatus;

#define BSS_ERROR                       ((uint32_t)0)           /*!< last operation failed */
#define BSS_DISABLED                    ((uint32_t)0xEE000091)  /*!< BSS/security not activated */
#define BSS_OK                          ((uint32_t)0xEE000092)  /*!< last operation succeded */

#define BSS_ENABLE_JTAG_AT_EXIT         ((unsigned int) 0x97A0FA17)
#define BSS_KEEP_JTAG_DISABLED_AT_EXIT 	((unsigned int) 0)
    
#define BSS_FWKEY_SIZE                  16

/* BSS version */
typedef struct
{
    uint8_t patch;                      /*!< bug fixes */
    uint8_t minor;                      /*!< backward compatible changes : public API deprecated, new API functions, improvements... */
    uint8_t major;                      /*!< backward incompatible changes : public API modification */
}BSS_Version_struct;

/* firmware key information. */
typedef struct
{
    uint8_t    iv[BSS_FWKEY_SIZE];      /*!< base initialization vector to be used with key */
    uint8_t    value[BSS_FWKEY_SIZE];   /*!< key value (max 64 bytes) */
}BSS_FWKey_t;

/* secure area parameters. */
typedef struct
{
    uint16_t scr_area_start;
    uint16_t scr_area_end;
    uint32_t scr_eren;
}BSS_SecureAcessArea_struct;

/*!
    \brief      get bss version
    \param[in]  none
    \param[out] none
    \retval     BSS_Version_struct: version of bss
*/
BSS_Version_struct BSS_get_version(void);

/*!
    \brief      get bss status
    \param[in]  none
    \param[out] none
    \retval     BSS_ErrStatus: BSS_OK or BSS_ERROR or BSS_DISABLED
*/
BSS_ErrStatus BSS_get_status(void);

/*!
    \brief      get bss certificate
    \param[in]  none
    \param[out] none
    \retval     pointer of bss certificate
*/
uint8_t* BSS_get_certificate(void);

/*!
    \brief      get bss certificate
    \param[in]  none
    \param[out] none
    \retval     size of bss certificate
*/
uint32_t BSS_get_certificate_size(void);
    
/*!
    \brief      reset and setup secure area(s)
    \param[in]  areas: 
                  sizeInBytes: size of secure-access area in bytes
                  startAddress: start address of secure-access area
                  removeDuringMassErase: if keep secure-access area during mass erase. 
    \param[out] none
    \retval     none
*/
void BSS_initialize_secure_access_areas(BSS_SecureAcessArea_struct* areas);

/*!
    \brief      securely exit from secure-access area
    \param[in]  vectors: target interrupt vector table address
                jtagState: if keep jtag disabled or enabled after exit secure-access area
                  BSS_ENABLE_JTAG_AT_EXIT: enable jtag after exit
                  BSS_KEEP_JTAG_DISABLED_AT_EXIT: jtag keep disable after exit
    \param[out] none
    \retval     none
*/
void BSS_exit_secure_access_area(unsigned int vectors, unsigned int jtagState);

typedef struct
{
    BSS_Version_struct  (*getVersion)(void);
    BSS_ErrStatus       (*getStatus)(void);
    uint8_t*            (*getCertificate)(void);
    uint32_t            (*getCertificateSize)(void);
    uint32_t            reserved;
    void                (*exitSecureArea)(unsigned int vectors, unsigned int jtagState);
    void                (*resetAndInitializeSecureAreas)(BSS_SecureAcessArea_struct* areas);
}BSS_API_Table_t;

/* bss api function table address */
#define BSS ((BSS_API_Table_t*)0x1FF0FC00)

#endif