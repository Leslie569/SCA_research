/*
 * University of Bristol – Open Access Software Licence
 * Copyright (c) 2016, The University of Bristol, a chartered
 * corporation having Royal Charter number RC000648 and a charity
 * (number X1121) and its place of administration being at Senate
 * House, Tyndall Avenue, Bristol, BS8 1TH, United Kingdom.
 * All rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided
 * with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Any use of the software for scientific publications or commercial
 * purposes should be reported to the University of Bristol
 * (OSI-notifications@bristol.ac.uk and quote reference 2668). This is
 * for impact and usage monitoring purposes only.
 *
 * Enquiries about further applications and development opportunities
 * are welcome. Please contact elisabeth.oswald@bristol.ac.uk
 */

#define NOTRACES 20000

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "AES.h"
#include "../../elmoasmfunctionsdef.h"

extern void acComputeAES(uint8_t *output, uint8_t *input, uint8_t *key);
extern void acComputeAESMasked(uint8_t *output, uint8_t *input, uint8_t *key);

int main(void) {

    int i,j;

    uint8_t *input, *output, *key;

    key = malloc(16*sizeof(uint8_t));
    input = malloc(16*sizeof(uint8_t));
    output = malloc(16*sizeof(uint8_t));
    
    static const uint8_t fixedkey[16] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0};
    static const uint8_t fixedinput[16] = {0xda, 0x39, 0xa3, 0xee, 0x5e, 0x6b, 0x4b, 0x0d, 0x32, 0x55, 0xbf, 0xef, 0x95, 0x60, 0x18, 0x90};
    
    U = 0;
    V = 0;
    UV = 0;
    
    for(j=0;j<16;j++){
        output[j] = 0x00;
    }

// Generate fixed traces
    for(i=0;i<NOTRACES;i++){

        readbyte(&U);
        readbyte(&V);
        
        for(j=0;j<16;j++){
            readbyte(&input[j]);
            input[j] = fixedinput[j];
            key[j] = fixedkey[j];
        }
        
        setmaskflowstart(0);
        initialisemaskflow(&U);
        
        setmaskflowstart(8);
        initialisemaskflow(&V);
        
        //Trigger started in asm file
        acComputeAESMasked(output, input, key);
    }
    
    resetdatafile();

    for(j=0;j<16;j++){
        output[j] = 0x00;
    }
    
// Generate random traces
    
    for(i=0;i<NOTRACES;i++){
        
        readbyte(&U);
        readbyte(&V);
        
        for(j=0;j<16;j++){
            readbyte(&input[j]);
            key[j] = fixedkey[j];
        }
        
        setmaskflowstart(0);
        initialisemaskflow(&U);
        
        setmaskflowstart(8);
        initialisemaskflow(&V);
        
        //Trigger started in asm file
        acComputeAESMasked(output, input, key);
    }
    
    endprogram();

    return 0;
}
