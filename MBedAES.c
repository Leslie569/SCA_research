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

#define NOTRACES 10000
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

void print_state(const ascon_state_t* s);

void print_state(const ascon_state_t* s) {
  printf("State:\n");
  for (int i = 0; i < 5; i++) {
      printf("  x[%d]: 0x%016llx\n", i, s->x[i]);
  }
}

static inline void ROUND(ascon_state_t* s, uint8_t C) {
  ascon_state_t t;

  // printf("Before ROUND constant addition:\n");
  // print_state(s);

  /* addition of round constant */
  s->x[2] ^= C;

  // printf("After ROUND constant addition:\n");
  // print_state(s);

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

  // printf("After ROUND:\n");
  // print_state(s);
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
      // printf("k0[%d]: 0x%02x\n", i, k0[i]);
      randbyte(&k1[i]);
      // printf("k1[%d]: 0x%02x\n", i, k1[i]);
    }
     /* load key and nonce */
    const uint64_t K0 = LOADBYTES(k0, 8);
    const uint64_t K1 = LOADBYTES(k1, 8);

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
      const uint64_t N0 = LOADBYTES(n0, 8);
      const uint64_t N1 = LOADBYTES(n1, 8);
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

      // printf("Final state after round %d:\n", i);
      // print_state(&s);

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


    endprogram();
    free(k0);
    free(k1);
    return 0;
}