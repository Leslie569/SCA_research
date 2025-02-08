#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define NOTRACES 10000
#define SBOX_SIZE 32

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

void readhex(uint8_t *byte, FILE *file) {
    char buffer[3]; // To hold the hex value as a string
    if (fscanf(file, "%2s", buffer) != 1) { // Read two characters
        fprintf(stderr, "Failed to read hex value or reached EOF\n");
        exit(EXIT_FAILURE);
    }
    *byte = (uint8_t) strtol(buffer, NULL, 16); // Convert hex string to uint8_t
}

void printhex(const uint8_t *bytes, int length, FILE *outputFile) {
    // Write 8 bytes per line to the file
    for (int i = 0; i < length; i++) {
        fprintf(outputFile, "%02x ", bytes[i]); // Write each byte in hexadecimal format
        if ((i + 1) % 8 == 0) { // Check if we've written 8 bytes
            fprintf(outputFile, "\n"); // New line after 8 bytes
        }
    }
    if (length % 8 != 0) { // If the total length is not a multiple of 8, write a new line
        fprintf(outputFile, "\n");
    }
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

    // Open input file
    FILE *file = fopen("ELMO/output/randdata.txt", "r");
    if (file == NULL) {
        perror("Failed to open randdata.txt");
        free(k0);
        return EXIT_FAILURE;
    }

    int i;
    for(i=0; i<8; i++)
    {
      readhex(&k0[i], file);
      readhex(&k1[i], file);
    }
    
    const uint64_t K0 = LOADBYTES(k0, 8);
    const uint64_t K1 = LOADBYTES(k1, 8);

    // Open output file
    FILE *outputFile = fopen("ELMO/output/ref_output.txt", "w");
    if (outputFile == NULL) {
        perror("Failed to open ref_output.txt");
        fclose(file);
        return EXIT_FAILURE;
    }

    for(i=0;i<NOTRACES;i++){
        uint8_t *n0, *n1;
        n0 = malloc(16*sizeof(uint8_t));
        n1 = malloc(16*sizeof(uint8_t));
        int j;
        for(j=0; j<8; j++)
        {
            readhex(&n0[j], file);
            readhex(&n1[j], file);
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
      
      
        ROUND(&s, 0xf0);


        for (j=0; j<5; j++)
        {
            unsigned char buffer[8];
            for (int k = 0; k < 8; k++) {
                buffer[k] = (unsigned char)(s.x[j] >> (k * 8));
            }
            printhex(buffer, 8, outputFile); 
        }

        free(n0);
        free(n1);
    }

    fclose(file);
    fclose(outputFile);
    free(k0);
    free(k1);
    // return 0;
}
