# Device Parameters

`Device Parameters` is a portable embedded C module for managing runtime parameters, validation, metadata, and optional NVM persistence from a single parameter definition table.

It is designed for projects that need a clean way to:

- define parameters once
- read and write them through a consistent API
- expose them to CLI, PC tools, or protocol layers by stable external IDs
- validate values before they are accepted
- persist selected values to NVM
- keep memory usage predictable on embedded targets

## What this module provides

- **Single source of truth** through `par_table.def`
- **Typed APIs** for `U8`, `I8`, `U16`, `I16`, `U32`, `I32`, and, when enabled, `F32`
- **Fixed-capacity object APIs** for strings, raw bytes, and fixed-size unsigned arrays
- **Optional metadata** such as name, unit, description, access, ID, persistence flags, and optional role-based read/write visibility metadata
- **Validation pipeline** with compile-time checks for integer ranges and optional runtime hooks for dynamic rules
- **Static live-value storage** grouped by width instead of heap allocation
- **Fast external lookup by ID** through a compile-time generated static hash map
- **Optional NVM integration** for persistent parameters
- **Portable core + platform hooks** for RTOS, mutex, logging, assertions, and atomic backends

## Start here

### Quick start

1. Add `src/par.c` and the sources from `src/def`, `src/layout`, `src/scalar`, `src/object`, `src/nvm`, and `src/port` to your build. Add `src/nvm/backend` only when scalar persistence or shared object persistence uses a packaged scalar backend adapter.
2. Provide a project-specific `par_table.def` at the package root.
3. Provide `port/par_cfg_port.h` in your include path.
4. Optionally provide `port/par_if_port.c` and `port/par_atomic_port.h` when your platform needs them.
5. Call `par_init()` before using runtime APIs.
6. Use the scalar typed `par_set_*` / `par_get_*` APIs for scalar rows and the dedicated object typed APIs (`par_set_str()` / `par_get_str()`, `par_set_bytes()` / `par_get_bytes()`, `par_set_arr_*()` / `par_get_arr_*()`) for object rows. Getter APIs use an explicit output pointer and return `par_status_t`. When `PAR_CFG_ENABLE_ACCESS = 1`, parameter table rows are limited to `ePAR_ACCESS_RO` or `ePAR_ACCESS_RW`; `ePAR_ACCESS_NONE` and write-only masks are rejected at build time.

A minimal example:

This quick-start example assumes `PAR_CFG_ENABLE_TYPE_F32 = 1`.

```c
#include "par.h"

static void app_init(void)
{
    if (par_init() != ePAR_OK)
    {
        /* Handle initialization error */
    }

    (void)par_set_f32(ePAR_CH1_REF_VAL, (float32_t)25.0f);

    float32_t ref_val = 0.0f;
    if (par_get_f32(ePAR_CH1_REF_VAL, &ref_val) != ePAR_OK)
    {
        /* Handle read error */
    }
}
```


## Optional role-policy metadata

When `PAR_CFG_ENABLE_ROLE_POLICY = 1`, each row in `par_table.def` also carries:

- `read_roles_`
- `write_roles_`

These fields are **metadata and helper-policy inputs**, not a built-in login/session system.
The package exposes `par_can_read()` / `par_can_write()` so the integration layer
(such as msh, CLI, RPC, or diagnostic service code) can decide which caller roles
are currently active and enforce them consistently.

Checked public getters and setters consume the access capability bits directly. Role masks are still evaluated only when the integration calls `par_can_read()` / `par_can_write()` or wraps them in a higher-level public interface such as the packaged RT-Thread shell port. Metadata getters and `par_get_default()` continue to read the configuration table directly and therefore do not consume runtime role-policy metadata.

Default restore APIs are maintenance/recovery paths rather than normal external writes. `par_set_to_default()`, `par_set_all_to_default()`, `par_reset_all_to_default_raw()`, and the packaged shell `par def` / `par def_all` commands intentionally bypass external write-access and role-policy checks.

`msh info` and `msh json` also show the configured role metadata when this option is enabled.

## Object parameter notes

When object types are enabled, the package also supports fixed-capacity strings, raw byte arrays, and fixed-size unsigned arrays through the shared object framework.

The bundled `par_table.def` treats its object rows as optional examples and guards them with the matching object-type switch, so disabled sample rows are compiled out.

- live payload bytes are stored in one shared object pool
- each object parameter owns one runtime slot with pool offset, current length, and capacity
- `STR` capacity is stored payload capacity only and does not include the trailing NUL used by `par_get_str()` output buffers; `STR` max length is limited to `UINT16_MAX - 1U`
- object rows remain RAM-only by default, but `pers_ = 1` is supported when `PAR_CFG_NVM_OBJECT_EN = 1` and `PAR_CFG_ENABLE_ID = 1`
- script layout mode requires generated object-slot counts and object-pool offset tables only when object rows are present; generated object-pool offsets are still checked at init for contiguous, parameter-table-order packing
- firmware-level object payload read/write uses only the dedicated object typed APIs; generic `par_get()` / `par_set()` and ID-based `par_get_by_id()` / `par_set_by_id()` remain scalar-only
- the packaged RT-Thread shell supports `par get <id>` for object rows as a read-only payload display only when `AUTOGEN_PM_MSH_CMD_GET_OBJECT` and `RT_USING_HEAP` are enabled: strings are quoted and escaped, raw bytes are printed as `hex:...`, and unsigned arrays are printed as `[v0,v1,...]`; shell `par set` for object rows is not supported
- parameter table access modes are limited to `ePAR_ACCESS_RO` and `ePAR_ACCESS_RW`; `ePAR_ACCESS_NONE` and write-only masks are rejected at build time

## Documentation map

- [Getting started](docs/getting-started.md) for integration steps, required files, and first-use examples
- [Architecture](docs/architecture.md) for storage model, validation flow, ID lookup, and layout design
- [API reference](docs/api-reference.md) for the public API grouped by responsibility
- [Object parameters](docs/object-parameters.md) for the dedicated object-pool design, read/write flow, fixed-size arrays, and current NVM behavior
- [Flash-ee backend design](docs/flash-ee-backend-design.md) for the portable flash-emulated EEPROM core and adapter model

## Package layout

```text
.
├── Kconfig
├── SConscript
├── backend/
│   ├── par_store_backend_flash_ee_fal.c
│   ├── par_store_backend_flash_ee_native.c
│   └── par_store_backend_rtt_at24cxx.c
├── par_table.def
├── parameters/
│   ├── README.md
│   ├── CHANGE_LOG.md
│   ├── docs/
│   │   ├── DeviceParameter_VerificationReport.xlsx
│   │   ├── api-reference.md
│   │   ├── architecture.md
│   │   ├── flash-ee-backend-design.md
│   │   └── getting-started.md
│   ├── src/
│   │   ├── par.c
│   │   ├── par.h
│   │   ├── par_cfg.h
│   │   ├── def/
│   │   ├── detail/
│   │   ├── layout/
│   │   ├── scalar/
│   │   │   └── par_scalar_api.c
│   │   ├── object/
│   │   │   ├── par_object.c
│   │   │   ├── par_object.h
│   │   │   └── par_object_api.c
│   │   ├── nvm/
│   │   │   ├── backend/
│   │   │   │   ├── par_store_backend.h
│   │   │   │   ├── par_store_backend_flash_ee.c
│   │   │   │   ├── par_store_backend_flash_ee.h
│   │   │   │   ├── par_store_backend_flash_ee_cfg.h
│   │   │   │   └── par_store_backend_gel_nvm.c
│   │   │   ├── fnv.h
│   │   │   ├── hash_32a.c
│   │   │   ├── par_nvm.c
│   │   │   ├── par_nvm.h
│   │   │   ├── par_nvm_cfg.h
│   │   │   ├── par_nvm_layout*.c
│   │   │   └── par_nvm_table_id.*
│   │   └── port/
│   └── template/
│       ├── par_cfg_port.htmp
│       ├── par_layout_static.htmp
│       └── par_table.deftmp
└── port/
    ├── par_atomic_port.h
    ├── par_cfg_port.h
    └── par_if_port.c
```

## Required integration files

This repository contains the reusable module core and templates. A real integration still needs project-owned files.

### Required

- `par_table.def` at the package root
- `port/par_cfg_port.h`

### Optional, depending on configuration

- `port/par_if_port.c` when `PAR_CFG_IF_PORT_EN = 1` and the target needs stronger platform hooks than the weak defaults in `src/port/par_if.c`
- `port/par_atomic_port.h` when `PAR_ATOMIC_BACKEND = PAR_ATOMIC_BACKEND_PORT`
- generated static layout header when `PAR_CFG_LAYOUT_SOURCE = PAR_CFG_LAYOUT_SCRIPT`
- a scalar/shared-object storage backend when scalar persistence is enabled or object persistence uses the shared backend address space
- dedicated object backend hooks when object persistence uses dedicated object-only storage

## When to read which document

- Read [Getting started](docs/getting-started.md) when you are integrating the module into a project.
- Read [Architecture](docs/architecture.md) when you need to understand how `par_table.def`, storage groups, layout, validation, or ID lookup work internally.
- Read [API reference](docs/api-reference.md) when you already understand the model and only need function-level guidance.
- Read [Object parameters](docs/object-parameters.md) when you are integrating `STR`, `BYTES`, or fixed-size array rows.

## Key integration notes

- `src/par.h` is the main public entry header. Keep `parameters/src` on the compiler include path so application code can use `#include "par.h"`.
- `par_cfg.h` includes `par_cfg_port.h` unconditionally, so your build must provide that header and make its directory visible to the compiler.
- `PAR_CFG_ENABLE_TYPE_F32` controls whether floating-point parameter support and the related typed APIs are compiled in.
- `PAR_CFG_ENABLE_RUNTIME_VALIDATION` and `PAR_CFG_ENABLE_CHANGE_CALLBACK` control whether normal setters include runtime validation callbacks and on-change callbacks. On-change callbacks are synchronous and run while the normal scalar setter still owns the parameter lock, so callback code should not call parameter APIs for the same parameter and should defer set/save or I/O work to application context.
- The RT-Thread port exposes leveled package logging hooks (`INFO` / `DEBUG` / `WARN` / `ERROR`). Enable them with `AUTOGEN_PM_USING_DEBUG` in package Kconfig.
- The module separates **internal parameter enumeration** (`par_num_t`) from **external parameter IDs** (`id`). Generic and ID-based get/set entry points remain scalar-only; object payload read/write uses the dedicated object typed APIs.
- The current ID lookup implementation uses a one-entry-per-bucket hash map generated at compile time from `par_table.def`. External IDs must therefore be not only unique, but also collision-free under the configured hash geometry. Optional runtime diagnostic scans can be enabled with `PAR_CFG_ENABLE_RUNTIME_ID_DUP_CHECK` and `PAR_CFG_ENABLE_RUNTIME_ID_HASH_COLLISION_CHECK` when additional startup logs are useful. See `docs/architecture.md` for the collision rule and avoidance guidance.
- Unchecked setter APIs skip runtime validation callbacks and on-change callbacks, so they should be reserved for tightly controlled hot paths. Bitwise fast setters are further restricted to `U8` / `U16` / `U32` flags or bitmask parameters. Legacy `*_fast()` names remain as deprecated aliases.
- NVM support is optional. When enabled, `src/nvm/par_nvm.c` only needs the packaged scalar storage backend when scalar persistence is enabled or object persistence uses the shared scalar-backend address space. Dedicated object-only products can disable scalar persistence and provide the object backend hooks without selecting a packaged scalar backend. Whether runtime ID support must also be enabled depends on the selected scalar persisted-record layout and backend constraints: the payload-only layouts may be used with `PAR_CFG_ENABLE_ID = 0` only when `PAR_CFG_TABLE_ID_CHECK_EN = 1`, while the stored-ID layouts and the current flash backend path require `PAR_CFG_ENABLE_ID = 1`. Persistence metadata is compiled in automatically under `PAR_CFG_NVM_EN`. The available packaged scalar-backend paths include the RT-Thread AT24CXX adapter and the portable flash-ee backend core, which can be bound either to FAL through the repository-root `backend/par_store_backend_flash_ee_fal.c` bridge or to product-specific native flash hooks through the repository-root `backend/par_store_backend_flash_ee_native.c` adapter.
- Live RAM layout and persisted NVM layout are intentionally different. RAM storage is grouped by value width, while the persistence area stores a compile-time ordered slot list using one selected serialized record layout: fixed 4-byte payload slot with size descriptor, fixed 4-byte payload slot without size descriptor, compact natural-width payload with size descriptor, fixed natural-width payload without stored ID, or grouped natural-width payload without stored ID.
- Compile-time persistent order is the primary slot layout contract of the managed NVM image. The stored-ID layouts keep `id` in each record as an integrity and diagnostics field, while the payload-only layouts omit stored `id` and instead rely on compile-time slot order plus table-ID validation. The fixed-slot layouts keep one 4-byte payload slot per persistent parameter, while the compact and payload-only layouts store only the natural 1/2/4-byte payload width.
- The serialized NVM header is written explicitly as a fixed 12-byte storage image (`sign(4) + obj_nb(2) + table_id(4) + crc16(2)`), so on-storage layout does not depend on compiler struct padding. Header CRC-16 covers the serialized `obj_nb + table_id` bytes, while each data record carries its own CRC-8 according to the selected record layout.
- `par_nvm.c` now binds one selected persisted-record layout adapter during initialization. Record address calculation, object preparation, stored-object validation, compatibility policy, and optional write-readback comparison are all delegated through that layout ops table, so the common NVM flow no longer carries layout-specific branching. When `PAR_CFG_NVM_WRITE_VERIFY_EN = 1`, parameter writes and header commits force a backend sync and then perform layout-aware readback verification before the write is considered committed.
- `par_nvm_write(par_num, false)` requests a normal parameter save without an additional caller-forced sync step. It does **not** guarantee RAM-only staging. Backend contracts may still persist data before the call returns, and a successful return means the backend-side persistence work required for that request is complete.
- The flash-emulated EEPROM backend may synchronize one dirty cache window before loading the next window during one logical write or erase. Successful multi-window requests therefore finish fully persisted, but failures are still non-transactional and may leave earlier windows committed while later windows remain old.
- For flash-emulated EEPROM integrations, size `PAR_CFG_NVM_BACKEND_FLASH_EE_CACHE_SIZE` so common parameter objects fit inside one cache window whenever practical. Also keep one must-stay-consistent parameter group within one independently committed window, or add an application-level consistency/version marker if that is not possible.
- Flash-ee build-time configuration now rejects invalid pure-configuration geometry combinations such as logical-size/cache-size values that do not divide by the configured line size or program-size values that do not divide the 64-byte bank header exactly.
- Native flash-ee integrations should provide strong definitions for the required public native hook ABI declared in `parameters/src/nvm/backend/par_store_backend_flash_ee.h`; missing required operational or geometry hooks are intended to fail the final link instead of degrading into runtime stub behavior, while benign helper hooks may still use package defaults.
- If the selected storage medium has ECC or other read-health reporting, treat that as a product-level policy input rather than as an automatic parameter-core decision. The business layer should decide whether an ECC event only needs reporting, should trigger parameter reset or rebuild, or should escalate to a wider system fault response.
- CRC calculation is routed through port hooks with bundled software defaults. In this single-target profile the persisted image and the table-ID digest both use the native byte order of the running platform, so no additional byte-order conversion hook is required by the persistence path.
- When `PAR_CFG_TABLE_ID_CHECK_EN = 1`, startup compares the stored table-ID against the live compatibility digest for the stored persistent prefix size from the header (`obj_nb`). Layouts with stored IDs hash external parameter IDs, so prefix ID renumbering still invalidates the image there. `FIXED_PAYLOAD_ONLY` intentionally excludes external parameter IDs and validates only prefix byte-layout compatibility (`obj_nb`, persistent order, and parameter type), which allows pure external-ID renumbering and compatible tail growth without a rebuild there. `GROUPED_PAYLOAD_ONLY` also excludes external parameter IDs, but because regrouped addresses depend on the full live persistent set it rebuilds whenever `stored_count != live_count`. Semantic-only prefix remaps that preserve the same byte layout must still be paired with an explicit `PAR_CFG_TABLE_ID_SCHEMA_VER` bump. Defaults, ranges, names, units, descriptions, and access flags remain outside the digest.
- A table-ID mismatch is treated as an incompatible persisted-layout change, not as a warning-only condition. Startup restores defaults and rebuilds the managed NVM image. Typical triggers are add/remove/reorder/type/ID changes of persistent parameters and transitions between persistent and non-persistent state. When the stored header count is smaller than the compile-time persistent count, layouts with stable prefix addresses repair the image by appending the missing tail slots from current defaults and rewriting the header count; `GROUPED_PAYLOAD_ONLY` instead treats any stored/live count mismatch as incompatible and rebuilds. A stored count larger than the compile-time count is always treated as incompatible and rebuilt.
- `PAR_CFG_TABLE_ID_SCHEMA_VER` defaults in `src/par_cfg.h` and may be overridden in `port/par_cfg_port.h`; the integrator should bump it when intentionally changing the serialized table-ID schema.
- The fixed 4-byte payload slot without size descriptor is not available when a flash backend requires 8-byte aligned writes, because that layout serializes each record to 7 bytes.
- The package intentionally keeps a compile-time ordered slot image instead of introducing a free-layout scanned log. That keeps the address model deterministic and avoids boot-time scan, compaction, and tombstone handling in the common parameter path.
- `par_init()` applies startup default values directly to live storage. Integer default values from `par_table.def` are compiled into a grouped width-based storage object, while `F32` default values are applied to the 32-bit storage group after layout offsets are available only when `PAR_CFG_ENABLE_TYPE_F32 = 1`. Because this startup initialization does not go through the public setter path, it does not invoke runtime validation or on-change callbacks.
- `PAR_CFG_ENABLE_RESET_ALL_RAW` controls whether raw reset-all support and grouped default mirror snapshot support are enabled. Default restore is a maintenance/recovery path and does not require external write permission or matching write-role metadata.

## Related projects

- [CLI module](https://github.com/GeneralEmbeddedCLibraries/cli)
- [NVM module](https://github.com/GeneralEmbeddedCLibraries/nvm)
- [General Embedded C Library Manual](https://github.com/GeneralEmbeddedCLibraries/documentation/blob/develop/General_Embedded_C_Library_Manual.pdf)


## Flash-emulated EEPROM backend

See `parameters/docs/flash-ee-backend-design.md` for the portable core, configuration header, commit/recovery model, cross-window write semantics, and FAL/native integration guidance.
