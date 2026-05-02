/**
 * @file par_cfg.h
 * @brief Provide compile-time configuration for the parameter module.
 * @author Ziga Miklosic
 * @version 1.0
 * @date 2026-01-29
 *
 * @copyright Copyright (c) 2026 Ziga Miklosic. Distributed under the MIT license.
 *
 * @note :
 * @par Change Log:
 * Date       Version Author        Description
 * 2026-01-29 1.0     Ziga Miklosic first version
 */
/**
 * @addtogroup PAR_CFG
 * @{ <!-- BEGIN GROUP -->
 *
 * @brief Configuration for device parameters.
 */

#ifndef _PAR_CFG_H_
#define _PAR_CFG_H_
/**
 * @brief Include dependencies.
 */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * @brief USER CODE BEGIN...
 */

/**
 * @brief Platform adaptation bridge.
 *
 * @note Object-type feature switches may be overridden from
 * `par_cfg_port.h`; otherwise par_cfg.h keeps them enabled by default.
 *
 * @note This header is included unconditionally.
 * Integrator shall provide "par_cfg_port.h" in include path.
 * If no platform override is needed, create an empty stub header.
 * with include guard (for example in port/par_cfg_port.h).
 */
#include "par_cfg_port.h"

/**
 * @brief USER CODE END...
 */
/**
 * @brief Compile-time definitions.
 */
/**
 * @brief Include persisted-storage configuration defaults.
 *
 * @details Override NVM-related PAR_CFG_* macros from par_cfg_port.h before
 * this include, or keep the defaults provided by par_nvm_cfg.h.
 */
#include "nvm/par_nvm_cfg.h"

/**
 * @brief Enable diagnostic logging in the parameter module.
 *
 * @details Set to 1 while debugging or validating integration. Set to 0
 * for production builds that must avoid log formatting and output overhead.
 */
#ifndef PAR_CFG_DEBUG_EN
#define PAR_CFG_DEBUG_EN (1)
#ifndef DEBUG
#undef PAR_CFG_DEBUG_EN
#define PAR_CFG_DEBUG_EN (0)
#endif /* !defined(DEBUG) */
#endif /* !defined(PAR_CFG_DEBUG_EN) */

/**
 * @brief Enable PAR_ASSERT checks.
 *
 * @details Set to 1 during bring-up and validation. Set to 0 only when
 * the platform has a separate production fault-handling policy.
 */
#ifndef PAR_CFG_ASSERT_EN
#define PAR_CFG_ASSERT_EN (1)
#ifndef DEBUG
#undef PAR_CFG_ASSERT_EN
#define PAR_CFG_ASSERT_EN (0)
#endif /* !defined(DEBUG) */
#endif /* !defined(PAR_CFG_ASSERT_EN) */

/**
 * @brief Platform hook fallbacks.
 */
#ifndef PAR_PORT_ASSERT
#define PAR_PORT_ASSERT(x) \
    do                     \
    {                      \
        (void)(x);         \
    } while (0)
#endif /* !defined(PAR_PORT_ASSERT) */

#ifndef PAR_PORT_LOG_INFO
#define PAR_PORT_LOG_INFO(...) \
    do                         \
    {                          \
    } while (0)
#endif /* !defined(PAR_PORT_LOG_INFO) */

#ifndef PAR_PORT_LOG_DEBUG
#define PAR_PORT_LOG_DEBUG(...) \
    do                          \
    {                           \
    } while (0)
#endif /* !defined(PAR_PORT_LOG_DEBUG) */

#ifndef PAR_PORT_LOG_WARN
#define PAR_PORT_LOG_WARN(...) \
    do                         \
    {                          \
    } while (0)
#endif /* !defined(PAR_PORT_LOG_WARN) */

#ifndef PAR_PORT_LOG_ERROR
#define PAR_PORT_LOG_ERROR(...) \
    do                          \
    {                           \
    } while (0)
#endif /* !defined(PAR_PORT_LOG_ERROR) */

#ifndef PAR_PORT_STATIC_ASSERT
#define PAR_PORT_STATIC_ASSERT(name, expn) typedef char _static_assert_##name[(expn) ? 1 : -1]
#endif /* !defined(PAR_PORT_STATIC_ASSERT) */

/**
 * @brief Platform weak symbol macro.
 *
 * @note Integrator may override this macro (for example: RT_WEAK).
 */
#ifndef PAR_PORT_WEAK
#define PAR_PORT_WEAK __attribute__((weak))
#endif /* !defined(PAR_PORT_WEAK) */

/**
 * @brief Type alignment abstraction.
 *
 * @note Default uses C11 _Alignof. If unavailable, falls back to.
 * offsetof-based alignment calculation.
 * @note This abstraction is intended for ordinary object types and platform.
 * atomic-wrapper types, but the platform must guarantee the expression.
 * is valid for its custom atomic wrapper definitions.
 */
#ifndef PAR_ALIGNOF
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
#define PAR_ALIGNOF(type) _Alignof(type)
#else
#define PAR_ALIGNOF(type) offsetof(struct { char _par_align_c; type _par_align_t; }, _par_align_t)
#endif /* defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L) */
#endif /* !defined(PAR_ALIGNOF) */

/**
 * @brief Package compile-time assert.
 */
#define PAR_STATIC_ASSERT(name, expn) PAR_PORT_STATIC_ASSERT(name, expn);
/**
 * @brief Resolve log/assert routing mode before default macro emission.
 */
#if !defined(PAR_CFG_PORT_HOOK_EN)
#define PAR_CFG_USE_PORT_HOOKS (0)
#elif (1 == PAR_CFG_PORT_HOOK_EN)
#define PAR_CFG_USE_PORT_HOOKS (1)
#else
#define PAR_CFG_USE_PORT_HOOKS (0)
#endif /* !defined(PAR_CFG_PORT_HOOK_EN) */

/**
 * @brief Debug communication port macros.
 */
#if (0 == PAR_CFG_USE_PORT_HOOKS)
#if (1 == PAR_CFG_DEBUG_EN)
#ifndef PAR_CFG_DIRECT_LOG
#define PAR_CFG_DIRECT_LOG(...) (cli_printf((char *)__VA_ARGS__))
#endif /* !defined(PAR_CFG_DIRECT_LOG) */
#ifndef PAR_CFG_DIRECT_INFO_LOG
#define PAR_CFG_DIRECT_INFO_LOG(...) PAR_CFG_DIRECT_LOG(__VA_ARGS__)
#endif /* !defined(PAR_CFG_DIRECT_INFO_LOG) */
#ifndef PAR_CFG_DIRECT_DEBUG_LOG
#define PAR_CFG_DIRECT_DEBUG_LOG(...) PAR_CFG_DIRECT_LOG(__VA_ARGS__)
#endif /* !defined(PAR_CFG_DIRECT_DEBUG_LOG) */
#ifndef PAR_CFG_DIRECT_WARN_LOG
#define PAR_CFG_DIRECT_WARN_LOG(...) PAR_CFG_DIRECT_LOG(__VA_ARGS__)
#endif /* !defined(PAR_CFG_DIRECT_WARN_LOG) */
#ifndef PAR_CFG_DIRECT_ERROR_LOG
#define PAR_CFG_DIRECT_ERROR_LOG(...) PAR_CFG_DIRECT_LOG(__VA_ARGS__)
#endif /* !defined(PAR_CFG_DIRECT_ERROR_LOG) */
#define PAR_INFO_PRINT(...) PAR_CFG_DIRECT_INFO_LOG(__VA_ARGS__)
#define PAR_DBG_PRINT(...)  PAR_CFG_DIRECT_DEBUG_LOG(__VA_ARGS__)
#define PAR_WARN_PRINT(...) PAR_CFG_DIRECT_WARN_LOG(__VA_ARGS__)
#define PAR_ERR_PRINT(...)  PAR_CFG_DIRECT_ERROR_LOG(__VA_ARGS__)
#else
#define PAR_INFO_PRINT(...) \
    {                       \
        ;                   \
    }
#define PAR_DBG_PRINT(...) \
    {                      \
        ;                  \
    }
#define PAR_WARN_PRINT(...) \
    {                       \
        ;                   \
    }
#define PAR_ERR_PRINT(...) \
    {                      \
        ;                  \
    }
#endif /* (1 == PAR_CFG_DEBUG_EN) */
#else
#if (1 == PAR_CFG_DEBUG_EN)
#define PAR_INFO_PRINT(...) PAR_PORT_LOG_INFO(__VA_ARGS__)
#define PAR_DBG_PRINT(...)  PAR_PORT_LOG_DEBUG(__VA_ARGS__)
#define PAR_WARN_PRINT(...) PAR_PORT_LOG_WARN(__VA_ARGS__)
#define PAR_ERR_PRINT(...)  PAR_PORT_LOG_ERROR(__VA_ARGS__)
#else
#define PAR_INFO_PRINT(...) \
    {                       \
        ;                   \
    }
#define PAR_DBG_PRINT(...) \
    {                      \
        ;                  \
    }
#define PAR_WARN_PRINT(...) \
    {                       \
        ;                   \
    }
#define PAR_ERR_PRINT(...) \
    {                      \
        ;                  \
    }
#endif /* (1 == PAR_CFG_DEBUG_EN) */
#endif /* (0 == PAR_CFG_USE_PORT_HOOKS) */

/**
 * @brief Assertion macros.
 */
#if (0 == PAR_CFG_USE_PORT_HOOKS)
#if (1 == PAR_CFG_ASSERT_EN)
#ifndef PAR_CFG_DIRECT_ASSERT
#ifdef PROJ_CFG_ASSERT
#define PAR_CFG_DIRECT_ASSERT(x) PROJ_CFG_ASSERT(x)
#else
#define PAR_CFG_DIRECT_ASSERT(x) \
    do                           \
    {                            \
        (void)(x);               \
    } while (0)
#endif /* defined(PROJ_CFG_ASSERT) */
#endif /* !defined(PAR_CFG_DIRECT_ASSERT) */
#define PAR_ASSERT(x) PAR_CFG_DIRECT_ASSERT(x)
#else
#define PAR_ASSERT(x) \
    {                 \
        ;             \
    }
#endif /* (1 == PAR_CFG_ASSERT_EN) */
#else
#if (1 == PAR_CFG_ASSERT_EN)
#define PAR_ASSERT(x) PAR_PORT_ASSERT(x)
#else
#define PAR_ASSERT(x) \
    {                 \
        ;             \
    }
#endif /* (1 == PAR_CFG_ASSERT_EN) */
#endif /* (0 == PAR_CFG_USE_PORT_HOOKS) */

#undef PAR_CFG_USE_PORT_HOOKS

/**
 * @brief Invalid configuration catcher.
 *
 * @note Shall be intact by end user!
 */
#if (0 == PAR_CFG_NVM_EN) && (1 == PAR_CFG_TABLE_ID_CHECK_EN)
#error "Parameter settings invalid: Disable table ID checking (PAR_CFG_TABLE_ID_CHECK_EN)!"
#endif /* (0 == PAR_CFG_NVM_EN) && (1 == PAR_CFG_TABLE_ID_CHECK_EN) */

/**
 * @brief Enable parameter access serialization through mutex hooks.
 *
 * @details Set to 1 when parameters may be accessed from multiple tasks,
 * shell commands, callbacks, or interrupt-deferred contexts. Set to 0 only
 * for single-threaded integrations.
 */
#ifndef PAR_CFG_MUTEX_EN
#define PAR_CFG_MUTEX_EN (1)
#endif /* !defined(PAR_CFG_MUTEX_EN) */

/**
 * @brief Configure parameter mutex wait timeout.
 *
 * @details Unit: ms. Increase only when normal parameter operations are
 * expected to be blocked by longer application-side critical sections.
 */
#ifndef PAR_CFG_MUTEX_TIMEOUT_MS
#define PAR_CFG_MUTEX_TIMEOUT_MS (10)
#endif /* !defined(PAR_CFG_MUTEX_TIMEOUT_MS) */

/**
 * @brief Select the platform-specific par_if backend.
 *
 * @details Set to 1 when the BSP provides mutex, CRC, logging, or other
 * platform hooks through the port layer. Keep 0 for standalone fallback use.
 */
#ifndef PAR_CFG_IF_PORT_EN
#define PAR_CFG_IF_PORT_EN (0)
#endif /* !defined(PAR_CFG_IF_PORT_EN) */

/**
 * @brief Route logs and assertions through platform hooks.
 *
 * @details Set to 1 when PAR_PORT_LOG_* and PAR_PORT_ASSERT are provided by
 * the integration layer, such as RT-Thread logging and RT_ASSERT.
 */
#ifndef PAR_CFG_PORT_HOOK_EN
#define PAR_CFG_PORT_HOOK_EN (0)
#endif /* !defined(PAR_CFG_PORT_HOOK_EN) */

/**
 * @brief Parameter storage layout-source constants.
 *
 * @details COMPILE_SCAN derives offsets in code. SCRIPT uses a generated
 * static layout header when startup scanning must be avoided.
 */
#define PAR_CFG_LAYOUT_COMPILE_SCAN (0u)
#define PAR_CFG_LAYOUT_SCRIPT       (1u)

/**
 * @brief Select parameter storage layout source.
 *
 * @note
 * - COMPILE_SCAN: counts are compile-time constants, offsets are scanned in init.
 * - SCRIPT : counts/offsets are provided by generated static layout header.
 */
#ifndef PAR_CFG_LAYOUT_SOURCE
#define PAR_CFG_LAYOUT_SOURCE PAR_CFG_LAYOUT_COMPILE_SCAN
#endif /* !defined(PAR_CFG_LAYOUT_SOURCE) */

/**
 * @brief Static layout include path.
 *
 * @note Can be overridden by integrator to include generated layout header.
 */
#ifndef PAR_CFG_LAYOUT_STATIC_INCLUDE
#define PAR_CFG_LAYOUT_STATIC_INCLUDE "par_layout_static.h"
#endif /* !defined(PAR_CFG_LAYOUT_STATIC_INCLUDE) */

/**
 * @brief Enable floating-point scalar parameter support.
 *
 * @details Set to 1 when par_table.def uses F32 parameters. Set to 0 to
 * remove float storage and APIs from integer-only builds.
 */
#ifndef PAR_CFG_ENABLE_TYPE_F32
#define PAR_CFG_ENABLE_TYPE_F32 (1)
#endif /* !defined(PAR_CFG_ENABLE_TYPE_F32) */

/**
 * @brief Enable fixed-capacity string object parameter support.
 *
 * @details Set to 1 when par_table.def uses STR rows. When disabled, string
 * setters, getters, storage slots, and object persistence records are not compiled.
 */
#ifndef PAR_CFG_ENABLE_TYPE_STR
#ifdef PAR_CFG_OBJECT_TYPES_ENABLED
#define PAR_CFG_ENABLE_TYPE_STR (PAR_CFG_OBJECT_TYPES_ENABLED)
#else
#define PAR_CFG_ENABLE_TYPE_STR (1)
#endif /* defined(PAR_CFG_OBJECT_TYPES_ENABLED) */
#endif /* !defined(PAR_CFG_ENABLE_TYPE_STR) */
/**
 * @brief Enable raw byte object parameter support.
 *
 * @details Set to 1 when par_table.def uses BYTES rows for opaque binary
 * configuration payloads.
 */
#ifndef PAR_CFG_ENABLE_TYPE_BYTES
#ifdef PAR_CFG_OBJECT_TYPES_ENABLED
#define PAR_CFG_ENABLE_TYPE_BYTES (PAR_CFG_OBJECT_TYPES_ENABLED)
#else
#define PAR_CFG_ENABLE_TYPE_BYTES (1)
#endif /* defined(PAR_CFG_OBJECT_TYPES_ENABLED) */
#endif /* !defined(PAR_CFG_ENABLE_TYPE_BYTES) */
/**
 * @brief Enable uint8_t array object parameter support.
 *
 * @details Set to 1 when par_table.def uses ARR_U8 rows. Lengths are stored
 * and validated in bytes.
 */
#ifndef PAR_CFG_ENABLE_TYPE_ARR_U8
#ifdef PAR_CFG_OBJECT_TYPES_ENABLED
#define PAR_CFG_ENABLE_TYPE_ARR_U8 (PAR_CFG_OBJECT_TYPES_ENABLED)
#else
#define PAR_CFG_ENABLE_TYPE_ARR_U8 (1)
#endif /* defined(PAR_CFG_OBJECT_TYPES_ENABLED) */
#endif /* !defined(PAR_CFG_ENABLE_TYPE_ARR_U8) */
/**
 * @brief Enable uint16_t array object parameter support.
 *
 * @details Set to 1 when par_table.def uses ARR_U16 rows. Object lengths
 * must remain aligned to the uint16_t element size.
 */
#ifndef PAR_CFG_ENABLE_TYPE_ARR_U16
#ifdef PAR_CFG_OBJECT_TYPES_ENABLED
#define PAR_CFG_ENABLE_TYPE_ARR_U16 (PAR_CFG_OBJECT_TYPES_ENABLED)
#else
#define PAR_CFG_ENABLE_TYPE_ARR_U16 (1)
#endif /* defined(PAR_CFG_OBJECT_TYPES_ENABLED) */
#endif /* !defined(PAR_CFG_ENABLE_TYPE_ARR_U16) */
/**
 * @brief Enable uint32_t array object parameter support.
 *
 * @details Set to 1 when par_table.def uses ARR_U32 rows. Object lengths
 * must remain aligned to the uint32_t element size.
 */
#ifndef PAR_CFG_ENABLE_TYPE_ARR_U32
#ifdef PAR_CFG_OBJECT_TYPES_ENABLED
#define PAR_CFG_ENABLE_TYPE_ARR_U32 (PAR_CFG_OBJECT_TYPES_ENABLED)
#else
#define PAR_CFG_ENABLE_TYPE_ARR_U32 (1)
#endif /* defined(PAR_CFG_OBJECT_TYPES_ENABLED) */
#endif /* !defined(PAR_CFG_ENABLE_TYPE_ARR_U32) */
/**
 * @brief Enable the shared object-parameter framework.
 *
 * @details Usually derived from the individual object-type switches. Override
 * only when an integration deliberately wants all object types on or off.
 */
#ifndef PAR_CFG_OBJECT_TYPES_ENABLED
#if ((1 == PAR_CFG_ENABLE_TYPE_STR) || (1 == PAR_CFG_ENABLE_TYPE_BYTES) ||      \
     (1 == PAR_CFG_ENABLE_TYPE_ARR_U8) || (1 == PAR_CFG_ENABLE_TYPE_ARR_U16) || \
     (1 == PAR_CFG_ENABLE_TYPE_ARR_U32))
#define PAR_CFG_OBJECT_TYPES_ENABLED (1)
#else
#define PAR_CFG_OBJECT_TYPES_ENABLED (0)
#endif /* ((1 == PAR_CFG_ENABLE_TYPE_STR) || (1 == PAR_CFG_ENABLE_TYPE_BYTES) || (1 == PAR_CFG_ENABLE_TYPE_ARR_U8) || (1 == PAR_CFG_ENABLE_TYPE_ARR_U16) || (1 == PAR_CFG_ENABLE_TYPE_ARR_U32)) */
#endif /* !defined(PAR_CFG_OBJECT_TYPES_ENABLED) */

/**
 * @brief True when at least one object parameter type is enabled.
 */
#define PAR_CFG_ANY_OBJECT_TYPE_ENABLED                                         \
    ((1 == PAR_CFG_ENABLE_TYPE_STR) || (1 == PAR_CFG_ENABLE_TYPE_BYTES) ||      \
     (1 == PAR_CFG_ENABLE_TYPE_ARR_U8) || (1 == PAR_CFG_ENABLE_TYPE_ARR_U16) || \
     (1 == PAR_CFG_ENABLE_TYPE_ARR_U32))

PAR_STATIC_ASSERT(par_cfg_object_types_enabled_matches_type_switches,
                  (((1 == PAR_CFG_OBJECT_TYPES_ENABLED) && PAR_CFG_ANY_OBJECT_TYPE_ENABLED) ||
                   ((0 == PAR_CFG_OBJECT_TYPES_ENABLED) && (!PAR_CFG_ANY_OBJECT_TYPE_ENABLED))));

#undef PAR_CFG_ANY_OBJECT_TYPE_ENABLED

/**
 * @brief Enable/Disable runtime validation callbacks in normal setters.
 */
#ifndef PAR_CFG_ENABLE_RUNTIME_VALIDATION
#define PAR_CFG_ENABLE_RUNTIME_VALIDATION (1)
#endif /* !defined(PAR_CFG_ENABLE_RUNTIME_VALIDATION) */

/**
 * @brief Enable/Disable on-change callbacks in normal setters.
 */
#ifndef PAR_CFG_ENABLE_CHANGE_CALLBACK
#define PAR_CFG_ENABLE_CHANGE_CALLBACK (1)
#endif /* !defined(PAR_CFG_ENABLE_CHANGE_CALLBACK) */

/**
 * @brief Enable/Disable raw reset-all API and default mirror storage.
 *
 * @note When enabled, module compiles par_reset_all_to_default_raw() and.
 * keeps a grouped default mirror snapshot for raw restore.
 *
 * @note The speedup comes from bypassing per-parameter setter-side logic such.
 * as runtime validation, change callback, and range handling.
 *
 * @note The raw reset path restores parameter storage directly from that.
 * grouped default mirror snapshot, so it is typically faster than.
 * par_set_all_to_default(), which resets parameters one by one through.
 * the normal runtime setter path.
 */
#ifndef PAR_CFG_ENABLE_RESET_ALL_RAW
#define PAR_CFG_ENABLE_RESET_ALL_RAW (1)
#endif /* !defined(PAR_CFG_ENABLE_RESET_ALL_RAW) */

/**
 * @brief Enable/Disable parameter range metadata (min/max).
 */
#ifndef PAR_CFG_ENABLE_RANGE
#define PAR_CFG_ENABLE_RANGE (1)
#endif /* !defined(PAR_CFG_ENABLE_RANGE) */

/**
 * @brief Enable/Disable parameter name metadata.
 */
#ifndef PAR_CFG_ENABLE_NAME
#define PAR_CFG_ENABLE_NAME (1)
#endif /* !defined(PAR_CFG_ENABLE_NAME) */

/**
 * @brief Enable/Disable parameter unit metadata.
 */
#ifndef PAR_CFG_ENABLE_UNIT
#define PAR_CFG_ENABLE_UNIT (1)
#endif /* !defined(PAR_CFG_ENABLE_UNIT) */

/**
 * @brief Enable/Disable parameter description metadata.
 */
#ifndef PAR_CFG_ENABLE_DESC
#define PAR_CFG_ENABLE_DESC (1)
#endif /* !defined(PAR_CFG_ENABLE_DESC) */

/**
 * @brief Enable/Disable parameter ID metadata.
 */
#ifndef PAR_CFG_ENABLE_ID
#define PAR_CFG_ENABLE_ID (1)
#endif /* !defined(PAR_CFG_ENABLE_ID) */

#include "def/par_def.h"


/**
 * @brief Enable/Disable parameter access metadata.
 */
#ifndef PAR_CFG_ENABLE_ACCESS
#define PAR_CFG_ENABLE_ACCESS (1)
#endif /* !defined(PAR_CFG_ENABLE_ACCESS) */

/**
 * @brief Enable/Disable optional external role-policy metadata and enforcement.
 */
#ifndef PAR_CFG_ENABLE_ROLE_POLICY
#define PAR_CFG_ENABLE_ROLE_POLICY (0)
#endif /* !defined(PAR_CFG_ENABLE_ROLE_POLICY) */

/**
 * @brief Enable/Disable description check.
 *
 * @note Default follows PAR_CFG_ENABLE_DESC.
 */
#ifndef PAR_CFG_ENABLE_DESC_CHECK
#define PAR_CFG_ENABLE_DESC_CHECK (PAR_CFG_ENABLE_DESC)
#endif /* !defined(PAR_CFG_ENABLE_DESC_CHECK) */

/**
 * @brief Configuration dependency checks for optional fields/features.
 */
#if (1 == PAR_CFG_NVM_EN) && (1 == PAR_CFG_NVM_SCALAR_EN) &&                       \
    (0 == PAR_CFG_ENABLE_ID) &&                                                    \
    (PAR_CFG_NVM_RECORD_LAYOUT != PAR_CFG_NVM_RECORD_LAYOUT_FIXED_PAYLOAD_ONLY) && \
    (PAR_CFG_NVM_RECORD_LAYOUT != PAR_CFG_NVM_RECORD_LAYOUT_GROUPED_PAYLOAD_ONLY)
#error "Parameter settings invalid: selected NVM layout requires PAR_CFG_ENABLE_ID = 1!"
#endif /* (1 == PAR_CFG_NVM_EN) && (1 == PAR_CFG_NVM_SCALAR_EN) && (0 == PAR_CFG_ENABLE_ID) && (PAR_CFG_NVM_RECORD_LAYOUT != PAR_CFG_NVM_RECORD_LAYOUT_FIXED_PAYLOAD_ONLY) && (PAR_CFG_NVM_RECORD_LAYOUT != PAR_CFG_NVM_RECORD_LAYOUT_GROUPED_PAYLOAD_ONLY) */

#if (1 == PAR_CFG_NVM_EN) && (1 == PAR_CFG_NVM_SCALAR_EN) &&                           \
    ((PAR_CFG_NVM_RECORD_LAYOUT == PAR_CFG_NVM_RECORD_LAYOUT_FIXED_PAYLOAD_ONLY) ||    \
     (PAR_CFG_NVM_RECORD_LAYOUT == PAR_CFG_NVM_RECORD_LAYOUT_GROUPED_PAYLOAD_ONLY)) && \
    (0 == PAR_CFG_TABLE_ID_CHECK_EN)
#error "Parameter settings invalid: payload-only NVM layouts require PAR_CFG_TABLE_ID_CHECK_EN = 1!"
#endif /* (1 == PAR_CFG_NVM_EN) && (1 == PAR_CFG_NVM_SCALAR_EN) && ((PAR_CFG_NVM_RECORD_LAYOUT == PAR_CFG_NVM_RECORD_LAYOUT_FIXED_PAYLOAD_ONLY) || (PAR_CFG_NVM_RECORD_LAYOUT == PAR_CFG_NVM_RECORD_LAYOUT_GROUPED_PAYLOAD_ONLY)) && (0 == PAR_CFG_TABLE_ID_CHECK_EN) */

#if (1 == PAR_CFG_NVM_SCALAR_EN) && (0 == PAR_CFG_NVM_EN)
#error "Parameter settings invalid: scalar persistence requires PAR_CFG_NVM_EN = 1!"
#endif /* (1 == PAR_CFG_NVM_SCALAR_EN) && (0 == PAR_CFG_NVM_EN) */

#if (1 == PAR_CFG_NVM_EN) && (0 == PAR_CFG_NVM_SCALAR_EN) && \
    (0 == PAR_CFG_NVM_OBJECT_EN)
#error "Parameter settings invalid: NVM requires scalar or object persistence to be enabled!"
#endif /* (1 == PAR_CFG_NVM_EN) && (0 == PAR_CFG_NVM_SCALAR_EN) && (0 == PAR_CFG_NVM_OBJECT_EN) */

#if (1 == PAR_CFG_NVM_OBJECT_EN) && (0 == PAR_CFG_NVM_EN)
#error "Parameter settings invalid: object persistence requires PAR_CFG_NVM_EN = 1!"
#endif /* (1 == PAR_CFG_NVM_OBJECT_EN) && (0 == PAR_CFG_NVM_EN) */

#if (1 == PAR_CFG_NVM_OBJECT_EN) && (0 == PAR_CFG_OBJECT_TYPES_ENABLED)
#error "Parameter settings invalid: object persistence requires at least one object type enabled!"
#endif /* (1 == PAR_CFG_NVM_OBJECT_EN) && (0 == PAR_CFG_OBJECT_TYPES_ENABLED) */

#if (1 == PAR_CFG_NVM_OBJECT_EN) && (0 == PAR_CFG_ENABLE_ID)
#error "Parameter settings invalid: object persistence requires PAR_CFG_ENABLE_ID = 1!"
#endif /* (1 == PAR_CFG_NVM_OBJECT_EN) && (0 == PAR_CFG_ENABLE_ID) */

/**
 * @brief Reject removed object persistence migration configuration.
 *
 * @details Generic object block migration is intentionally unsupported because
 * flash-safe relocation requires product-specific transaction storage,
 * power-loss recovery, and erase-block planning. Use fixed or dedicated object
 * placement for released products that must keep object values stable.
 */
#if defined(PAR_CFG_NVM_OBJECT_MIGRATION_EN) || \
    defined(PAR_CFG_NVM_OBJECT_MIGRATION_SRC_ADDR)
#error "Object persistence migration is not supported; use fixed or dedicated placement!"
#endif /* defined(PAR_CFG_NVM_OBJECT_MIGRATION_EN) || defined(PAR_CFG_NVM_OBJECT_MIGRATION_SRC_ADDR) */

#if (1 == PAR_CFG_NVM_OBJECT_EN) &&                                       \
    (PAR_CFG_NVM_OBJECT_STORE_MODE != PAR_CFG_NVM_OBJECT_STORE_SHARED) && \
    (PAR_CFG_NVM_OBJECT_STORE_MODE != PAR_CFG_NVM_OBJECT_STORE_DEDICATED)
#error "Parameter settings invalid: PAR_CFG_NVM_OBJECT_STORE_MODE must be SHARED or DEDICATED!"
#endif /* (1 == PAR_CFG_NVM_OBJECT_EN) && (PAR_CFG_NVM_OBJECT_STORE_MODE != PAR_CFG_NVM_OBJECT_STORE_SHARED) && (PAR_CFG_NVM_OBJECT_STORE_MODE != PAR_CFG_NVM_OBJECT_STORE_DEDICATED) */

#if (1 == PAR_CFG_NVM_OBJECT_EN) &&                                           \
    (PAR_CFG_NVM_OBJECT_STORE_MODE == PAR_CFG_NVM_OBJECT_STORE_SHARED) &&     \
    (PAR_CFG_NVM_OBJECT_ADDR_MODE != PAR_CFG_NVM_OBJECT_ADDR_AFTER_SCALAR) && \
    (PAR_CFG_NVM_OBJECT_ADDR_MODE != PAR_CFG_NVM_OBJECT_ADDR_FIXED)
#error "Parameter settings invalid: PAR_CFG_NVM_OBJECT_ADDR_MODE must be AFTER_SCALAR or FIXED!"
#endif /* (1 == PAR_CFG_NVM_OBJECT_EN) && (PAR_CFG_NVM_OBJECT_STORE_MODE == PAR_CFG_NVM_OBJECT_STORE_SHARED) && (PAR_CFG_NVM_OBJECT_ADDR_MODE != PAR_CFG_NVM_OBJECT_ADDR_AFTER_SCALAR) && (PAR_CFG_NVM_OBJECT_ADDR_MODE != PAR_CFG_NVM_OBJECT_ADDR_FIXED) */

#if (1 == PAR_CFG_NVM_OBJECT_EN) &&                                       \
    (PAR_CFG_NVM_OBJECT_STORE_MODE == PAR_CFG_NVM_OBJECT_STORE_SHARED) && \
    (PAR_CFG_NVM_OBJECT_ADDR_MODE == PAR_CFG_NVM_OBJECT_ADDR_FIXED) &&    \
    (0U == PAR_CFG_NVM_OBJECT_FIXED_ADDR)
#error "Parameter settings invalid: fixed object persistence mode requires PAR_CFG_NVM_OBJECT_FIXED_ADDR != 0!"
#endif /* (1 == PAR_CFG_NVM_OBJECT_EN) && (PAR_CFG_NVM_OBJECT_STORE_MODE == PAR_CFG_NVM_OBJECT_STORE_SHARED) && (PAR_CFG_NVM_OBJECT_ADDR_MODE == PAR_CFG_NVM_OBJECT_ADDR_FIXED) && (0U == PAR_CFG_NVM_OBJECT_FIXED_ADDR) */

#if (0 == PAR_CFG_ENABLE_ID) && (1 == PAR_CFG_ENABLE_RUNTIME_ID_DUP_CHECK)
#error "Parameter settings invalid: runtime duplicate-ID diagnostics require PAR_CFG_ENABLE_ID = 1!"
#endif /* (0 == PAR_CFG_ENABLE_ID) && (1 == PAR_CFG_ENABLE_RUNTIME_ID_DUP_CHECK) */

#if (0 == PAR_CFG_ENABLE_ID) && (1 == PAR_CFG_ENABLE_RUNTIME_ID_HASH_COLLISION_CHECK)
#error "Parameter settings invalid: runtime ID hash-collision diagnostics require PAR_CFG_ENABLE_ID = 1!"
#endif /* (0 == PAR_CFG_ENABLE_ID) && (1 == PAR_CFG_ENABLE_RUNTIME_ID_HASH_COLLISION_CHECK) */

#if (PAR_CFG_LAYOUT_SOURCE != PAR_CFG_LAYOUT_COMPILE_SCAN) && (PAR_CFG_LAYOUT_SOURCE != PAR_CFG_LAYOUT_SCRIPT)
#error "Parameter settings invalid: PAR_CFG_LAYOUT_SOURCE must be PAR_CFG_LAYOUT_COMPILE_SCAN or PAR_CFG_LAYOUT_SCRIPT!"
#endif /* (PAR_CFG_LAYOUT_SOURCE != PAR_CFG_LAYOUT_COMPILE_SCAN) && (PAR_CFG_LAYOUT_SOURCE != PAR_CFG_LAYOUT_SCRIPT) */

#define PAR_UINT16_MAX (65535u)
/**
 * @brief Function declarations.
 */
const par_cfg_t *par_cfg_get_table(void);
const par_cfg_t *par_cfg_get(const par_num_t par_num);
/**
 * @brief Return the number of configuration entries.
 * @return Configuration table size.
 */
uint32_t par_cfg_get_table_size(void); /**< @} <!-- END GROUP --> */

#endif /* !defined(_PAR_CFG_H_) */
