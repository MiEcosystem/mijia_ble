/**
 ****************************************************************************************
 *
 * @file nvds.h
 *
 * @brief Non Volatile Data Storage (NVDS) driver
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 ****************************************************************************************
 */
#ifndef _NVDS_H_
#define _NVDS_H_

/**
 ****************************************************************************************
 * @addtogroup NVDS
 * @ingroup COMMON
 * @brief Non Volatile Data Storage (NVDS)
 *
 *   Parameters management
 *   there are two compilation options:
 *     + NVDS_8BIT_TAGLENGTH :
 *       if set, each TAG has a maximum length of 256 bytes
 *       if not set, each TAG has a maximum length of 65536 bytes
 *     + NVDS_PACKED :
 *       if not set, all the TAG header structures and TAG data contents are stored with an
 *       alignment on 32 bit boundary
 *       if set, all the TAG header structures and TAG data contents are stored
 *       consecutively without gaps (as would be a structure with pragma packed)
 *     + NVDS_READ_WRITE :
 *       if not set, only GET action on TAGs is provided.
 *       if set, PUT/DEL/LOCK actions are provided in addition of GET action.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdbool.h> // boolean definition
#include <stdint.h>  // integer definition
#include "fsl_flash.h"

/*
 * DEFINES
 ****************************************************************************************
 */
#if defined(CFG_QN908XA)
/// NVDS space location in FLASH : 0x2107E000 (504 - 506KB)
#define CFG_NVDS_ADDRESS (0x2107E000)

/// NVDS backup space location in FLASH : 0x2107D800 (502 - 504KB)
#define CFG_NVDS_BACKUP_ADDRESS (0x2107D800)
#else
/// NVDS space location in FLASH : 0x2107F000 (508 - 510KB)
#define CFG_NVDS_ADDRESS (0x2107F000)

/// NVDS backup space location in FLASH : 0x2107E800 (506 - 508KB)
#define CFG_NVDS_BACKUP_ADDRESS (0x2107E800)
#endif
/// NVDS size in FLASH : 0x00000800 (2KB)
#define CFG_NVDS_SIZE (0x00000800)

/// NVDS is defined as read-write
#define NVDS_READ_WRITE 1

/// NVDS is defined as packed
#define NVDS_PACKED 0

/// NVDS has 8-bit length tags
#define NVDS_8BIT_TAGLENGTH 0

/// Type of the tag length (8 or 16 bits)
#if (NVDS_8BIT_TAGLENGTH)
typedef uint8_t nvds_tag_len_t;
#else
typedef uint16_t nvds_tag_len_t;
#endif // NVDS_8BIT_TAGLENGTH

/*
 * ENUMERATION DEFINITIONS
 ****************************************************************************************
 */

/// Possible Returned Status
enum NVDS_STATUS
{
    /// NVDS status OK
    NVDS_OK,
    /// generic NVDS status KO
    NVDS_FAIL,
    /// NVDS TAG unrecognized
    NVDS_TAG_NOT_DEFINED,
    /// No space for NVDS
    NVDS_NO_SPACE_AVAILABLE,
    /// Length violation
    NVDS_LENGTH_OUT_OF_RANGE,
    /// NVDS parameter locked
    NVDS_PARAM_LOCKED,
    /// NVDS corrupted
    NVDS_CORRUPT,
    /// No backup space
    NVDS_NO_BACKUP_SPACE_AVAILABLE
};

/// List of NVDS TAG identifiers
enum NVDS_TAG
{
    /// Definition of the tag associated to each parameters
    /// Local Bd Address
    NVDS_TAG_BD_ADDRESS = 0x01,
    /// Device Name
    NVDS_TAG_DEVICE_NAME = 0x02,
    /// Radio Drift
    NVDS_TAG_LPCLK_DRIFT = 0x03,
    /// External wake-up time
    NVDS_TAG_EXT_WAKEUP_TIME = 0x04,
    /// Oscillator wake-up time
    NVDS_TAG_OSC_WAKEUP_TIME = 0x05,
    /// 0x06 ~ 0x0f is reserved
    NVDS_TAG_RESERVED = 0x06,

    /// crystal oscillator cap loading selection
    NVDS_TAG_XCSEL = 0x10,
    /// 32.768k crystal oscillator cap loading selection
    NVDS_TAG_32K_XCSEL = 0x11,

    /// BLE Channel Assessment tags
    NVDS_TAG_BLE_CA_TIMER_DUR = 0x20,
    NVDS_TAG_BLE_CRA_TIMER_CNT = 0x21,
    NVDS_TAG_BLE_CA_MIN_THR = 0x22,
    NVDS_TAG_BLE_CA_MAX_THR = 0x23,
    NVDS_TAG_BLE_CA_NOISE_THR = 0x24,
    NVDS_TAG_BLE_PROCEDURE_STARTUP = 0x25,

    /// SP Private Key
    NVDS_TAG_LE_PRIVATE_KEY_P256 = 0x28,
    /// SP Public Key
    NVDS_TAG_LE_PUBLIC_KEY_P256 = 0x29,
    /// SP Debug: Used Fixed Private Key from NVDS
    NVDS_TAG_LE_DBG_FIXED_P256_KEY_EN = 0x2a,
#ifdef MBED_PORT
    NVDS_TAG_BOND_DATA = 0x2B,
#endif

    /// this is the TAG used to be the marker of the last TAG in NVDS (= 0xFF because when
    /// FLASH are erased, they are set to = 0xFF)
    NVDS_END_MARKER_TAG = 0xFF
};

/// List of NVDS Tag lengths
enum NVDS_LEN
{
    // Definition of length associated to each parameters
    /// Local Bd Address
    NVDS_LEN_BD_ADDRESS = 6,
    /// Device Name
    NVDS_LEN_DEVICE_NAME = 248,
    /// Low power clock drift
    NVDS_LEN_LPCLK_DRIFT = 2,
    /// External wake-up time
    NVDS_LEN_EXT_WAKEUP_TIME = 2,
    /// Oscillator wake-up time
    NVDS_LEN_OSC_WAKEUP_TIME = 2,

    /// crystal oscillator cap loading selection
    NVDS_LEN_XCSEL = 1,
    /// 32.768k crystal oscillator cap loading selection
    NVDS_LEN_32K_XCSEL = 1,

    /// BLE Channel Assessment
    NVDS_LEN_BLE_CA_TIMER_DUR = 2,
    NVDS_LEN_BLE_CRA_TIMER_CNT = 1,
    NVDS_LEN_BLE_CA_MIN_THR = 1,
    NVDS_LEN_BLE_CA_MAX_THR = 1,
    NVDS_LEN_BLE_CA_NOISE_THR = 1,
    NVDS_LEN_BLE_PROCEDURE_STARTUP = 2,

    /// P256
    NVDS_LEN_LE_PRIVATE_KEY_P256 = 32,
    NVDS_LEN_LE_PUBLIC_KEY_P256 = 64,
    NVDS_LEN_LE_DBG_FIXED_P256_KEY_EN = 1,
    NVDS_LEN_SP_PRIVATE_KEY_P256 = 32,
    NVDS_LEN_SP_PUBLIC_KEY_P256 = 64,
#ifdef MBED_PORT
    NVDS_LEN_BOND_DATA = 232,
#endif
};

/*
 * STRUCT DEFINITIONS
 ****************************************************************************************
 */

/// NVDS_API
struct nvds_if
{
    /// Function to read the device Address being in the NVDS memory space
    void (*read)(uint32_t const address, uint32_t const length, uint8_t *const buf);
    /// Function to write the device Address being in the NVDS memory space
    void (*write)(uint32_t const address, uint32_t const length, uint8_t *const buf);
    /// Function to erase the entire NVDS memory space
    void (*erase)(uint32_t const address, uint32_t const length);

    uint8_t (*nvds_get)(uint8_t tag, nvds_tag_len_t *lengthPtr, uint8_t *buf);
    uint8_t (*nvds_del)(uint8_t tag);
    uint8_t (*nvds_lock)(uint8_t tag);
    uint8_t (*nvds_put)(uint8_t tag, nvds_tag_len_t length, uint8_t *buf);
};

/// Environment structure of the NVDS module
struct nvds_env_tag
{
    /// NVDS base pointer
    uint8_t *nvds_space;

    /// Total size of the NVDS area
    uint32_t total_size;

    /// NVDS backup pointer
    uint8_t *nvds_backup;

    /// NVDS API
    struct nvds_if *nvds_api;

    /// Flash configuration
    flash_config_t flash_cfg;
};

/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */
/// NVDS environment
extern struct nvds_env_tag nvds_env;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize NVDS.
 * @return NVDS_OK
 ****************************************************************************************
 */
uint8_t nvds_init(uint8_t *base, uint32_t len, uint8_t *backup, struct nvds_if *nvds_api, flash_config_t *flash_cfg);

/**
 ****************************************************************************************
 * @brief Look for a specific tag and return, if found and matching (in length), the
 *        DATA part of the TAG.
 *
 * If the length does not match, the TAG header structure is still filled, in order for
 * the caller to be able to check the actual length of the TAG.
 *
 * @param[in]  tag     TAG to look for whose DATA is to be retrieved
 * @param[in]  length  Expected length of the TAG
 * @param[out] buf     A pointer to the buffer allocated by the caller to be filled with
 *                     the DATA part of the TAG
 *
 * @return  NVDS_OK                  The read operation was performed
 *          NVDS_LENGTH_OUT_OF_RANGE The length passed in parameter is different than the TAG's
 ****************************************************************************************
 */
uint8_t nvds_get(uint8_t tag, nvds_tag_len_t *lengthPtr, uint8_t *buf);

#if (NVDS_READ_WRITE == 1)

/**
 ****************************************************************************************
 * @brief Look for a specific tag and delete it (Status set to invalid)
 *
 * Implementation notes
 * 1. The write function call return status is not handled
 *
 * @param[in]  tag    TAG to mark as deleted
 *
 * @return NVDS_OK                TAG found and deleted
 *         NVDS_PARAM_LOCKED    TAG found but can not be deleted because it is locked
 *         (others)        return values from function call @ref nvds_browse_tag
 ****************************************************************************************
 */
uint8_t nvds_del(uint8_t tag);

/**
 ****************************************************************************************
 * @brief Look for a specific tag and lock it (Status lock bit set to LOCK).
 *
 * The write function call return status is not handled
 *
 * @param[in]  tag    TAG to mark as locked
 *
 * @return NVDS_OK    TAG found and locked
 *         (others)        return values from function call @ref nvds_browse_tag
 ****************************************************************************************
 */
uint8_t nvds_lock(uint8_t tag);

/**
 ****************************************************************************************
 * @brief This function adds a specific TAG to the NVDS.
 *
 * Steps:
 * 1) parse all the TAGs to:
 * 1.1) calculate the total size of all the valid TAGs
 * 1.2) erase the existing TAGs that have the same ID
 * 1.3) check if we can use the same TAG area in case of an EEPROM
 * 1.4) check that the TAG is not locked
 * 2) if we have to add the new TAG at the end fo the NVDS (cant use same area):
 * 2.1) allocate the appropriate amount of memory
 * 2.2) purge the NVDS
 * 2.3) free the memory allocated
 * 2.4) check that there is now enough room for the new TAG or return
 *      NO_SPACE_AVAILABLE
 * 3) add the new TAG
 *
 * @param[in]  tag     TAG to look for whose DATA is to be retrieved
 * @param[in]  length  Expected length of the TAG
 * @param[in]  buf     Pointer to the buffer containing the DATA part of the TAG to add to
 *                     the NVDS
 *
 * @return NVDS_OK                  New TAG correctly written to the NVDS
 *         NVDS_PARAM_LOCKED        New TAG is trying to overwrite a TAG that is locked
 *         NO_SPACE_AVAILABLE       New TAG can not fit in the available space in the NVDS
 ****************************************************************************************
 */
uint8_t nvds_put(uint8_t tag, nvds_tag_len_t length, uint8_t *buf);

#endif //(NVDS_READ_WRITE == 1)

/// @} NVDS

#endif // _NVDS_H_
