/******************************************************************************
  Filename:       crypto.c
  Description:    This file contains the implementation of crypto APIs.

*******************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "mible_crypto.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */
struct crypt_state {
    uint8_t perm[256];
    uint8_t index1;
    uint8_t index2;
};
static struct crypt_state state;

static void swap_bytes(uint8_t* a, uint8_t* b)
{
    uint8_t temp;

    temp = *a;
    *a = *b;
    *b = temp;
}

/*
 * Initialize an RC4 state buffer using the supplied key,
 * which can have arbitrary length.
 */
static void crypt_init(struct crypt_state* const state, const uint8_t* key,
    uint16_t keylen)
{
    uint8_t j;
    uint16_t i;

    /* Initialize state with identity permutation */
    for (i = 0; i <= 255; i++)
        state->perm[i] = i;
    state->index1 = 0;
    state->index2 = 0;

    /* Randomize the permutation using key data */
    for (j = i = 0; i <= 255; i++) {
        j += state->perm[i] + key[i % keylen];
        swap_bytes(&state->perm[i], &state->perm[j]);
    }
}

/*
 * Encrypt some data using the supplied RC4 state buffer.
 * The input and output buffers may be the same buffer.
 * Since RC4 is a stream cypher, this function is used
 * for both encryption and decryption.
 */
static void crypt(struct crypt_state* const state, uint8_t* inbuf,
    uint8_t* outbuf, uint16_t buflen)
{
    uint16_t i;
    uint8_t j;

    for (i = 0; i < buflen; i++) {

        /* Update modification indicies */
        state->index1++;
        state->index2 += state->perm[state->index1];

        /* Modify permutation */
        swap_bytes(&state->perm[state->index1], &state->perm[state->index2]);

        /* Encrypt/decrypt next byte */
        j = state->perm[state->index1] + state->perm[state->index2];
        outbuf[i] = inbuf[i] ^ state->perm[j];
    }
}

#define TOKEN_LEN 12

/*
 * miio_ble_mix_1 - Algorithm to generate t1 from token.
 *             t1 is used in key exchange.
 *
 * @param[in]   in   - The input token
 * @param[in]   mmac - Master public address
 * @param[in]   smac - Slave public address
 * @param[in]   pid  - Product ID
 * @param[out]  out  - The generated output token
 */
void mible_mix_1(uint8_t* in, uint8_t* mmac, uint8_t* smac, uint8_t* pid,
    uint8_t* out)
{

    uint8_t key[8];
    key[0] = smac[0];
    key[1] = smac[2];
    key[2] = smac[5];
    key[3] = pid[0];
    key[4] = pid[0];
    key[5] = smac[4];
    key[6] = smac[5];
    key[7] = smac[1];

    /* RC4 encrypt */
    crypt_init(&state, key, 8);
    crypt(&state, in, out, TOKEN_LEN);
}

/*
 * miio_ble_mix_2 - Algorithm to generate t2 from t1.
 *             t2 is used in key exchange.
 *
 * @param[in]   in   - The input token
 * @param[in]   mmac - Master public address
 * @param[in]   smac - Slave public address
 * @param[in]   pid  - Product ID
 * @param[out]  out  - The generated output token
 */
void mible_mix_2(uint8_t* in, uint8_t* mmac, uint8_t* smac, uint8_t* pid,
    uint8_t* out)
{

    uint8_t key[8];
    key[0] = smac[0];
    key[1] = smac[2];
    key[2] = smac[5];
    key[3] = pid[1];
    key[4] = smac[4];
    key[5] = smac[0];
    key[6] = smac[5];
    key[7] = pid[0];

    /* RC4 encrypt */
    crypt_init(&state, key, 8);
    crypt(&state, in, out, TOKEN_LEN);
}

/*
 * miio_ble_encrypt - Encryption API in mi service
 *
 * @param[in]   in     - The input plain text
 * @param[in]   inLen  - Length of the input text
 * @param[in]   key    - Key used to encrypt
 * @param[in]   keyLen - Length of the key
 * @param[out]  out    - The output cipher text
 */
void mible_internal_encrypt(uint8_t* in, uint16_t inLen, uint8_t* key,
    uint16_t keyLen, uint8_t* out)
{

    /* RC4 encrypt */
    crypt_init(&state, key, keyLen);
    crypt(&state, in, out, inLen);
}
