#ifndef MBEDTLS_THREADING_ALT_H
#define MBEDTLS_THREADING_ALT_H

/***************************************************************************//**
 * \addtogroup sl_crypto
 * \{
 ******************************************************************************/

/***************************************************************************//**
 * \addtogroup sl_crypto_threading Threading primitives
 * \brief Threading primitive implementation for mbed TLS
 *
 * This file contains the 'glue logic' between the mbed TLS threading API and
 * three RTOS'es, being Micrium uC/OS III, Micrium OS, and FreeRTOS. If adding a
 * custom RTOS, see the implementations given for reference.
 *
 * In order to use the Silicon Labs Hardware Acceleration implementation, you
 * have to call THREADING_setup() as soon as you have brought up your RTOS, and
 * before calling any mbed TLS function.
 *
 * \{
 ******************************************************************************/

#include "mbedtls/threading.h"
#if defined (MBEDTLS_THREADING_ALT) && defined (MBEDTLS_THREADING_C)

#if defined (MBEDTLS_UCOS3)

#define SL_THREADING_ALT

#include "em_assert.h"
#include "os.h"
#include "os_type.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef OS_MUTEX mbedtls_threading_mutex_t;

/**
 * \brief          Initialize a given mutex
 *
 * \param mutex    Pointer to the mutex needing initialization
 */
static inline void THREADING_InitMutex( mbedtls_threading_mutex_t *mutex )
{
  OS_ERR err;
  OSMutexCreate(mutex, "dynamic", &err);
  EFM_ASSERT(err == OS_ERR_NONE);
}

/**
 * \brief          Free a given mutex
 *
 * \param mutex    Pointer to the mutex being freed
 */
static inline void THREADING_FreeMutex( mbedtls_threading_mutex_t *mutex )
{
  OS_ERR err;
  OSMutexDel(mutex, OS_OPT_DEL_ALWAYS, &err);
  EFM_ASSERT(err == OS_ERR_NONE);
}

/**
 * \brief          Pend on a mutex
 *
 * \param mutex    Pointer to the mutex being pended on
 *
 * \return         OS_ERR_NONE on success, error code otherwise.
 */
static inline int THREADING_TakeMutexBlocking( mbedtls_threading_mutex_t *mutex )
{
  OS_ERR err;
  OSMutexPend(mutex,
              0,
              OS_OPT_PEND_BLOCKING,
              NULL,
              &err);
  EFM_ASSERT(err == OS_ERR_NONE);
  return (err == OS_ERR_NONE ? 0 : MBEDTLS_ERR_THREADING_MUTEX_ERROR);
}

/**
 * \brief          Try to own a mutex without waiting
 *
 * \param mutex    Pointer to the mutex being tested
 *
 * \return         OS_ERR_NONE on success (= mutex successfully owned), error code otherwise.
 */
static inline int THREADING_TakeMutexNonBlocking( mbedtls_threading_mutex_t *mutex )
{
  OS_ERR err;
  OSMutexPend(mutex,
              0,
              OS_OPT_PEND_NON_BLOCKING,
              NULL,
              &err);
  return (err == OS_ERR_NONE ? 0 : MBEDTLS_ERR_THREADING_MUTEX_ERROR);
}

/**
 * \brief          Release a mutex
 *
 * \param mutex    Pointer to the mutex being released
 *
 * \return         OS_ERR_NONE on success, error code otherwise.
 */
static inline int THREADING_GiveMutex( mbedtls_threading_mutex_t *mutex )
{
  OS_ERR err;
  OSMutexPost(mutex, OS_OPT_POST_NONE, &err);
  EFM_ASSERT(err == OS_ERR_NONE);
  return (err == OS_ERR_NONE ? 0 : MBEDTLS_ERR_THREADING_MUTEX_ERROR);
}

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_UCOS3 */

#if defined (MBEDTLS_FREERTOS)

#define SL_THREADING_ALT

#include "em_assert.h"
#include "FreeRTOS.h"
#include "semphr.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef SemaphoreHandle_t mbedtls_threading_mutex_t;

static inline void THREADING_InitMutex( mbedtls_threading_mutex_t *mutex )
{
  *mutex = (SemaphoreHandle_t) xSemaphoreCreateMutex();
  EFM_ASSERT(*mutex != NULL);
}

static inline void THREADING_FreeMutex( mbedtls_threading_mutex_t *mutex )
{
  (void) mutex;
  /*
    Removed call to
    vSemaphoreDelete( (SemaphoreHandle_t) *pComp );

    Mutex semaphores can not be deleted with default heap management of the
    current port. The following comment is from the vPortFree function of
    FreeRTOS which is called from vSemaphoreDelete (and subsequently
    vQueueDelete).
  */
  /* Memory cannot be freed using this scheme.  See heap_2.c, heap_3.c and
     heap_4.c for alternative implementations, and the memory management pages
     of http://www.FreeRTOS.org for more information. */
}

static inline int THREADING_TakeMutexBlocking( mbedtls_threading_mutex_t *mutex )
{
  BaseType_t status =
    xSemaphoreTake( *mutex, (TickType_t) portMAX_DELAY );
  return (status == pdTRUE ? 0 : MBEDTLS_ERR_THREADING_MUTEX_ERROR);
}

static inline int THREADING_TakeMutexNonBlocking( mbedtls_threading_mutex_t *mutex )
{
  BaseType_t status =
    xSemaphoreTake( *mutex, (TickType_t) 0 );
  return (status == pdTRUE ? 0 : MBEDTLS_ERR_THREADING_MUTEX_ERROR);
}

static inline int THREADING_GiveMutex( mbedtls_threading_mutex_t *mutex )
{
  BaseType_t status = xSemaphoreGive( *mutex );
  EFM_ASSERT(status == pdTRUE);
  return (status == pdTRUE ? 0 : MBEDTLS_ERR_THREADING_MUTEX_ERROR);
}

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_FREERTOS */

#if defined (MBEDTLS_MICRIUMOS)

#define SL_THREADING_ALT

#include "em_assert.h"
#include <kernel/include/os.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef OS_MUTEX mbedtls_threading_mutex_t;

static inline void THREADING_InitMutex( mbedtls_threading_mutex_t *mutex )
{
  RTOS_ERR err;
  OSMutexCreate(mutex, "dynamic", &err);
  EFM_ASSERT(err.Code == RTOS_ERR_NONE);
}

static inline void THREADING_FreeMutex( mbedtls_threading_mutex_t *mutex )
{
  RTOS_ERR err;
  OSMutexDel(mutex, OS_OPT_DEL_ALWAYS, &err);
  EFM_ASSERT(err.Code == RTOS_ERR_NONE);
}

static inline int THREADING_TakeMutexBlocking( mbedtls_threading_mutex_t *mutex )
{
  RTOS_ERR err;
  OSMutexPend(mutex,
              0,
              OS_OPT_PEND_BLOCKING,
              NULL,
              &err);
  EFM_ASSERT(err.Code == RTOS_ERR_NONE);
  return (err.Code == RTOS_ERR_NONE ? 0 : MBEDTLS_ERR_THREADING_MUTEX_ERROR);
}

static inline int THREADING_TakeMutexNonBlocking( mbedtls_threading_mutex_t *mutex )
{
  RTOS_ERR err;
  OSMutexPend(mutex,
              0,
              OS_OPT_PEND_NON_BLOCKING,
              NULL,
              &err);
  return (err.Code == RTOS_ERR_NONE ? 0 : MBEDTLS_ERR_THREADING_MUTEX_ERROR);
}

static inline int THREADING_GiveMutex( mbedtls_threading_mutex_t *mutex )
{
  RTOS_ERR err;
  OSMutexPost(mutex, OS_OPT_POST_NONE, &err);
  EFM_ASSERT(err.Code == RTOS_ERR_NONE);
  return (err.Code == RTOS_ERR_NONE ? 0 : MBEDTLS_ERR_THREADING_MUTEX_ERROR);
}

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_MICRIUMOS */

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declaration of threading_set_alt */
void mbedtls_threading_set_alt( void (*mutex_init)( mbedtls_threading_mutex_t * ),
                       void (*mutex_free)( mbedtls_threading_mutex_t * ),
                       int (*mutex_lock)( mbedtls_threading_mutex_t * ),
                       int (*mutex_unlock)( mbedtls_threading_mutex_t * ) );

/**
 * \brief          Helper function for setting up the mbed TLS threading subsystem
 */
static inline void THREADING_setup( void )
{
  mbedtls_threading_set_alt(&THREADING_InitMutex,
                            &THREADING_FreeMutex,
                            &THREADING_TakeMutexBlocking,
                            &THREADING_GiveMutex);
}

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_THREADING_ALT && MBEDTLS_THREADING_C */

/** \} (end addtogroup sl_crypto_threading) */
/** \} (end addtogroup sl_crypto) */

#endif /* MBEDTLS_THREADING_ALT_H */
