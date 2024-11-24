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

#define NOTRACES 1000
#define SBOX_SIZE 32

#include <stdio.h>
#include <stdlib.h>

#include "aes.h"
#include "aes.c"
#include "../../elmoasmfunctionsdef.h"


#define ASCON_128_KEYBYTES 16
#define ASCON_128_RATE 8
#define ASCON_128_PA_ROUNDS 12
#define ASCON_128_PB_ROUNDS 6

#define ASCON_128_IV                            \
  (((uint64_t)(ASCON_128_KEYBYTES * 8) << 56) | \
   ((uint64_t)(ASCON_128_RATE * 8) << 48) |     \
   ((uint64_t)(ASCON_128_PA_ROUNDS) << 40) |    \
   ((uint64_t)(ASCON_128_PB_ROUNDS) << 32))

typedef struct {
  uint64_t x[5];
} ascon_state_t;

static inline uint64_t ROR(uint64_t x, int n) {
  return x >> n | x << (-n & 63);
}

static inline void ROUND(ascon_state_t* s, uint8_t C) {
  ascon_state_t t;
  /* addition of round constant */
  s->x[2] ^= C;
  /* printstate(" round constant", s); */
  /* substitution layer */
  s->x[0] ^= s->x[4];
  s->x[4] ^= s->x[3];
  s->x[2] ^= s->x[1];
  /* start of keccak s-box */
  t.x[0] = s->x[0] ^ (~s->x[1] & s->x[2]);
  t.x[1] = s->x[1] ^ (~s->x[2] & s->x[3]);
  t.x[2] = s->x[2] ^ (~s->x[3] & s->x[4]);
  t.x[3] = s->x[3] ^ (~s->x[4] & s->x[0]);
  t.x[4] = s->x[4] ^ (~s->x[0] & s->x[1]);
  /* end of keccak s-box */
  t.x[1] ^= t.x[0];
  t.x[0] ^= t.x[4];
  t.x[3] ^= t.x[2];
  t.x[2] = ~t.x[2];
  /* printstate(" substitution layer", &t); */
  /* linear diffusion layer */
  s->x[0] = t.x[0] ^ ROR(t.x[0], 19) ^ ROR(t.x[0], 28);
  s->x[1] = t.x[1] ^ ROR(t.x[1], 61) ^ ROR(t.x[1], 39);
  s->x[2] = t.x[2] ^ ROR(t.x[2], 1) ^ ROR(t.x[2], 6);
  s->x[3] = t.x[3] ^ ROR(t.x[3], 10) ^ ROR(t.x[3], 17);
  s->x[4] = t.x[4] ^ ROR(t.x[4], 7) ^ ROR(t.x[4], 41);
//   printstate(" round output", s);
}

/* set byte in 64-bit Ascon word */
#define SETBYTE(b, i) ((uint64_t)(b) << (56 - 8 * (i)))

/* load bytes into 64-bit Ascon word */
static inline uint64_t LOADBYTES(const uint8_t* bytes, int n) {
  int i;
  uint64_t x = 0;
  for (i = 0; i < n; ++i) x |= SETBYTE(bytes[i], i);
  return x;
}


int main(void) {
    uint8_t *k0, *k1;
    k0 = malloc(16*sizeof(uint8_t));
    k1 = malloc(16*sizeof(uint8_t));
    int i;
    for(i=0; i<8; i++)
    {
      randbyte(&k0[i]);
      randbyte(&k1[i]);
    }
     /* load key and nonce */
    const uint64_t K0 = *k0;
    const uint64_t K1 = *k1;

    for(i=0;i<NOTRACES;i++){
      uint8_t *n0, *n1;
      n0 = malloc(16*sizeof(uint8_t));
      n1 = malloc(16*sizeof(uint8_t));
      int j;
      for(j=0; j<8; j++)
      {
        randbyte(&n0[j]);
        randbyte(&n1[j]);
      }
      const uint64_t N0 = *n0;
      const uint64_t N1 = *n1;
      /* initialize */
      ascon_state_t s;
      s.x[0] = ASCON_128_IV;
      s.x[1] = K0;
      s.x[2] = K1;
      s.x[3] = N0;
      s.x[4] = N1;
      
      starttrigger();
      
        ROUND(&s, 0xf0);

      endtrigger();

      for (j=0; j<5; j++)
      {
        unsigned char buffer[8];
        for (int k = 0; k < 8; k++) {
          buffer[k] = (unsigned char)(s.x[j] >> (k * 8));
          printbyte(&buffer[k]);   
        }
      }
      free(n0);
      free(n1);
    }

    // for(i=0;i<NOTRACES;i++){
    //   uint8_t *n0, *n1;
    //   n0 = malloc(16*sizeof(uint8_t));
    //   n1 = malloc(16*sizeof(uint8_t));
    //   int j;
    //   for(j=0; j<8; j++)
    //   {
    //     randbyte(&n0[j]);
    //     randbyte(&n1[j]);
    //   }
    //   const uint64_t N0 = *n0;
    //   const uint64_t N1 = *n1;
    //   /* initialize */
    //   ascon_state_t s;
    //   s.x[0] = ASCON_128_IV;
    //   s.x[1] = K0;
    //   s.x[2] = K1;
    //   s.x[3] = N0;
    //   s.x[4] = N1;
      
    //   starttrigger();
      
    //     ROUND(&s, 0xf0);

    //   endtrigger();

    //   for (j=0; j<5; j++)
    //   {
    //     unsigned char buffer[8];
    //     for (int k = 0; k < 8; k++) {
    //       buffer[k] = (unsigned char)(s.x[j] >> (k * 8));
    //       printbyte(&buffer[k]);   
    //     }
    //   }
    //   free(n0);
    //   free(n1);
    // }


    endprogram();
    free(k0);
    free(k1);
    return 0;
}

// // Declare and initialize the s-box array
// uint8_t sbox[SBOX_SIZE] = {
//     0x04, 0x0b, 0x1f, 0x14, 0x1a, 0x15, 0x09, 0x02,
//     0x1b, 0x05, 0x08, 0x12, 0x1d, 0x03, 0x06, 0x1c,
//     0x1e, 0x13, 0x07, 0x0e, 0x00, 0x0d, 0x11, 0x18,
//     0x10, 0x0c, 0x01, 0x19, 0x16, 0x0a, 0x0f, 0x17
// };

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

// int main(void) {
//     int i,j;
//     uint8_t *input, *output;
//     input = malloc(5*sizeof(uint8_t));
//     output = malloc(5*sizeof(uint8_t));
    
//     srand(time(0));

//     for(i=0;i<NOTRACES;i++){
            
//         for(j=0;j<5;j++){
//             input[j] = rand() % 2; //Get random data for inputs.
//             output[j] = 0x00;
//         }
        
//         starttrigger();
        
//             ascon_sbox(input, output);
        
//         endtrigger();
                
//     }
    
//     endprogram();

//     return 0;
// }

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
