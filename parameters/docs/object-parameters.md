# Object parameters

This document describes the dedicated object-parameter framework used by the parameter manager for fixed-capacity strings, raw byte arrays, and fixed-size unsigned arrays.

## Scope

Supported object types are:

- `STR`
- `BYTES`
- `ARR_U8`
- `ARR_U16`
- `ARR_U32`

These object rows share the same high-level model:

- configuration is still defined in `par_table.def`
- live payload bytes are stored out-of-line in one shared object pool
- each object parameter owns one runtime slot descriptor
- managed NVM persistence is optional and uses a dedicated object block when `PAR_CFG_NVM_OBJECT_EN = 1` and `PAR_CFG_ENABLE_ID = 1`

## Why objects are handled separately from scalars

The original parameter manager is optimized for scalar values stored in grouped width-based storage arrays.

That scalar model works well for `U8/U16/U32/I8/I16/I32/F32`, but it does not directly represent:

- variable current payload length
- object payload capacity
- byte-oriented defaults
- array payloads whose logical element count differs from the stored byte count

For that reason, object rows keep their live payload bytes outside the scalar storage groups and use dedicated runtime metadata.

## Configuration model

Each object row in `par_table.def` stores:

- object type
- minimum accepted payload length in bytes
- maximum accepted payload length in bytes
- default payload descriptor
- normal access metadata such as name, unit, description, ID, and read or write policy

Examples:

```c
PAR_ITEM_STR(
    ePAR_WIFI_SSID,
    50001,
    "WiFi SSID",
    0U,
    32U,
    ((par_obj_init_t){ .p_data = (const uint8_t *)"demo-ap", .len = 7U }),
    NULL,
    ePAR_ACCESS_RW,
    ePAR_ROLE_ALL,
    ePAR_ROLE_ALL,
    0,
    "Fixed-capacity string stored in the shared object pool.")

PAR_ITEM_BYTES(
    ePAR_AES_KEY,
    50002,
    "AES Key",
    16U,
    16U,
    ((par_obj_init_t){ .p_data = (const uint8_t[]){ 0x00U, 0x11U, 0x22U, 0x33U, 0x44U, 0x55U, 0x66U, 0x77U, 0x88U, 0x99U, 0xAAU, 0xBBU, 0xCCU, 0xDDU, 0xEEU, 0xFFU }, .len = 16U }),
    NULL,
    ePAR_ACCESS_RW,
    ePAR_ROLE_ALL,
    ePAR_ROLE_ALL,
    0,
    "Fixed-capacity raw byte-array stored in the shared object pool.")
```

For `STR`, `min_` and `max_` are the stored payload-length limits in bytes. They do not include the trailing NUL that `par_get_str()` and `par_get_default_str()` add to caller buffers, so `PAR_ITEM_STR(..., max_, ...)` is limited to `UINT16_MAX - 1U`.

For fixed-size arrays, `min_` and `max_` are the element-count limits in the table row, and the compile-time configuration converts them into byte-length limits internally.

The object rows shipped in the bundled `par_table.def` are optional examples. They are wrapped in the matching `PAR_CFG_ENABLE_TYPE_*` condition so a disabled object type removes the bundled sample row from the build instead of making that sample fail.

Product tables should use such guards only for optional demo/sample rows. Required production object rows should remain unguarded so disabling the required type fails early.

## Runtime storage model

Object parameters use two runtime containers.

### Shared object pool

The shared object pool stores the live payload bytes for every object parameter.

Conceptually it looks like this:

```text
+---------------- shared object pool ----------------+
| STR bytes | BYTES payload | ARR_U8 | ARR_U16 | ... |
+----------------------------------------------------+
```

Each object row owns one fixed byte window inside this pool.

### Object slot table

Each object parameter also owns one runtime slot descriptor.

The slot tracks:

- payload start offset inside the shared object pool
- current payload length in bytes
- configured capacity in bytes

Conceptually:

```text
slot[n] -> { pool_offset, len, capacity }
```

The slot does not duplicate the payload bytes. It only describes where the payload is stored and how many bytes are currently valid.

## Read and write flow

### Startup flow

1. `par_table.def` is expanded into the generated configuration table.
2. `par_layout` assigns one object slot index and one object-pool byte offset to each object row.
3. `par_init()` copies default payload bytes from `value_cfg.object.def` into the shared object pool.
4. The runtime slot length is initialized to the default payload length.

### Write flow

The write path for `par_set_str()`, `par_set_bytes()`, and `par_set_arr_*()` is:

1. verify that the row type matches the requested object API
2. verify the supplied payload length in bytes
3. verify the payload length is within the configured range
4. copy payload bytes into the assigned object-pool window
5. update the runtime slot length
6. return without invoking any core object on-change callback

### Read flow

The read path for `par_get_str()`, `par_get_bytes()`, and `par_get_arr_*()` is:

1. verify that the row type matches the requested object API
2. read the runtime slot length
3. copy payload bytes from the shared object pool into the caller buffer
4. return the actual payload length or element count through the typed API contract

The public API intentionally avoids exposing live object-pool pointers. Use the dedicated typed getters to copy payloads out of the shared object pool. When `PAR_CFG_ENABLE_ACCESS = 1`, object rows must use `ePAR_ACCESS_RO` or `ePAR_ACCESS_RW`; `ePAR_ACCESS_NONE` and write-only access are rejected at build time.

## Byte length versus element count

Strings and raw bytes use a direct byte-length model.

For `STR`, the configured capacity is the maximum stored payload length. It does not reserve or count the trailing NUL copied to a `par_get_str()` destination buffer, so callers must provide at least `payload_len + 1` bytes.

Fixed-size arrays use the same internal byte-length storage, but the public typed-array APIs present element counts to the caller.

Examples:

- `ARR_U8` capacity for 4 elements = 4 bytes
- `ARR_U16` capacity for 4 elements = 8 bytes
- `ARR_U32` capacity for 4 elements = 16 bytes

That is why object metadata stores both:

- `elem_size`
- byte-length range

## Metadata layout inside `par_cfg_t`

Type-specific value metadata is stored in:

```c
typedef union
{
    par_scalar_cfg_t scalar;
    par_object_cfg_t object;
} par_value_cfg_t;
```

This is a union because one parameter row is either:

- a scalar row, or
- an object row

It cannot be both at the same time.

On a typical 32-bit embedded target, the current sizes are usually:

- `sizeof(par_scalar_cfg_t) = 12`
- `sizeof(par_object_cfg_t) = 16`
- `sizeof(par_value_cfg_t) = 16`

On a 64-bit host build, `par_object_cfg_t` usually grows because `par_obj_init_t` contains a pointer, so `sizeof(par_value_cfg_t)` is typically larger.

## Layout generation

The object framework depends on layout generation in the same way scalar groups do.

### Compile-scan layout mode

Compile-scan layout mode can derive object-pool offsets by scanning the configuration table at runtime.

### Script layout mode

Script layout mode requires generated object-slot counts and object-pool byte offsets only when object rows are compiled in. Scalar-only builds can keep using scalar-only static layout headers.

The generated static layout contract should include:

- parameter-to-slot offsets for scalar groups
- parameter-to-object-slot indices when needed
- parameter-to-object-pool byte offsets
- total shared object-pool byte count

The runtime still validates that generated object-pool offsets are contiguous and follow parameter-table order. Generators must not leave gaps or reorder object windows.

That keeps the script-generated layout authoritative within the same contiguous packing contract used by compile-scan mode.

## NVM behavior

Object rows remain RAM-only by default. When `PAR_CFG_NVM_OBJECT_EN = 0`, any object row with `pers_ = 1` is rejected at compile time so legacy scalar NVM images are not changed accidentally.

When `PAR_CFG_NVM_EN = 1`, `PAR_CFG_NVM_OBJECT_EN = 1`, and `PAR_CFG_ENABLE_ID = 1`, persistent object rows are stored in a dedicated object persistence block. The scalar block continues to use the selected scalar record layout, while the object block uses fixed-capacity records so each persistent object has a deterministic address within the current compiled object layout.

`PAR_CFG_NVM_OBJECT_STORE_MODE` selects the storage target first:

- `PAR_CFG_NVM_OBJECT_STORE_SHARED`: default compact mode. Scalar and object records use the same backend address space.
- `PAR_CFG_NVM_OBJECT_STORE_DEDICATED`: product isolation mode. Object records use a port-provided object backend or partition through `par_object_store_backend_bind()` and `par_object_store_backend_get_api()`. Object addresses are relative to that object backend, so scalar block growth, scalar erase/rebuild, and scalar address changes do not move or overwrite object records.

When shared-store mode is selected, `PAR_CFG_NVM_OBJECT_ADDR_MODE` selects where the object block is placed:

- `PAR_CFG_NVM_OBJECT_ADDR_AFTER_SCALAR`: default compact placement. The object block starts immediately after the current scalar NVM block and needs no extra address configuration. If the scalar persistent layout grows, the object block base can move.
- `PAR_CFG_NVM_OBJECT_ADDR_FIXED`: fixed placement in the same backend. The object block starts at `PAR_CFG_NVM_OBJECT_FIXED_ADDR`; optional `PAR_CFG_NVM_OBJECT_REGION_SIZE` rejects compiled object layouts that do not fit the reserved region. The fixed address must not overlap the scalar block in the same backend address space.

When dedicated-store mode is selected, the object block starts at `PAR_CFG_NVM_OBJECT_DEDICATED_BASE_ADDR` inside the object backend or partition. Keeping this value at zero is valid when the object partition begins directly with the object persistence block.

The generic core does not migrate object blocks between addresses. In shared after-scalar mode, scalar persistent-layout growth can move the object block; on flash-class media, the scalar rebuild or erase sequence may already have overwritten the old object block before object recovery runs. A reliable relocation needs product-specific storage planning, such as scratch or double-buffer sectors, commit markers, power-loss recovery, and erase-block ownership rules. Keep `PAR_CFG_NVM_OBJECT_ADDR_AFTER_SCALAR` for compact layouts whose object values may be rebuilt from defaults after scalar layout growth. Use `PAR_CFG_NVM_OBJECT_ADDR_FIXED` or `PAR_CFG_NVM_OBJECT_STORE_DEDICATED` before release when persisted object values must remain stable across scalar layout changes.

Each object record stores:

- external parameter ID when ID support is enabled
- object type
- element size
- fixed payload capacity in bytes
- current valid payload length in bytes
- CRC-16 over metadata and valid payload bytes
- payload bytes up to the configured capacity

The object block has its own signature, layout version, object count, table-ID digest, body size, and header CRC. Object persistence requires external parameter IDs and stores that ID in each object record. A scalar value rebuild does not rewrite the object block when the resolved object address remains valid. An object persistence rebuild restores only persistent object rows to their defaults and rewrites only the object block.

Compatibility is intentionally conservative. The object table-ID digest covers the persistent object prefix, object layout version, parameter type, external parameter ID, element size, minimum length, and maximum length. If the live schema is incompatible, the object block is rebuilt from defaults. When table-ID checking is enabled, compatible tail append is supported only when the object block base remains stable; capacity changes of existing records are treated as an incompatible schema change because fixed record addresses would otherwise shift.

## Public API boundary

Firmware-level object payload read/write is handled by the dedicated object typed APIs.

- supported firmware APIs: `par_set_str()` / `par_get_str()`, `par_set_bytes()` / `par_get_bytes()`, `par_set_arr_*()` / `par_get_arr_*()`, `par_get_obj_len()`, `par_get_obj_capacity()`, `par_set_obj_n_save()` when object persistence and external IDs are enabled, and `par_get_default_*()`
- supported shell payload display: the packaged RT-Thread shell accepts `par get <id>` for object rows after the normal read boundary succeeds only when `AUTOGEN_PM_MSH_CMD_GET_OBJECT` and `RT_USING_HEAP` are enabled. `STR` rows are printed as quoted and escaped strings, `BYTES` rows as `hex:...`, and `ARR_U8` / `ARR_U16` / `ARR_U32` rows as `[v0,v1,...]`.
- supported shell default restore: `par def <id>` and `par def_all` can restore object rows to their configured defaults through the maintenance/recovery path without requiring object write permission.
- not supported: generic scalar payload APIs `par_set()` / `par_get()` / `par_set_scalar_n_save()`, ID-based payload wrappers `par_set_by_id()` / `par_get_by_id()`, or shell `par set` for object rows

The core does not provide object on-change callbacks. Integrations that need notifications should wrap successful object setter calls and emit events or compare values at the application layer.

## Practical guidance

- Use `STR` for human-readable identifiers or small text fields.
- Use `BYTES` for opaque byte payloads such as keys, hashes, or binary configuration blobs.
- Use `ARR_U8/U16/U32` for fixed-size lookup tables and coefficient arrays.
- Keep `pers_ = 0` unless the product explicitly enables both `PAR_CFG_NVM_OBJECT_EN` and `PAR_CFG_ENABLE_ID` and accepts the object-block compatibility policy.
- Prefer fixed-size arrays only when the element count is part of the contract.

## Related documents

- [README](../README.md)
- [Getting started](getting-started.md)
- [Architecture](architecture.md)
- [API reference](api-reference.md)
