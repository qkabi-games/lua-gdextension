# Lua Sandbox Security System

The lua-gdextension library includes a configurable sandbox system to restrict what APIs are available to untrusted Lua scripts.

## Features

- Block variant types (e.g., `FileAccess`, `DirAccess`)
- Block utility functions (e.g., `dofile`, `loadfile`)
- Block classes (e.g., `OS`, `ClassDB`)
- Block singletons (e.g., `Engine`)
- Block enums (e.g., `Error`)
- Block Lua standard library functions (e.g., `os.execute`, `io.popen`)
- Block Lua libraries (e.g., `debug`, `ffi`)

## Usage

### Option 1: Built-in Defaults + Custom

```gdscript
var config = LuaSandboxConfig.with_defaults()
config.block_class("Thread")  # Add more blocks
lua.set_sandbox_config(config)
lua.open_libraries(LuaState.ALL_LIBS)
```

### Option 2: Fully Programmatic

```gdscript
var config = LuaSandboxConfig.create()
config.block_variant_type("FileAccess")
config.block_variant_type("DirAccess")
config.block_singleton("Engine")
lua.set_sandbox_config(config)
lua.open_libraries(LuaState.ALL_LIBS)
```

## API Reference

### LuaSandboxConfig Class

#### Static Methods

| Method | Description |
|--------|-------------|
| `create()` | Create empty config |
| `with_defaults()` | Create config with built-in dangerous defaults |

#### Instance Methods (Fluent)

| Method | Description |
|--------|-------------|
| `block_variant_type(type: StringName)` | Block a variant type |
| `unblock_variant_type(type: StringName)` | Remove block from variant type |
| `is_variant_type_blocked(type: StringName) -> bool` | Check if blocked |
| `block_utility_function(func: StringName)` | Block a utility function |
| `unblock_utility_function(func: StringName)` | Remove block from utility function |
| `is_utility_function_blocked(func: StringName) -> bool` | Check if blocked |
| `block_class(cls: StringName)` | Block a class |
| `unblock_class(cls: StringName)` | Remove block from class |
| `is_class_blocked(cls: StringName) -> bool` | Check if blocked |
| `block_singleton(name: StringName)` | Block a singleton |
| `unblock_singleton(name: StringName)` | Remove block from singleton |
| `is_singleton_blocked(name: StringName) -> bool` | Check if blocked |
| `block_enum(name: StringName)` | Block an enum |
| `unblock_enum(name: StringName)` | Remove block from enum |
| `block_library(name: StringName)` | Block a Lua library (e.g., "debug", "ffi") |
| `unblock_library(name: StringName)` | Remove block from library |
| `is_library_blocked(name: StringName) -> bool` | Check if blocked |
| `block_library_function(func: StringName)` | Block a Lua library function (e.g., "os.execute") |
| `unblock_library_function(func: StringName)` | Remove block from library function |
| `is_library_function_blocked(func: StringName) -> bool` | Check if blocked |

## Error Handling

When a blocked item is accessed, an error is logged to the Godot console:

```
LuaSandbox: Blocked class 'FileAccess' is not available in sandbox
```

The script receives `nil` for the blocked access.

## Example: Safe Game Script Sandbox

```gdscript
var lua = LuaState.new()

# Create sandbox with defaults
var config = LuaSandboxConfig.with_defaults()

# Allow your game's safe API
# (these are accessible via your game engine, not Lua directly)

lua.set_sandbox_config(config)
lua.open_libraries(LuaState.ALL_LIBS)

# Safe script - only math, string, table operations available
# No file system, OS, or threading access
lua.do_string("""
local result = math.sqrt(16)  -- OK
local greeting = "Hello, " .. "World!"  -- OK
local data = {1, 2, 3, 4, 5}  -- OK

-- These would fail:
-- local file = FileAccess.open("secret.txt", FileAccess.READ)  -- Blocked!
-- os.execute("rm -rf /")  -- Blocked!
""")
```

## Security Considerations

1. **Defense in Depth**: The sandbox blocks dangerous APIs but don't rely on it as the only security measure
2. **Performance**: Blocking checks have minimal overhead
3. **Updates**: When Godot adds new dangerous APIs, update your config accordingly
4. **Testing**: Always test your sandbox configuration thoroughly

## Migration from Non-Sandboxed

To add sandboxing to an existing project:

1. Create a `LuaSandboxConfig` with appropriate settings
2. Call `lua.set_sandbox_config(config)` before `lua.open_libraries()`
3. Test all existing scripts to ensure they still work
4. Adjust the config as needed for your use case
