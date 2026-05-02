/**
 * @file par_layout.h
 * @brief Declare parameter storage layout helpers.
 * @author wdfk-prog
 * @version 1.1
 * @date 2026-04-22
 *
 * @copyright Copyright (c) 2026 Ziga Miklosic. Distributed under the MIT license.
 *
 * @note :
 * @par Change Log:
 * Date       Version Author        Description
 * 2026-03-27 1.0     wdfk-prog     first version
 * 2026-04-22 1.1     wdfk-prog     add object layout counters and static object-pool map
 */
#ifndef _PAR_LAYOUT_H_
#define _PAR_LAYOUT_H_
/**
 * @brief Include dependencies.
 */
#include <stdint.h>

#include "par_cfg.h"
#include "def/par_def.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Aggregated storage counts for one active layout source.
 */
typedef struct
{
    uint16_t count8;       /**< Number of 8-bit scalar storage entries. */
    uint16_t count16;      /**< Number of 16-bit scalar storage entries. */
    uint16_t count32;      /**< Number of 32-bit scalar storage entries. */
    uint16_t count_obj;    /**< Number of object storage slots. */
    uint32_t obj_pool_bytes; /**< Total object-pool size in bytes. */
} par_layout_count_t;

#if (PAR_CFG_LAYOUT_SOURCE == PAR_CFG_LAYOUT_COMPILE_SCAN)
#define PAR_STORAGE_COUNT8         (PAR_LAYOUT_COMPILE_COUNT8)
#define PAR_STORAGE_COUNT16        (PAR_LAYOUT_COMPILE_COUNT16)
#define PAR_STORAGE_COUNT32        (PAR_LAYOUT_COMPILE_COUNT32)
#define PAR_STORAGE_COUNTOBJ       (PAR_LAYOUT_COMPILE_COUNTOBJ)
#define PAR_STORAGE_OBJ_POOL_BYTES (PAR_LAYOUT_COMPILE_OBJ_POOL_BYTES)
#elif (PAR_CFG_LAYOUT_SOURCE == PAR_CFG_LAYOUT_SCRIPT)
#include PAR_CFG_LAYOUT_STATIC_INCLUDE
#define PAR_STORAGE_COUNT8         (PAR_LAYOUT_STATIC_COUNT8)
#define PAR_STORAGE_COUNT16        (PAR_LAYOUT_STATIC_COUNT16)
#define PAR_STORAGE_COUNT32        (PAR_LAYOUT_STATIC_COUNT32)
#if (PAR_LAYOUT_COMPILE_COUNTOBJ > 0u)
#ifndef PAR_LAYOUT_STATIC_COUNTOBJ
#error "PAR_LAYOUT_STATIC_COUNTOBJ must be provided by static layout include when object rows are present!"
#endif /* !defined(PAR_LAYOUT_STATIC_COUNTOBJ) */
#ifndef PAR_LAYOUT_STATIC_OBJ_POOL_BYTES
#error "PAR_LAYOUT_STATIC_OBJ_POOL_BYTES must be provided by static layout include when object rows are present!"
#endif /* !defined(PAR_LAYOUT_STATIC_OBJ_POOL_BYTES) */
#define PAR_STORAGE_COUNTOBJ       (PAR_LAYOUT_STATIC_COUNTOBJ)
#define PAR_STORAGE_OBJ_POOL_BYTES (PAR_LAYOUT_STATIC_OBJ_POOL_BYTES)
PAR_STATIC_ASSERT(par_layout_static_count_obj_matches_table, (PAR_STORAGE_COUNTOBJ == PAR_LAYOUT_COMPILE_COUNTOBJ));
PAR_STATIC_ASSERT(par_layout_static_obj_pool_bytes_matches_table, (PAR_STORAGE_OBJ_POOL_BYTES == PAR_LAYOUT_COMPILE_OBJ_POOL_BYTES));
#else
#define PAR_STORAGE_COUNTOBJ       (0u)
#define PAR_STORAGE_OBJ_POOL_BYTES (0u)
#endif /* (PAR_LAYOUT_COMPILE_COUNTOBJ > 0u) */
#else
#error "Unsupported PAR_CFG_LAYOUT_SOURCE value!"
#endif

#define PAR_STORAGE_NONZERO(count_) (((count_) > 0u) ? (count_) : 1u)
/**
 * @brief Return the active parameter offset table.
 * @return Pointer to the active offset table.
 */
const uint16_t *par_layout_get_offset_table(void);
/**
 * @brief Return the storage offset for one parameter.
 * @param par_num Parameter number.
 * @return Storage offset for one parameter.
 */
uint16_t par_layout_get_offset(const par_num_t par_num);
/**
 * @brief Return the shared object-pool byte offset for one parameter.
 * @param par_num Parameter number.
 * @return Object-pool byte offset for one parameter.
 */
uint32_t par_layout_get_obj_pool_offset(const par_num_t par_num);
/**
 * @brief Return the grouped storage counts.
 * @return Storage counts for scalar groups and the object pool.
 */
par_layout_count_t par_layout_get_count(void);
/**
 * @brief Initialize the storage layout metadata.
 */
void par_layout_init(void);

#ifdef __cplusplus
}
#endif

#endif /* _PAR_LAYOUT_H_ */
