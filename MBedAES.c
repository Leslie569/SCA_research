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

#define NOTRACES 2000
#define SBOX_SIZE 32

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "aes.h"
#include "aes.c"
#include "../../elmoasmfunctionsdef.h"


// Declare and initialize the s-box array
uint8_t sbox[SBOX_SIZE] = {
    0x04, 0x0b, 0x1f, 0x14, 0x1a, 0x15, 0x09, 0x02,
    0x1b, 0x05, 0x08, 0x12, 0x1d, 0x03, 0x06, 0x1c,
    0x1e, 0x13, 0x07, 0x0e, 0x00, 0x0d, 0x11, 0x18,
    0x10, 0x0c, 0x01, 0x19, 0x16, 0x0a, 0x0f, 0x17
};

// void ascon_sbox(const unsigned char input[5],
//                          unsigned char output[5]);
// void ascon_sbox(const unsigned char input[5],
//                          unsigned char output[5])
// {
//     unsigned int hexInputValue = 0;
//     // Convert binary input to a single number
//     for (int i = 0; i < 5; i++) {
//         hexInputValue = (hexInputValue << 1) | (input[i] & 1);
//     }

//     unsigned int hexOutputValue = sbox[hexInputValue];
//     for (int i = 0; i < 5; i++) {
//         output[i] = (hexOutputValue >> (4 - i)) & 1; // Shift and mask to get each bit
//     }
// }

unsigned char ascon_sbox(const unsigned char input);
unsigned char ascon_sbox(const unsigned char input)
{
    // unsigned int hexInputValue = 0;
    // // Convert binary input to a single number
    // for (int i = 0; i < 5; i++) {
    //     hexInputValue = (hexInputValue << 1) | (input[i] & 1);
    // }

    // unsigned char hexOutputValue = sbox[hexInputValue];
    // for (int i = 0; i < 5; i++) {
    //     output[i] = (hexOutputValue >> (4 - i)) & 1; // Shift and mask to get each bit
    // }
    unsigned char output;
    output = sbox[((uint8_t) input) & 31];

    return output;
}

int main(void) {
    int i;
    // uint8_t *input, *output;
    unsigned char input, output;
    // input = malloc(5*sizeof(uint8_t));
    // output = malloc(5*sizeof(uint8_t));

    srand(time(0));

    for(i=0;i<NOTRACES;i++){
            
        // for(j=0;j<5;j++){
        //     input[j] = rand() % 2; //Get random data for inputs.
            
        //     output[j] = 0x00;
        // }
        randbyte(&input);

        starttrigger();
        
            output = ascon_sbox(input);
        
        endtrigger();

        printbyte(&output);    
    }
    
    endprogram();

    return 0;
}

// int main(void) {
    
//     int i,j;
    
//     uint8_t *input, *output, *key;
    
//     key = malloc(16*sizeof(uint8_t));
//     input = malloc(16*sizeof(uint8_t));
//     output = malloc(16*sizeof(uint8_t));
    
//     mbedtls_aes_context *ctx;
//     ctx = malloc(sizeof(mbedtls_aes_context));
    
//     for(i=0;i<NOTRACES;i++){
        
//         // Set up inputs and key.
        
//         for(j=0;j<16;j++){

//             randbyte(&input[j]); //Get random data for inputs. Random data generated is stored in randdata.txt.
//             key[j] = j; // Initialise key to 0, 1, 2, 3,..., 15
//             output[j] = 0x00;
                
//         }

//         mbedtls_aes_setkey_enc(ctx, key, 128);
        
//         starttrigger();
        
//             mbedtls_aes_encrypt(ctx, input, output);
        
//         endtrigger();
        
//         // Print cipher texts.
        
//         for(j=0;j<16;j++){
                
//             printbyte(&output[j]);

//         }
                
//     }
    
//     endprogram();

//     return 0;
// }
