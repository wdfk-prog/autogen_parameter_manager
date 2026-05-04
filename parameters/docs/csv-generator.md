# CSV Parameter Generator

This document explains how to maintain the parameter table in `parameters/schema/par_table.csv` and use `parameters/tools/pargen.py` to generate the C files used by firmware builds.

## Goal

The generator moves the human-maintained parameter definition source from `par_table.def` to CSV:

```text
parameters/schema/par_table.csv
        -> parameters/tools/pargen.py
par_table.def
parameters/generated/par_layout_static.h
parameters/generated/par_layout_static.c
parameters/generated/par_generated_info.h
parameters/generated/par_generated_info.c
parameters/generated/par_manifest.json
parameters/schema/par_id_lock.json
```

`par_table.def` is still kept as the compatibility input for the existing X-Macro implementation. Do not edit the generated `par_table.def` manually.

## Python Environment

`pargen.py` uses only the Python standard library. No third-party Python packages are required.

Recommended environment:

- Python 3.8 or later;
- Python from RT-Thread Env / RT-Thread Studio, or system Python;
- no `pip install` step;
- no `pandas`, `jinja2`, `pyyaml`, or `pytest` dependency.

Check Python:

```sh
python --version
```

If a local development environment injects extra Python startup or `site` packages and causes startup failures, use this temporary diagnostic form:

```sh
python -S parameters/tools/pargen.py --check
```

`-S` skips automatic `site` imports. It is usually only needed to isolate local Python environment problems. Normal project workflows should use plain `python3`.

## Common Commands

Validate the CSV without writing generated files:

```sh
python parameters/tools/pargen.py --check
```

Regenerate all outputs:

```sh
python parameters/tools/pargen.py
```

Verify that generated outputs are synchronized with the CSV:

```sh
python parameters/tools/pargen.py --verify
```

Run generator unit tests:

```sh
python -m unittest discover -s parameters/tests -v
```

## CSV Fields

`parameters/schema/par_table.csv` must contain these fields:

| Field | Description |
|---|---|
| `group` | Parameter group used to generate comments in `par_table.def`. |
| `section` | Parameter subsection used to generate comments in `par_table.def`. |
| `condition` | Optional C preprocessor condition, such as `(1 == PAR_CFG_ENABLE_TYPE_STR)`. An empty value means the row is always compiled. |
| `enum` | Parameter enum name. It must match `ePAR_[A-Z0-9_]+`. |
| `id` | External parameter ID. Use `AUTO` or leave it empty to let the generator allocate one. |
| `type` | Parameter type: `U8/U16/U32/I8/I16/I32/F32/STR/BYTES/ARR_U8/ARR_U16/ARR_U32`. |
| `name` | Parameter display name. |
| `min` | Scalar minimum value, or object minimum length / minimum element count. |
| `max` | Scalar maximum value, or object maximum length / maximum element count. |
| `default` | Default value. Scalars use numeric values; objects use controlled text formats. |
| `unit` | Engineering unit. An empty value generates `NULL`. |
| `access` | `RO` or `RW`. |
| `read_roles` | `ALL/NONE/PUBLIC/SERVICE/DEVELOPER/MANUFACTURING`, or a `|`-separated combination. |
| `write_roles` | Same format as `read_roles`. |
| `persistent` | `0/1` or `true/false`. |
| `desc` | Parameter description stored in the parameter table. |
| `comment` | Source comment only. It is not stored in the parameter table. |

## Object Default Formats

For object parameters, the `default` field uses controlled formats. The generator converts them to `par_obj_init_t` values:

| Type | CSV example | Generated meaning |
|---|---|---|
| `STR` | `ap` | UTF-8 string default value. Length is counted in bytes. |
| `BYTES` | `0x00 0x11 0x22` | Raw byte array. |
| `ARR_U8` | `1,2,3,4` | Fixed `uint8_t` array. |
| `ARR_U16` | `100,200,300` | Fixed `uint16_t` array. |
| `ARR_U32` | `1000,2000` | Fixed `uint32_t` array. |

Array object types currently require `min == max`, and the default element count must equal `max`.

## ID and Hash Collision Policy

The generator reuses the firmware static ID hash algorithm:

```c
(((uint32_t)id * 0x61C88647u) >> (32u - PAR_ID_HASH_BITS))
```

Rules:

1. Explicit `id` values are never changed silently by the generator.
2. `AUTO` IDs first reuse entries from `parameters/schema/par_id_lock.json`.
3. New `AUTO` IDs are allocated from the group ranges in `parameters/schema/pargen.json`.
4. Any duplicate ID or hash bucket collision fails generation.
5. After successful generation, `par_id_lock.json` records the current `enum -> id` mapping so row reordering does not change allocated IDs.

## Static Layout Mode

The default build still uses `PAR_CFG_LAYOUT_COMPILE_SCAN`, which scans the parameter table at startup to generate offsets.

Enable this RT-Thread package option:

```text
AUTOGEN_PM_LAYOUT_SOURCE_SCRIPT=y
```

It maps to:

```c
#define PAR_CFG_LAYOUT_SOURCE         PAR_CFG_LAYOUT_SCRIPT
#define PAR_CFG_LAYOUT_STATIC_INCLUDE "par_layout_static.h"
```

In this mode, `SConscript` compiles:

```text
parameters/generated/par_layout_static.c
```

`par_layout.h` performs compile-time checks that the generated `count8/count16/count32/count_obj/obj_pool_bytes` values and generated layout signature match the current `par_table.def` X-Macro expansion. The current-table signature is calculated independently from the generated files, so reordering same-type rows without refreshing `parameters/generated/par_layout_static.*` fails the build instead of using stale offsets.

## Generated Summary Information

Enable this RT-Thread package option:

```text
AUTOGEN_PM_ENABLE_GENERATED_INFO=y
```

It compiles:

```text
parameters/generated/par_generated_info.c
```

The generated file exports:

```c
extern const par_generated_info_t g_par_generated_info;
```

Debugger integrations, shell commands, or product diagnostic code can use this object to inspect the total parameter count, grouped storage counts, object-pool size, and ID hash geometry.

## Change Workflow

1. Edit `parameters/schema/par_table.csv`.
2. Run `python parameters/tools/pargen.py --check`.
3. Run `python parameters/tools/pargen.py`.
4. Run `python parameters/tools/pargen.py --verify`.
5. Run `python -m unittest discover -s parameters/tests -v`.
6. Run the SCons build for the selected project configuration.

A commit should include the CSV, lock file, generated C files, and related documentation. Do not submit only the CSV while omitting generated outputs.
