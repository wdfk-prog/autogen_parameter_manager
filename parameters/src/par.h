/**
 * @file par.h
 * @brief Declare the public device-parameter API.
 * @author Ziga Miklosic
 * @version V3.0.1
 * @date 2026-01-29
 *
 * @copyright Copyright (c) 2026 Ziga Miklosic. Distributed under the MIT license.
 *
 * @note :
 * @par Change Log:
 * Date       Version Author        Description
 * 2026-01-29 V3.0.1  Ziga Miklosic first version
 */
/**
 * @addtogroup PARAMETERS_API
 * @{ <!-- BEGIN GROUP -->
 */

#ifndef _PAR_H_
#define _PAR_H_
/**
 * @brief Include dependencies.
 */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "par_cfg.h"
/**
 * @brief Compile-time definitions.
 */
/**
 * @brief Module version.
 */
#define PAR_VER_MAJOR   (3)
#define PAR_VER_MINOR   (0)
#define PAR_VER_DEVELOP (2)

/**
 * @brief Parameter status.
 */
typedef enum
{
    ePAR_OK = 0U, /**< Normal operation. */

    /* Error status bits. */
    ePAR_STATUS_ERROR_MASK = 0x07FFU,
    ePAR_ERROR = 0x0001U,          /**< General parameter error. */
    ePAR_ERROR_INIT = 0x0002U,     /**< Parameter initialization error or usage before initialization. */
    ePAR_ERROR_NVM = 0x0004U,      /**< Parameter storage to NVM error. */
    ePAR_ERROR_CRC = 0x0008U,      /**< Parameter CRC corrupted. */
    ePAR_ERROR_TYPE = 0x0010U,     /**< Using invalid API function for given parameter data type. */
    ePAR_ERROR_MUTEX = 0x0020U,    /**< Acquiring mutex failed. */
    ePAR_ERROR_VALUE = 0x0040U,    /**< Invalid parameter value (validation failed). */
    ePAR_ERROR_PARAM = 0x0080U,    /**< Invalid function argument. */
    ePAR_ERROR_PAR_NUM = 0x0100U,  /**< Invalid parameter number. */
    ePAR_ERROR_ACCESS = 0x0200U,   /**< Access denied by parameter access policy. */
    ePAR_ERROR_TABLE_ID = 0x0400U, /**< Stored parameter-table ID does not match the live table. */

    /* Warning status bits. */
    ePAR_STATUS_WAR_MASK = 0xF800U,
    ePAR_WAR_SET_TO_DEF = 0x0800U,    /**< Parameters set to default. */
    ePAR_WAR_NVM_REWRITTEN = 0x1000U, /**< NVM parameters area completely re-written. */
    ePAR_WAR_NO_PERSISTENT = 0x2000U, /**< No persistent parameters -> set PAR_CFG_NVM_EN to 0. */
    ePAR_WAR_LIMITED = 0x4000U,       /**< Parameter value limited within [min,max]. */
} par_status_t;
/**
 * @brief Parameters type enumeration.
 */
typedef enum
{
    ePAR_TYPE_U8 = 0,  /**< Unsigned 8-bit value. */
    ePAR_TYPE_U16,     /**< Unsigned 16-bit value. */
    ePAR_TYPE_U32,     /**< Unsigned 32-bit value. */
    ePAR_TYPE_I8,      /**< Signed 8-bit value. */
    ePAR_TYPE_I16,     /**< Signed 16-bit value. */
    ePAR_TYPE_I32,     /**< Signed 32-bit value. */
    ePAR_TYPE_F32,     /**< 32-bit floating value. */
#if (1 == PAR_CFG_ENABLE_TYPE_STR)
    ePAR_TYPE_STR,     /**< Fixed-capacity string stored out-of-line. */
#endif /* (1 == PAR_CFG_ENABLE_TYPE_STR) */
#if (1 == PAR_CFG_ENABLE_TYPE_BYTES)
    ePAR_TYPE_BYTES,   /**< Fixed-capacity byte array stored out-of-line. */
#endif /* (1 == PAR_CFG_ENABLE_TYPE_BYTES) */
#if (1 == PAR_CFG_ENABLE_TYPE_ARR_U8)
    ePAR_TYPE_ARR_U8,  /**< Fixed-size unsigned 8-bit array stored out-of-line. */
#endif /* (1 == PAR_CFG_ENABLE_TYPE_ARR_U8) */
#if (1 == PAR_CFG_ENABLE_TYPE_ARR_U16)
    ePAR_TYPE_ARR_U16, /**< Fixed-size unsigned 16-bit array stored out-of-line. */
#endif /* (1 == PAR_CFG_ENABLE_TYPE_ARR_U16) */
#if (1 == PAR_CFG_ENABLE_TYPE_ARR_U32)
    ePAR_TYPE_ARR_U32, /**< Fixed-size unsigned 32-bit array stored out-of-line. */
#endif /* (1 == PAR_CFG_ENABLE_TYPE_ARR_U32) */
    ePAR_TYPE_NUM_OF
} par_type_list_t;
/**
 * @brief Parameter number type.
 */
typedef uint16_t par_num_t;
#if (1 == PAR_CFG_ENABLE_ACCESS)
/**
 * @brief Parameter read/write capability bit mask.
 * @details Parameter table rows must use either ePAR_ACCESS_RO or
 * ePAR_ACCESS_RW. The ePAR_ACCESS_NONE value is reserved as a sentinel for
 * invalid metadata lookups and is rejected by compile-time table checks.
 * Write-only access is intentionally unsupported.
 */
typedef enum
{
    ePAR_ACCESS_NONE = 0U,                                    /**< Invalid/no-access sentinel; not valid in table rows. */
    ePAR_ACCESS_READ = (1U << 0),                             /**< External read capability bit. */
    ePAR_ACCESS_WRITE = (1U << 1),                            /**< External write capability bit, valid only with read capability. */
    ePAR_ACCESS_RO = ePAR_ACCESS_READ,                        /**< Parameter read only. */
    ePAR_ACCESS_RW = (ePAR_ACCESS_READ | ePAR_ACCESS_WRITE),  /**< Parameter read/write. */
} par_access_t;
#endif /* (1 == PAR_CFG_ENABLE_ACCESS) */
#if (1 == PAR_CFG_ENABLE_ROLE_POLICY)
/**
 * @brief External role-mask bits used by optional parameter role policy.
 */
typedef enum
{
    ePAR_ROLE_NONE = 0U,                                   /**< No external role granted. */
    ePAR_ROLE_PUBLIC = (1U << 0),                          /**< End-user/public role. */
    ePAR_ROLE_SERVICE = (1U << 1),                         /**< Service/maintenance role. */
    ePAR_ROLE_DEVELOPER = (1U << 2),                       /**< Developer/debug role. */
    ePAR_ROLE_MANUFACTURING = (1U << 3),                   /**< Manufacturing/production role. */
    ePAR_ROLE_ALL = (ePAR_ROLE_PUBLIC | ePAR_ROLE_SERVICE | ePAR_ROLE_DEVELOPER | ePAR_ROLE_MANUFACTURING), /**< All external roles. */
} par_role_t;
#endif /* (1 == PAR_CFG_ENABLE_ROLE_POLICY) */
/**
 * @brief 32-bit floating data type definition.
 */
typedef float float32_t;
/**
 * @brief Supported data types.
 */
typedef union
{
    uint8_t u8;    /**< Unsigned 8-bit value. */
    uint16_t u16;  /**< Unsigned 16-bit value. */
    uint32_t u32;  /**< Unsigned 32-bit value. */
    int8_t i8;     /**< Signed 8-bit value. */
    int16_t i16;   /**< Signed 16-bit value. */
    int32_t i32;   /**< Signed 32-bit value. */
    float32_t f32; /**< 32-bit floating value. */
} par_type_t;
/**
 * @brief Parameter value range.
 */
#if (1 == PAR_CFG_ENABLE_RANGE)
typedef struct
{
    par_type_t min; /**< Minimum value. */
    par_type_t max; /**< Maximum value. */
} par_range_t;
#endif /* (1 == PAR_CFG_ENABLE_RANGE) */

/**
 * @brief Scalar parameter metadata stored in the unified parameter table.
 */
typedef struct
{
#if (1 == PAR_CFG_ENABLE_RANGE)
    par_range_t range; /**< Scalar value range. */
#endif /* (1 == PAR_CFG_ENABLE_RANGE) */
    par_type_t def;    /**< Scalar default value. */
} par_scalar_cfg_t;

#if (1 == PAR_CFG_OBJECT_TYPES_ENABLED)
/**
 * @brief Default initializer descriptor for one fixed-capacity object parameter.
 */
typedef struct
{
    const uint8_t *p_data; /**< Pointer to default payload bytes, or NULL for an empty default. */
    uint16_t len;          /**< Default payload length in bytes. */
} par_obj_init_t;

/**
 * @brief Payload-length range metadata for one fixed-capacity object parameter.
 * @details For string rows, max_len is the stored payload capacity and does
 * not include the trailing NUL written by par_get_str() and
 * par_get_default_str(). String max_len is therefore limited to
 * UINT16_MAX - 1U so a uint16_t buffer-size argument can still represent the
 * required read buffer size.
 */
typedef struct
{
    uint16_t min_len; /**< Minimum accepted payload length in bytes. */
    uint16_t max_len; /**< Maximum accepted payload length in bytes. */
} par_obj_range_t;

/**
 * @brief Fixed-capacity object metadata stored in the unified parameter table.
 * @details Object parameters keep their live payload in the shared object pool,
 * while the table stores static metadata such as element size, accepted
 * payload-length range, and default payload descriptor.
 */
typedef struct
{
    uint16_t elem_size;    /**< Object element size in bytes. */
    par_obj_range_t range; /**< Accepted payload-length range in bytes. */
    par_obj_init_t def;    /**< Default payload descriptor. */
} par_object_cfg_t;
#endif /* (1 == PAR_CFG_OBJECT_TYPES_ENABLED) */

/**
 * @brief Type-specific metadata stored by one parameter-table entry.
 * @details When any object type is enabled, scalar and object metadata are
 * mutually exclusive because one table row always resolves to exactly one
 * value type. The union keeps the metadata table compact without changing the
 * scalar access model. When all object types are disabled, only scalar
 * metadata remains and the object member is compiled out.
 */
#if (1 == PAR_CFG_OBJECT_TYPES_ENABLED)
typedef union
{
    par_scalar_cfg_t scalar; /**< Scalar metadata. */
    par_object_cfg_t object; /**< Object metadata. */
} par_value_cfg_t;
#else
typedef struct
{
    par_scalar_cfg_t scalar; /**< Scalar metadata. */
} par_value_cfg_t;
#endif /* (1 == PAR_CFG_OBJECT_TYPES_ENABLED) */

/**
 * @brief Parameter data settings.
 * @note The exact object size depends on enabled metadata fields and target ABI.
 */
typedef struct par_cfg_s
{
#if (1 == PAR_CFG_ENABLE_NAME)
    const char *name;         /**< Parameter display name. */
#endif /* (1 == PAR_CFG_ENABLE_NAME) */
    par_value_cfg_t value_cfg; /**< Type-specific scalar or object metadata. */
#if (1 == PAR_CFG_ENABLE_UNIT)
    const char *unit;         /**< Parameter engineering unit string. */
#endif /* (1 == PAR_CFG_ENABLE_UNIT) */
#if (1 == PAR_CFG_ENABLE_DESC)
    const char *desc;         /**< Parameter description string. */
#endif /* (1 == PAR_CFG_ENABLE_DESC) */
#if (1 == PAR_CFG_ENABLE_ID)
    uint16_t id;              /**< External parameter ID. */
#endif /* (1 == PAR_CFG_ENABLE_ID) */
    par_type_list_t type;     /**< Parameter type. */
#if (1 == PAR_CFG_ENABLE_ACCESS)
    par_access_t access;      /**< External access capability mask. */
#endif /* (1 == PAR_CFG_ENABLE_ACCESS) */
#if (1 == PAR_CFG_ENABLE_ROLE_POLICY)
    par_role_t read_roles;    /**< External roles allowed to read the parameter. */
    par_role_t write_roles;   /**< External roles allowed to write the parameter. */
#endif /* (1 == PAR_CFG_ENABLE_ROLE_POLICY) */
#if (1 == PAR_CFG_NVM_EN)
    bool persistent;          /**< Parameter persistence flag. */
    uint16_t persist_idx;     /**< Persistent slot index or PAR_PERSIST_IDX_INVALID. */
#endif /* (1 == PAR_CFG_NVM_EN) */
} par_cfg_t;
/**
 * @brief Device Parameters on-change callback.
 * @note The callback runs only on the normal setter path. Startup default
 * initialization, raw restore paths, fast setters, and bitwise fast setters do
 * not invoke it.
 * @note Keep the callback synchronous, short, and non-blocking. Avoid long I/O,
 * waits, sleeps, or any operation that extends the parameter lock hold time.
 * @note The callback is invoked synchronously from the normal scalar setter
 * path while the setter still owns the parameter lock. Do not call parameter
 * APIs for the same parameter from the callback. Avoid cross-parameter set/save
 * flows unless the application owns a strict lock-order policy.
 * @note Re-entering the parameter module from this callback is an advanced usage
 * pattern and must be reviewed carefully at application level. Prefer deferring
 * follow-up writes or persistence work to an application task or event queue.
 */
typedef void (*pf_par_on_change_cb_t)(const par_num_t par_num, const par_type_t new_val, const par_type_t old_val);
/**
 * @brief Device Parameters validation callback.
 * @note Validation runs only on the normal setter path. Startup default
 * initialization, raw restore paths, fast setters, and bitwise fast setters do
 * not invoke it.
 * @note Keep validation synchronous, short, and non-blocking. Avoid long I/O,
 * waits, sleeps, or any operation that extends the parameter lock hold time.
 * @note Re-entering the parameter module from validation is an advanced usage
 * pattern and must be reviewed carefully at application level.
 */
typedef bool (*pf_par_validation_t)(const par_num_t par_num, const par_type_t val);
#if (1 == PAR_CFG_OBJECT_TYPES_ENABLED)
/**
 * @brief Device Parameters object validation callback.
 * @param par_num Parameter number.
 * @param p_data Pointer to the candidate payload bytes.
 * @param len Candidate payload length in bytes.
 * @return True when the payload is accepted; otherwise false.
 * @note Object setters do not provide a core on-change callback. Integrations
 * that need object change notifications should wrap successful object setter
 * calls at a higher layer.
 */
typedef bool (*pf_par_obj_validation_t)(const par_num_t par_num,
                                        const uint8_t *p_data,
                                        const uint16_t len);
#endif /* (1 == PAR_CFG_OBJECT_TYPES_ENABLED) */
/**
 * @brief Function declarations.
 */
/**
 * @brief Initialize the parameter module.
 * @return Operation status.
 */
par_status_t par_init(void);
/**
 * @brief Deinitialize the parameter module.
 * @return Operation status.
 */
par_status_t par_deinit(void);
/**
 * @brief Report whether the parameter module is initialized.
 * @return True when the module is initialized; otherwise false.
 */
bool par_is_init(void);
/**
 * @brief Acquire the parameter lock for one parameter path.
 * @param par_num Parameter number.
 * @return Operation status.
 */
par_status_t par_acquire_mutex(const par_num_t par_num);
/**
 * @brief Release the parameter lock for one parameter path.
 * @param par_num Parameter number.
 */
void par_release_mutex(const par_num_t par_num);
/**
 * @brief Typed parameter setter API.
 */
/**
 * @brief Macro-generated typed API note.
 * @details par_set_u8/i8/u16/i16/u32/i32(/f32), par_set_xxx_fast, and
 * par_get_xxx are generated by macros. Their declarations are listed here,
 * while the implementations are emitted in par.c through
 * #include "par_typed_impl.inc".
 */
/**
 * @brief Set one scalar parameter from a typed input pointer.
 * @details Object-backed rows do not use this generic setter. Use the dedicated
 * object typed setters instead.
 * @param par_num Parameter number.
 * @param p_val Pointer to the input value.
 * @return Operation status. Returns ePAR_ERROR_TYPE for object rows or ePAR_ERROR_ACCESS when write access is denied.
 */
par_status_t par_set(const par_num_t par_num, const void *p_val);
/**
 * @brief Set one scalar parameter from a typed input pointer through the unchecked fast path.
 *
 * @note This generic fast setter validates initialization, parameter number,
 * and pointer arguments, then dispatches to the matching typed
 * `par_set_xxx_fast()` implementation.
 *
 * @note It intentionally bypasses access enforcement, runtime validation
 * callbacks, and on-change callbacks. Range limiting still follows
 * the typed fast setter behavior.
 *
 * @note Object-backed rows do not use this generic fast setter.
 *
 * @param par_num Parameter number.
 * @param p_val Pointer to the input value.
 * @return Operation status. Returns ePAR_ERROR_TYPE for object rows.
 */
par_status_t par_set_fast(const par_num_t par_num, const void *p_val);
#if (1 == PAR_CFG_OBJECT_TYPES_ENABLED)
#if (1 == PAR_CFG_ENABLE_TYPE_BYTES)
/**
 * @brief Set one fixed-capacity byte-array parameter.
 * @param par_num Parameter number.
 * @param p_data Pointer to the source bytes.
 * @param len Payload length in bytes.
 * @return Operation status.
 */
par_status_t par_set_bytes(const par_num_t par_num, const uint8_t *p_data, const uint16_t len);
#endif /* (1 == PAR_CFG_ENABLE_TYPE_BYTES) */
#if (1 == PAR_CFG_ENABLE_TYPE_BYTES)
/**
 * @brief Read one fixed-capacity byte-array parameter.
 * @param par_num Parameter number.
 * @param p_buf Pointer to the destination buffer.
 * @param buf_size Destination buffer size in bytes.
 * @param[out] p_out_len Pointer to the returned payload length in bytes.
 * @return Operation status. Returns ePAR_ERROR_ACCESS when read access is denied.
 */
par_status_t par_get_bytes(const par_num_t par_num, uint8_t *p_buf, const uint16_t buf_size, uint16_t *p_out_len);
#endif /* (1 == PAR_CFG_ENABLE_TYPE_BYTES) */
#if (1 == PAR_CFG_ENABLE_TYPE_STR)
/**
 * @brief Set one fixed-capacity string parameter.
 * @param par_num Parameter number.
 * @param p_str Pointer to a NUL-terminated source string, or NULL
 * for an empty string.
 * @return Operation status.
 */
par_status_t par_set_str(const par_num_t par_num, const char *p_str);
#endif /* (1 == PAR_CFG_ENABLE_TYPE_STR) */
#if (1 == PAR_CFG_ENABLE_TYPE_STR)
/**
 * @brief Read one fixed-capacity string parameter.
 * @param par_num Parameter number.
 * @param p_buf Pointer to the destination character buffer.
 * @param buf_size Destination buffer size in bytes, including the trailing '\0'.
 * Must be at least payload length + 1.
 * @param[out] p_out_len Pointer to the returned string length without the trailing '\0'.
 * @return Operation status. Returns ePAR_ERROR_ACCESS when read access is denied.
 */
par_status_t par_get_str(const par_num_t par_num, char *p_buf, const uint16_t buf_size, uint16_t *p_out_len);
#endif /* (1 == PAR_CFG_ENABLE_TYPE_STR) */
#if (1 == PAR_CFG_ENABLE_TYPE_ARR_U8)
/**
 * @brief Set one fixed-size unsigned 8-bit array parameter.
 * @param par_num Parameter number.
 * @param p_data Pointer to the source elements.
 * @param count Number of elements to write.
 * @return Operation status.
 */
par_status_t par_set_arr_u8(const par_num_t par_num, const uint8_t *p_data, const uint16_t count);
#endif /* (1 == PAR_CFG_ENABLE_TYPE_ARR_U8) */
#if (1 == PAR_CFG_ENABLE_TYPE_ARR_U8)
/**
 * @brief Read one fixed-size unsigned 8-bit array parameter.
 * @param par_num Parameter number.
 * @param p_buf Pointer to the destination buffer.
 * @param buf_count Destination capacity in elements.
 * @param[out] p_out_count Pointer to the returned element count.
 * @return Operation status. Returns ePAR_ERROR_ACCESS when read access is denied.
 */
par_status_t par_get_arr_u8(const par_num_t par_num, uint8_t *p_buf, const uint16_t buf_count, uint16_t *p_out_count);
#endif /* (1 == PAR_CFG_ENABLE_TYPE_ARR_U8) */
#if (1 == PAR_CFG_ENABLE_TYPE_ARR_U16)
/**
 * @brief Set one fixed-size unsigned 16-bit array parameter.
 * @param par_num Parameter number.
 * @param p_data Pointer to the source elements.
 * @param count Number of elements to write.
 * @return Operation status.
 */
par_status_t par_set_arr_u16(const par_num_t par_num, const uint16_t *p_data, const uint16_t count);
#endif /* (1 == PAR_CFG_ENABLE_TYPE_ARR_U16) */
#if (1 == PAR_CFG_ENABLE_TYPE_ARR_U16)
/**
 * @brief Read one fixed-size unsigned 16-bit array parameter.
 * @param par_num Parameter number.
 * @param p_buf Pointer to the destination buffer.
 * @param buf_count Destination capacity in elements.
 * @param[out] p_out_count Pointer to the returned element count.
 * @return Operation status. Returns ePAR_ERROR_ACCESS when read access is denied.
 */
par_status_t par_get_arr_u16(const par_num_t par_num, uint16_t *p_buf, const uint16_t buf_count, uint16_t *p_out_count);
#endif /* (1 == PAR_CFG_ENABLE_TYPE_ARR_U16) */
#if (1 == PAR_CFG_ENABLE_TYPE_ARR_U32)
/**
 * @brief Set one fixed-size unsigned 32-bit array parameter.
 * @param par_num Parameter number.
 * @param p_data Pointer to the source elements.
 * @param count Number of elements to write.
 * @return Operation status.
 */
par_status_t par_set_arr_u32(const par_num_t par_num, const uint32_t *p_data, const uint16_t count);
#endif /* (1 == PAR_CFG_ENABLE_TYPE_ARR_U32) */
#if (1 == PAR_CFG_ENABLE_TYPE_ARR_U32)
/**
 * @brief Read one fixed-size unsigned 32-bit array parameter.
 * @param par_num Parameter number.
 * @param p_buf Pointer to the destination buffer.
 * @param buf_count Destination capacity in elements.
 * @param[out] p_out_count Pointer to the returned element count.
 * @return Operation status. Returns ePAR_ERROR_ACCESS when read access is denied.
 */
par_status_t par_get_arr_u32(const par_num_t par_num, uint32_t *p_buf, const uint16_t buf_count, uint16_t *p_out_count);
#endif /* (1 == PAR_CFG_ENABLE_TYPE_ARR_U32) */
/**
 * @brief Return the current payload length of one object parameter.
 * @param par_num Parameter number.
 * @param[out] p_len Pointer to the returned payload length in bytes.
 * @return Operation status. Returns ePAR_ERROR_ACCESS when read access is denied.
 */
par_status_t par_get_obj_len(const par_num_t par_num, uint16_t *p_len);
/**
 * @brief Return the configured payload capacity of one object parameter.
 * @param par_num Parameter number.
 * @param[out] p_capacity Pointer to the returned payload capacity in bytes.
 * @return Operation status.
 */
par_status_t par_get_obj_capacity(const par_num_t par_num, uint16_t *p_capacity);
#if (1 == PAR_CFG_ENABLE_TYPE_BYTES)
/**
 * @brief Read the default byte-array payload for one object parameter.
 * @param par_num Parameter number.
 * @param p_buf Pointer to the destination buffer.
 * @param buf_size Destination buffer size in bytes.
 * @param[out] p_out_len Pointer to the returned default payload length in bytes.
 * @return Operation status.
 */
par_status_t par_get_default_bytes(const par_num_t par_num, uint8_t *p_buf, const uint16_t buf_size, uint16_t *p_out_len);
#endif /* (1 == PAR_CFG_ENABLE_TYPE_BYTES) */
#if (1 == PAR_CFG_ENABLE_TYPE_STR)
/**
 * @brief Read the default string payload for one object parameter.
 * @param par_num Parameter number.
 * @param p_buf Pointer to the destination character buffer.
 * @param buf_size Destination buffer size in bytes, including the trailing '\0'.
 * Must be at least payload length + 1.
 * @param[out] p_out_len Pointer to the returned default string length without the trailing '\0'.
 * @return Operation status.
 */
par_status_t par_get_default_str(const par_num_t par_num, char *p_buf, const uint16_t buf_size, uint16_t *p_out_len);
#endif /* (1 == PAR_CFG_ENABLE_TYPE_STR) */
#if (1 == PAR_CFG_ENABLE_TYPE_ARR_U8)
/**
 * @brief Read the default unsigned 8-bit array payload for one object parameter.
 * @param par_num Parameter number.
 * @param p_buf Pointer to the destination buffer.
 * @param buf_count Destination capacity in elements.
 * @param[out] p_out_count Pointer to the returned element count.
 * @return Operation status.
 */
par_status_t par_get_default_arr_u8(const par_num_t par_num, uint8_t *p_buf, const uint16_t buf_count, uint16_t *p_out_count);
#endif /* (1 == PAR_CFG_ENABLE_TYPE_ARR_U8) */
#if (1 == PAR_CFG_ENABLE_TYPE_ARR_U16)
/**
 * @brief Read the default unsigned 16-bit array payload for one object parameter.
 * @param par_num Parameter number.
 * @param p_buf Pointer to the destination buffer.
 * @param buf_count Destination capacity in elements.
 * @param[out] p_out_count Pointer to the returned element count.
 * @return Operation status.
 */
par_status_t par_get_default_arr_u16(const par_num_t par_num, uint16_t *p_buf, const uint16_t buf_count, uint16_t *p_out_count);
#endif /* (1 == PAR_CFG_ENABLE_TYPE_ARR_U16) */
#if (1 == PAR_CFG_ENABLE_TYPE_ARR_U32)
/**
 * @brief Read the default unsigned 32-bit array payload for one object parameter.
 * @param par_num Parameter number.
 * @param p_buf Pointer to the destination buffer.
 * @param buf_count Destination capacity in elements.
 * @param[out] p_out_count Pointer to the returned element count.
 * @return Operation status.
 */
par_status_t par_get_default_arr_u32(const par_num_t par_num, uint32_t *p_buf, const uint16_t buf_count, uint16_t *p_out_count);
#endif /* (1 == PAR_CFG_ENABLE_TYPE_ARR_U32) */
#endif /* (1 == PAR_CFG_OBJECT_TYPES_ENABLED) */
#if (1 == PAR_CFG_ENABLE_ID)
/**
 * @brief Set one scalar parameter by external parameter ID.
 * @details Object-backed rows do not use the generic ID-based setter. Use the
 * dedicated object typed setters after resolving the parameter number in the
 * application layer.
 * @param id External parameter ID.
 * @param p_val Pointer to the input value.
 * @return Operation status. Returns ePAR_ERROR_TYPE for object rows or ePAR_ERROR_ACCESS when write access is denied.
 */
par_status_t par_set_by_id(const uint16_t id, const void *p_val);
#endif /* (1 == PAR_CFG_ENABLE_ID) */
/**
 * @brief Set one U8 parameter.
 * @param par_num Parameter number.
 * @param val U8 value to write.
 * @return Operation status.
 */
par_status_t par_set_u8(const par_num_t par_num, const uint8_t val);
/**
 * @brief Set one I8 parameter.
 * @param par_num Parameter number.
 * @param val I8 value to write.
 * @return Operation status.
 */
par_status_t par_set_i8(const par_num_t par_num, const int8_t val);
/**
 * @brief Set one U16 parameter.
 * @param par_num Parameter number.
 * @param val U16 value to write.
 * @return Operation status.
 */
par_status_t par_set_u16(const par_num_t par_num, const uint16_t val);
/**
 * @brief Set one I16 parameter.
 * @param par_num Parameter number.
 * @param val I16 value to write.
 * @return Operation status.
 */
par_status_t par_set_i16(const par_num_t par_num, const int16_t val);
/**
 * @brief Set one U32 parameter.
 * @param par_num Parameter number.
 * @param val U32 value to write.
 * @return Operation status.
 */
par_status_t par_set_u32(const par_num_t par_num, const uint32_t val);
/**
 * @brief Set one I32 parameter.
 * @param par_num Parameter number.
 * @param val I32 value to write.
 * @return Operation status.
 */
par_status_t par_set_i32(const par_num_t par_num, const int32_t val);
#if (1 == PAR_CFG_ENABLE_TYPE_F32)
/**
 * @brief Set one F32 parameter.
 * @param par_num Parameter number.
 * @param val F32 value to write.
 * @return Operation status.
 */
par_status_t par_set_f32(const par_num_t par_num, const float32_t val);
#endif /* (1 == PAR_CFG_ENABLE_TYPE_F32) */
/**
 * @brief Fast typed setters.
 *
 * @note These APIs are performance-oriented public entry points.
 * Callers must guarantee the module is initialized, par_num is valid,.
 * and the selected typed API matches the parameter type.
 *
 * @note They intentionally bypass access enforcement, runtime validation.
 * callbacks, and on-change callbacks. Range limiting still follows.
 * build-time PAR_CFG_ENABLE_RANGE.
 */
/**
 * @brief Set one U8 parameter through the unchecked fast path.
 * @param par_num Parameter number.
 * @param val U8 value to write.
 * @return Operation status.
 */
par_status_t par_set_u8_fast(const par_num_t par_num, const uint8_t val);
/**
 * @brief Set one I8 parameter through the unchecked fast path.
 * @param par_num Parameter number.
 * @param val I8 value to write.
 * @return Operation status.
 */
par_status_t par_set_i8_fast(const par_num_t par_num, const int8_t val);
/**
 * @brief Set one U16 parameter through the unchecked fast path.
 * @param par_num Parameter number.
 * @param val U16 value to write.
 * @return Operation status.
 */
par_status_t par_set_u16_fast(const par_num_t par_num, const uint16_t val);
/**
 * @brief Set one I16 parameter through the unchecked fast path.
 * @param par_num Parameter number.
 * @param val I16 value to write.
 * @return Operation status.
 */
par_status_t par_set_i16_fast(const par_num_t par_num, const int16_t val);
/**
 * @brief Set one U32 parameter through the unchecked fast path.
 * @param par_num Parameter number.
 * @param val U32 value to write.
 * @return Operation status.
 */
par_status_t par_set_u32_fast(const par_num_t par_num, const uint32_t val);
/**
 * @brief Set one I32 parameter through the unchecked fast path.
 * @param par_num Parameter number.
 * @param val I32 value to write.
 * @return Operation status.
 */
par_status_t par_set_i32_fast(const par_num_t par_num, const int32_t val);
#if (1 == PAR_CFG_ENABLE_TYPE_F32)
/**
 * @brief Set one F32 parameter through the unchecked fast path.
 * @param par_num Parameter number.
 * @param val F32 value to write.
 * @return Operation status.
 */
par_status_t par_set_f32_fast(const par_num_t par_num, const float32_t val);
#endif /* (1 == PAR_CFG_ENABLE_TYPE_F32) */
/**
 * @brief Fast bitwise setters for flags/bitmask parameters.
 *
 * @note These APIs are intended only for U8/U16/U32 parameters that model.
 * flags or bitmasks. They are not a general-purpose replacement for the.
 * normal setter APIs used with ranged numeric values.
 *
 * @note They follow the same trust model as other fast setters: callers must.
 * guarantee the module is initialized, par_num is valid, and the typed.
 * API matches the parameter type.
 *
 * @note They intentionally bypass runtime validation callbacks, on-change.
 * callbacks, and normal setter range semantics.
 */
/**
 * @brief Apply a fast bitwise AND update to one U8 parameter.
 * @param par_num Parameter number.
 * @param val U8 mask value.
 * @return Operation status.
 */
par_status_t par_bitand_set_u8_fast(const par_num_t par_num, const uint8_t val);
/**
 * @brief Apply a fast bitwise AND update to one U16 parameter.
 * @param par_num Parameter number.
 * @param val U16 mask value.
 * @return Operation status.
 */
par_status_t par_bitand_set_u16_fast(const par_num_t par_num, const uint16_t val);
/**
 * @brief Apply a fast bitwise AND update to one U32 parameter.
 * @param par_num Parameter number.
 * @param val U32 mask value.
 * @return Operation status.
 */
par_status_t par_bitand_set_u32_fast(const par_num_t par_num, const uint32_t val);
/**
 * @brief Apply a fast bitwise OR update to one U8 parameter.
 * @param par_num Parameter number.
 * @param val U8 mask value.
 * @return Operation status.
 */
par_status_t par_bitor_set_u8_fast(const par_num_t par_num, const uint8_t val);
/**
 * @brief Apply a fast bitwise OR update to one U16 parameter.
 * @param par_num Parameter number.
 * @param val U16 mask value.
 * @return Operation status.
 */
par_status_t par_bitor_set_u16_fast(const par_num_t par_num, const uint16_t val);
/**
 * @brief Apply a fast bitwise OR update to one U32 parameter.
 * @param par_num Parameter number.
 * @param val U32 mask value.
 * @return Operation status.
 */
par_status_t par_bitor_set_u32_fast(const par_num_t par_num, const uint32_t val);
/**
 * @brief Restore one parameter to its configured default value.
 * @param par_num Parameter number.
 * @return Operation status.
 * @note Default restore is an internal recovery/maintenance path and does not
 * enforce external write access or role-policy checks. It also bypasses runtime
 * validation callbacks and on-change callbacks. Range limiting still follows the
 * typed fast setter behavior for scalar rows.
 */
par_status_t par_set_to_default(const par_num_t par_num);
/**
 * @brief Restore all parameters to their configured default values.
 * @return Operation status.
 * @note Bulk default restore is an internal recovery/maintenance path and does
 * not enforce external write access or role-policy checks. When raw reset is
 * disabled, this API iterates through par_set_to_default().
 */
par_status_t par_set_all_to_default(void);
#if (1 == PAR_CFG_ENABLE_RESET_ALL_RAW)
/**
 * @brief Restore all parameters through the raw grouped-storage path.
 * @return Operation status.
 * @note Raw default restore bypasses normal setter hooks, external write access,
 * role-policy checks, validation callbacks, on-change callbacks, and setter-side
 * range behavior.
 */
par_status_t par_reset_all_to_default_raw(void);
#endif /* (1 == PAR_CFG_ENABLE_RESET_ALL_RAW) */

/**
 * @brief Report whether one parameter differs from its default value.
 * @param par_num Parameter number.
 * @param p_has_changed Pointer to the changed-state output flag.
 * @return Operation status.
 */
par_status_t par_has_changed(const par_num_t par_num, bool * const p_has_changed);
/**
 * @brief Typed macro wrappers for parameter set.
 */
#define PAR_SET_U8(par_num, value)  par_set_u8((par_num), (value))
#define PAR_SET_I8(par_num, value)  par_set_i8((par_num), (value))
#define PAR_SET_U16(par_num, value) par_set_u16((par_num), (value))
#define PAR_SET_I16(par_num, value) par_set_i16((par_num), (value))
#define PAR_SET_U32(par_num, value) par_set_u32((par_num), (value))
#define PAR_SET_I32(par_num, value) par_set_i32((par_num), (value))
#if (1 == PAR_CFG_ENABLE_TYPE_F32)
#define PAR_SET_F32(par_num, value) par_set_f32((par_num), (value))
#endif /* (1 == PAR_CFG_ENABLE_TYPE_F32) */

/**
 * @brief Getting parameter value API (module must be first initialized before using those func).
 */
/**
 * @brief Read one scalar parameter into a typed output pointer.
 * @details Object-backed rows do not use this generic getter. Use the dedicated
 * object typed getters instead.
 * @param par_num Parameter number.
 * @param p_val Pointer to the output value.
 * @return Operation status. Returns ePAR_ERROR_TYPE for object rows or ePAR_ERROR_ACCESS when read access is denied.
 */
par_status_t par_get(const par_num_t par_num, void * const p_val);
#if (1 == PAR_CFG_ENABLE_ID)
/**
 * @brief Read one scalar parameter by external parameter ID.
 * @details Object-backed rows do not use the generic ID-based getter. Use the
 * dedicated object typed getters after resolving the parameter number in the
 * application layer.
 * @param id External parameter ID.
 * @param p_val Pointer to the output value.
 * @return Operation status. Returns ePAR_ERROR_TYPE for object rows or ePAR_ERROR_ACCESS when read access is denied.
 */
par_status_t par_get_by_id(const uint16_t id, void * const p_val);
#endif /* (1 == PAR_CFG_ENABLE_ID) */
/**
 * @brief Read one U8 parameter.
 * @param par_num Parameter number.
 * @param p_val Pointer to the U8 output value.
 * @return Operation status. Returns ePAR_ERROR_ACCESS when read access is denied.
 */
par_status_t par_get_u8(const par_num_t par_num, uint8_t * const p_val);
/**
 * @brief Read one I8 parameter.
 * @param par_num Parameter number.
 * @param p_val Pointer to the I8 output value.
 * @return Operation status. Returns ePAR_ERROR_ACCESS when read access is denied.
 */
par_status_t par_get_i8(const par_num_t par_num, int8_t * const p_val);
/**
 * @brief Read one U16 parameter.
 * @param par_num Parameter number.
 * @param p_val Pointer to the U16 output value.
 * @return Operation status. Returns ePAR_ERROR_ACCESS when read access is denied.
 */
par_status_t par_get_u16(const par_num_t par_num, uint16_t * const p_val);
/**
 * @brief Read one I16 parameter.
 * @param par_num Parameter number.
 * @param p_val Pointer to the I16 output value.
 * @return Operation status. Returns ePAR_ERROR_ACCESS when read access is denied.
 */
par_status_t par_get_i16(const par_num_t par_num, int16_t * const p_val);
/**
 * @brief Read one U32 parameter.
 * @param par_num Parameter number.
 * @param p_val Pointer to the U32 output value.
 * @return Operation status. Returns ePAR_ERROR_ACCESS when read access is denied.
 */
par_status_t par_get_u32(const par_num_t par_num, uint32_t * const p_val);
/**
 * @brief Read one I32 parameter.
 * @param par_num Parameter number.
 * @param p_val Pointer to the I32 output value.
 * @return Operation status. Returns ePAR_ERROR_ACCESS when read access is denied.
 */
par_status_t par_get_i32(const par_num_t par_num, int32_t * const p_val);
#if (1 == PAR_CFG_ENABLE_TYPE_F32)
/**
 * @brief Read one F32 parameter.
 * @param par_num Parameter number.
 * @param p_val Pointer to the F32 output value.
 * @return Operation status. Returns ePAR_ERROR_ACCESS when read access is denied.
 */
par_status_t par_get_f32(const par_num_t par_num, float32_t * const p_val);
#endif /* (1 == PAR_CFG_ENABLE_TYPE_F32) */
/**
 * @brief Read the configured default value for one parameter.
 * @param par_num Parameter number.
 * @param p_val Pointer to the output value.
 * @return Operation status.
 */
par_status_t par_get_default(const par_num_t par_num, void * const p_val);
/**
 * @brief Parameter configurations API (usage without module init pre-step).
 */
const par_cfg_t *par_get_config(const par_num_t par_num);
#if (1 == PAR_CFG_ENABLE_NAME)
const char *par_get_name(const par_num_t par_num);
#endif /* (1 == PAR_CFG_ENABLE_NAME) */
#if (1 == PAR_CFG_ENABLE_RANGE)
/**
 * @brief Return the configured range for one parameter.
 * @param par_num Parameter number.
 * @return Configured parameter range.
 */
par_range_t par_get_range(const par_num_t par_num);
#endif /* (1 == PAR_CFG_ENABLE_RANGE) */
#if (1 == PAR_CFG_ENABLE_UNIT)
const char *par_get_unit(const par_num_t par_num);
#endif /* (1 == PAR_CFG_ENABLE_UNIT) */
#if (1 == PAR_CFG_ENABLE_DESC)
const char *par_get_desc(const par_num_t par_num);
#endif /* (1 == PAR_CFG_ENABLE_DESC) */
/**
 * @brief Return the configured data type for one parameter.
 * @param par_num Parameter number.
 * @return Parameter data type.
 */
par_type_list_t par_get_type(const par_num_t par_num);
#if (1 == PAR_CFG_ENABLE_ACCESS)
/**
 * @brief Return the configured external read/write capability mask for one parameter.
 * @param par_num Parameter number.
 * @return Configured external capability mask.
 */
par_access_t par_get_access(const par_num_t par_num);
#endif /* (1 == PAR_CFG_ENABLE_ACCESS) */
#if (1 == PAR_CFG_ENABLE_ROLE_POLICY)
/**
 * @brief Return the configured external read-role mask for one parameter.
 * @param par_num Parameter number.
 * @return Configured read-role mask.
 */
par_role_t par_get_read_roles(const par_num_t par_num);
/**
 * @brief Return the configured external write-role mask for one parameter.
 * @param par_num Parameter number.
 * @return Configured write-role mask.
 */
par_role_t par_get_write_roles(const par_num_t par_num);
/**
 * @brief Test whether a caller role mask may read one parameter.
 * @param par_num Parameter number.
 * @param roles Caller role mask.
 * @return True when read is allowed; otherwise false.
 */
bool par_can_read(const par_num_t par_num, const par_role_t roles);
/**
 * @brief Test whether a caller role mask may write one parameter.
 * @param par_num Parameter number.
 * @param roles Caller role mask.
 * @return True when write is allowed; otherwise false.
 */
bool par_can_write(const par_num_t par_num, const par_role_t roles);
#endif /* (1 == PAR_CFG_ENABLE_ROLE_POLICY) */
#if (1 == PAR_CFG_NVM_EN)
/**
 * @brief Report whether one parameter is marked persistent.
 * @param par_num Parameter number.
 * @return True when the parameter is persistent; otherwise false.
 */
bool par_is_persistent(const par_num_t par_num);
#endif /* (1 == PAR_CFG_NVM_EN) */
#if (1 == PAR_CFG_ENABLE_ID)
/**
 * @brief Resolve an external parameter ID to an internal parameter number.
 * @param id External parameter ID.
 * @param p_par_num Pointer to the resolved parameter number.
 * @return Operation status.
 */
par_status_t par_get_num_by_id(const uint16_t id, par_num_t * const p_par_num);
/**
 * @brief Resolve an internal parameter number to an external parameter ID.
 * @param par_num Parameter number.
 * @param p_id Pointer to the resolved external parameter ID.
 * @return Operation status.
 */
par_status_t par_get_id_by_num(const par_num_t par_num, uint16_t * const p_id);
#endif /* (1 == PAR_CFG_ENABLE_ID) */

/**
 * @brief Parameter NVM storage API.
 */
#if (1 == PAR_CFG_NVM_EN)
/**
 * @brief Set one scalar parameter and persist it immediately when changed.
 * @details Object-backed rows do not use this scalar convenience API. Use
 * par_set_obj_n_save() for object payloads when object persistence is enabled.
 * @param par_num Parameter number.
 * @param p_val Pointer to the input scalar value.
 * @return Operation status. Returns ePAR_ERROR_TYPE for object rows.
 */
par_status_t par_set_scalar_n_save(const par_num_t par_num, const void *p_val);
#if (1 == PAR_CFG_NVM_OBJECT_EN) && (1 == PAR_CFG_OBJECT_TYPES_ENABLED)
/**
 * @brief Set one object parameter and persist it immediately when changed.
 * @details The payload length is expressed in bytes. For STR parameters, len
 * excludes the trailing '\0' and p_data must not contain embedded NUL bytes.
 * For ARR_U16 and ARR_U32 parameters, len must be aligned to the element size
 * configured for the row.
 * @param par_num Parameter number.
 * @param p_data Pointer to the input payload bytes, or NULL when len is zero.
 * @param len Input payload length in bytes.
 * @return Operation status. Returns ePAR_ERROR_TYPE for scalar rows.
 */
par_status_t par_set_obj_n_save(const par_num_t par_num,
                                const uint8_t *p_data,
                                const uint16_t len);
#endif /* (1 == PAR_CFG_NVM_OBJECT_EN) && (1 == PAR_CFG_OBJECT_TYPES_ENABLED) */
/**
 * @brief Persist all scalar and object persistent parameters.
 * @return Operation status.
 */
par_status_t par_save_all(void);
/**
 * @brief Persist one scalar or object parameter.
 * @param par_num Parameter number.
 * @return Operation status.
 */
par_status_t par_save(const par_num_t par_num);
#if (1 == PAR_CFG_ENABLE_ID)
/**
 * @brief Persist one scalar or object parameter by external parameter ID.
 * @param par_id External parameter ID.
 * @return Operation status.
 */
par_status_t par_save_by_id(const uint16_t par_id);
#endif /* (1 == PAR_CFG_ENABLE_ID) */
/**
 * @brief Rewrite the full managed parameter NVM area.
 * @return Operation status.
 */
par_status_t par_save_clean(void);
#endif /* (1 == PAR_CFG_NVM_EN) */

/**
 * @brief Registration API.
 */
#if (1 == PAR_CFG_ENABLE_CHANGE_CALLBACK)
/**
 * @brief Register a change callback for one parameter.
 * @param par_num Parameter number.
 * @param cb Change callback function pointer.
 * @note Registered callback is not used by startup default initialization,
 * raw restore paths, fast setters, or bitwise fast setters.
 * @note Register callbacks during single-threaded setup or under application
 * synchronization. Runtime registration is not serialized against callback
 * dispatch by the parameter module.
 */
void par_register_on_change_cb(const par_num_t par_num, const pf_par_on_change_cb_t cb);
#endif /* (1 == PAR_CFG_ENABLE_CHANGE_CALLBACK) */
#if (1 == PAR_CFG_ENABLE_RUNTIME_VALIDATION)
/**
 * @brief Register a validation callback for one scalar parameter.
 * @param par_num Parameter number.
 * @param validation Validation callback function pointer.
 */
void par_register_validation(const par_num_t par_num, const pf_par_validation_t validation);
#if (1 == PAR_CFG_OBJECT_TYPES_ENABLED)
/**
 * @brief Register an object validation callback for one object parameter.
 * @param par_num Parameter number.
 * @param validation Validation callback function pointer.
 */
void par_register_obj_validation(const par_num_t par_num, const pf_par_obj_validation_t validation);
#endif /* (1 == PAR_CFG_OBJECT_TYPES_ENABLED) */
#endif /* (1 == PAR_CFG_ENABLE_RUNTIME_VALIDATION) */
#if (1 == PAR_CFG_ENABLE_DESC) && (1 == PAR_CFG_ENABLE_DESC_CHECK)
/**
 * @brief Validate a description string in the port layer.
 * @param p_desc Pointer to the description string.
 * @return True when the description is valid; otherwise false.
 */
PAR_PORT_WEAK bool par_port_is_desc_valid(const char * const p_desc);
#endif /* (1 == PAR_CFG_ENABLE_DESC) && (1 == PAR_CFG_ENABLE_DESC_CHECK) */

#if defined(AUTOGEN_PM_USING_MSH_TOOL) && defined(RT_USING_FINSH)
/**
 * @brief Return the shell group name for a parameter.
 * @param par_num Parameter number.
 * @return Group name string, or RT_NULL when no group label is used.
 */
PAR_PORT_WEAK const char *par_port_get_shell_group(const par_num_t par_num);
#endif /* defined(AUTOGEN_PM_USING_MSH_TOOL) && defined(RT_USING_FINSH) */

#if (PAR_CFG_DEBUG_EN)
const char *par_get_status_str(const par_status_t status);
#endif /* (PAR_CFG_DEBUG_EN) */
/**
 * @} <!-- END GROUP -->
 */

#endif /* !defined(_PAR_H_) */
