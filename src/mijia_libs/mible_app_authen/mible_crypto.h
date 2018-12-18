#ifndef MIBLE_CRYPTO_H__
#define MIBLE_CRYPTO_H__

#include "mible_port.h"
/*
 * miio_ble_decrypt - Encryption API in mi service
 *
 * @param[in]   in     - The input cipher text
 * @param[in]   inLen  - Length of the input text
 * @param[in]   key    - Key used to encrypt
 * @param[in]   keyLen - Length of the key
 * @param[out]  out    - The output plain text
 */
#define mible_internal_decrypt mible_internal_encrypt
void mible_mix_1(uint8_t* in, uint8_t* mmac, uint8_t* smac, uint8_t* pid,
    uint8_t* out);
void mible_mix_2(uint8_t* in, uint8_t* mmac, uint8_t* smac, uint8_t* pid,
    uint8_t* out);
void mible_internal_encrypt(uint8_t* in, uint16_t inLen, uint8_t* key,
    uint16_t keyLen, uint8_t* out);

#endif
