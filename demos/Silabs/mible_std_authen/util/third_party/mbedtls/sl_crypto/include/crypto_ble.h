/*
 *  BLE-specific cipher implementations optimized for Silicon Labs devices
 *  with a CRYPTO peripheral.
 *
 *  Copyright (C) 2017, Silicon Labs, http://www.silabs.com
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#ifndef CRYPTO_BLE_H
#define CRYPTO_BLE_H

/***************************************************************************//**
 * \addtogroup sl_crypto
 * \{
 ******************************************************************************/

/***************************************************************************//**
 * \addtogroup sl_crypto_ble BLE-specific CRYPTO
 * \brief BLE-tailored cryptographic primitives.
 * \{
 ******************************************************************************/

#include <stddef.h>
#include <stdint.h>
#include "mbedtls/ccm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          CCM buffer encryption optimized for BLE
 *
 * \param data     Input/output buffer of payload data of BLE packet
 * \param length   length of input data
 * \param iv       nonce (initialization vector)
 *                 must be 13 bytes
 * \param header   header of BLE packet (1 byte)
 * \param tag      buffer where the BLE packet tag (4 bytes) will be written
 *
 * \return         0 if successful
 */
int mbedtls_ccm_encrypt_and_tag_ble( unsigned char       *data,
                                     size_t               length,
                                     const unsigned char *key,
                                     const unsigned char *iv,
                                     unsigned char        header,
                                     unsigned char       *tag );

/**
 * \brief          CCM buffer authenticated decryption optimized for BLE
 *
 * \param data     Input/output buffer of payload data of BLE packet
 * \param length   length of input data
 * \param iv       nonce (initialization vector)
 *                 must be 13 bytes
 * \param header   header of BLE packet (1 byte)
 * \param tag      authentication tag of BLE packet (4 bytes)
 *
 * \return         0 if successful and authenticated,
 *                 MBEDTLS_ERR_CCM_AUTH_FAILED if tag does not match payload
 */
int mbedtls_ccm_auth_decrypt_ble( unsigned char       *data,
                                  size_t               length,
                                  const unsigned char *key,
                                  const unsigned char *iv,
                                  unsigned char        header,
                                  unsigned char       *tag );

/**
 * \brief          Process a table of BLE RPA device keys and look for a
 *                 match against the supplied hash
 *
 * \param keytable Pointer to an array of AES-128 keys, corresponding to the
 *                 per-device key in the BLE RPA process
 * \param keymask  Bitmask indicating with key indices in keytable are valid
 * \param prand    24-bit BLE nonce to encrypt with each key and match against hash
 * \param hash     BLE RPA hash to match against (last 24 bits of AES result)
 *
 * \return         0-based index of matching key if a match is found, -1 for no match.
 */
int mbedtls_process_ble_rpa(  const unsigned char   keytable[],
                              uint32_t              keymask,
                              uint32_t              prand,
                              uint32_t              hash );

#ifdef __cplusplus
}
#endif

/** \} (end addtogroup sl_crypto_ble) */
/** \} (end addtogroup sl_crypto) */

#endif /* CRYPTO_BLE_H */
