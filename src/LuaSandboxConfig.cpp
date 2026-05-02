/**
 * Copyright (C) 2026 Gil Barbosa Reis.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "LuaSandboxConfig.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/variant.hpp>

namespace luagdextension {

void LuaSandboxConfig::_bind_methods() {
	ClassDB::bind_static_method("LuaSandboxConfig", D_METHOD("create"), &LuaSandboxConfig::create);
	ClassDB::bind_static_method("LuaSandboxConfig", D_METHOD("with_defaults"), &LuaSandboxConfig::with_defaults);

	ClassDB::bind_method(D_METHOD("block_variant_type", "type"), &LuaSandboxConfig::block_variant_type);
	ClassDB::bind_method(D_METHOD("unblock_variant_type", "type"), &LuaSandboxConfig::unblock_variant_type);
	ClassDB::bind_method(D_METHOD("is_variant_type_blocked", "type"), &LuaSandboxConfig::is_variant_type_blocked);
	ClassDB::bind_method(D_METHOD("get_blocked_variant_types"), &LuaSandboxConfig::get_blocked_variant_types);

	ClassDB::bind_method(D_METHOD("block_utility_function", "func"), &LuaSandboxConfig::block_utility_function);
	ClassDB::bind_method(D_METHOD("unblock_utility_function", "func"), &LuaSandboxConfig::unblock_utility_function);
	ClassDB::bind_method(D_METHOD("is_utility_function_blocked", "func"), &LuaSandboxConfig::is_utility_function_blocked);
	ClassDB::bind_method(D_METHOD("get_blocked_utility_functions"), &LuaSandboxConfig::get_blocked_utility_functions);

	ClassDB::bind_method(D_METHOD("block_class", "cls"), &LuaSandboxConfig::block_class);
	ClassDB::bind_method(D_METHOD("unblock_class", "cls"), &LuaSandboxConfig::unblock_class);
	ClassDB::bind_method(D_METHOD("is_class_blocked", "cls"), &LuaSandboxConfig::is_class_blocked);
	ClassDB::bind_method(D_METHOD("get_blocked_classes"), &LuaSandboxConfig::get_blocked_classes);

	ClassDB::bind_method(D_METHOD("block_singleton", "name"), &LuaSandboxConfig::block_singleton);
	ClassDB::bind_method(D_METHOD("unblock_singleton", "name"), &LuaSandboxConfig::unblock_singleton);
	ClassDB::bind_method(D_METHOD("is_singleton_blocked", "name"), &LuaSandboxConfig::is_singleton_blocked);
	ClassDB::bind_method(D_METHOD("get_blocked_singletons"), &LuaSandboxConfig::get_blocked_singletons);

	ClassDB::bind_method(D_METHOD("block_enum", "name"), &LuaSandboxConfig::block_enum);
	ClassDB::bind_method(D_METHOD("unblock_enum", "name"), &LuaSandboxConfig::unblock_enum);
	ClassDB::bind_method(D_METHOD("is_enum_blocked", "name"), &LuaSandboxConfig::is_enum_blocked);
	ClassDB::bind_method(D_METHOD("get_blocked_enums"), &LuaSandboxConfig::get_blocked_enums);

	ClassDB::bind_method(D_METHOD("block_library", "name"), &LuaSandboxConfig::block_library);
	ClassDB::bind_method(D_METHOD("unblock_library", "name"), &LuaSandboxConfig::unblock_library);
	ClassDB::bind_method(D_METHOD("is_library_blocked", "name"), &LuaSandboxConfig::is_library_blocked);
	ClassDB::bind_method(D_METHOD("get_blocked_libraries"), &LuaSandboxConfig::get_blocked_libraries);

	ClassDB::bind_method(D_METHOD("block_library_function", "func"), &LuaSandboxConfig::block_library_function);
	ClassDB::bind_method(D_METHOD("unblock_library_function", "func"), &LuaSandboxConfig::unblock_library_function);
	ClassDB::bind_method(D_METHOD("is_library_function_blocked", "func"), &LuaSandboxConfig::is_library_function_blocked);
	ClassDB::bind_method(D_METHOD("get_blocked_library_functions"), &LuaSandboxConfig::get_blocked_library_functions);
}

Ref<LuaSandboxConfig> LuaSandboxConfig::create() {
	return Ref<LuaSandboxConfig>(memnew(LuaSandboxConfig()));
}

Ref<LuaSandboxConfig> LuaSandboxConfig::with_defaults() {
	Ref<LuaSandboxConfig> config = memnew(LuaSandboxConfig);

	// File system
	config->blocked_classes.insert("FileAccess");
	config->blocked_classes.insert("DirAccess");
	config->blocked_classes.insert("ResourceLoader");
	config->blocked_classes.insert("ResourceSaver");

	// System level
	config->blocked_classes.insert("OS");
	config->blocked_classes.insert("JavaScriptBridge");
	config->blocked_classes.insert("CSharpServer");

	// Threading
	config->blocked_classes.insert("Thread");
	config->blocked_classes.insert("WorkerThreadPool");
	config->blocked_classes.insert("Mutex");
	config->blocked_classes.insert("Semaphore");

	// Reflection
	config->blocked_classes.insert("ClassDB");
	config->blocked_classes.insert("Class");

	// Dangerous Lua stdlib libraries
	config->blocked_libraries.insert("debug");
	config->blocked_libraries.insert("ffi");

	// Dangerous Lua stdlib functions
	config->blocked_library_functions.insert("os.execute");
	config->blocked_library_functions.insert("os.remove");
	config->blocked_library_functions.insert("os.rename");
	config->blocked_library_functions.insert("os.getenv");
	config->blocked_library_functions.insert("os.setenv");
	config->blocked_library_functions.insert("os.exit");
	config->blocked_library_functions.insert("os.clock");
	config->blocked_library_functions.insert("io.popen");

	return config;
}

Ref<LuaSandboxConfig> LuaSandboxConfig::block_variant_type(const StringName &type) {
	blocked_variant_types.insert(type);
	return this;
}

Ref<LuaSandboxConfig> LuaSandboxConfig::unblock_variant_type(const StringName &type) {
	blocked_variant_types.erase(type);
	return this;
}

bool LuaSandboxConfig::is_variant_type_blocked(const StringName &type) const {
	return blocked_variant_types.has(type);
}

PackedStringArray LuaSandboxConfig::get_blocked_variant_types() const {
	PackedStringArray arr;
	for (const StringName &type : blocked_variant_types) {
		arr.append(type);
	}
	return arr;
}

Ref<LuaSandboxConfig> LuaSandboxConfig::block_utility_function(const StringName &func) {
	blocked_utility_functions.insert(func);
	return this;
}

Ref<LuaSandboxConfig> LuaSandboxConfig::unblock_utility_function(const StringName &func) {
	blocked_utility_functions.erase(func);
	return this;
}

bool LuaSandboxConfig::is_utility_function_blocked(const StringName &func) const {
	return blocked_utility_functions.has(func);
}

PackedStringArray LuaSandboxConfig::get_blocked_utility_functions() const {
	PackedStringArray arr;
	for (const StringName &func : blocked_utility_functions) {
		arr.append(func);
	}
	return arr;
}

Ref<LuaSandboxConfig> LuaSandboxConfig::block_class(const StringName &cls) {
	blocked_classes.insert(cls);
	return this;
}

Ref<LuaSandboxConfig> LuaSandboxConfig::unblock_class(const StringName &cls) {
	blocked_classes.erase(cls);
	return this;
}

bool LuaSandboxConfig::is_class_blocked(const StringName &cls) const {
	return blocked_classes.has(cls);
}

PackedStringArray LuaSandboxConfig::get_blocked_classes() const {
	PackedStringArray arr;
	for (const StringName &cls : blocked_classes) {
		arr.append(cls);
	}
	return arr;
}

Ref<LuaSandboxConfig> LuaSandboxConfig::block_singleton(const StringName &name) {
	blocked_singletons.insert(name);
	return this;
}

Ref<LuaSandboxConfig> LuaSandboxConfig::unblock_singleton(const StringName &name) {
	blocked_singletons.erase(name);
	return this;
}

bool LuaSandboxConfig::is_singleton_blocked(const StringName &name) const {
	return blocked_singletons.has(name);
}

PackedStringArray LuaSandboxConfig::get_blocked_singletons() const {
	PackedStringArray arr;
	for (const StringName &name : blocked_singletons) {
		arr.append(name);
	}
	return arr;
}

Ref<LuaSandboxConfig> LuaSandboxConfig::block_enum(const StringName &name) {
	blocked_enums.insert(name);
	return this;
}

Ref<LuaSandboxConfig> LuaSandboxConfig::unblock_enum(const StringName &name) {
	blocked_enums.erase(name);
	return this;
}

bool LuaSandboxConfig::is_enum_blocked(const StringName &name) const {
	return blocked_enums.has(name);
}

PackedStringArray LuaSandboxConfig::get_blocked_enums() const {
	PackedStringArray arr;
	for (const StringName &name : blocked_enums) {
		arr.append(name);
	}
	return arr;
}

Ref<LuaSandboxConfig> LuaSandboxConfig::block_library(const StringName &name) {
	blocked_libraries.insert(name);
	return this;
}

Ref<LuaSandboxConfig> LuaSandboxConfig::unblock_library(const StringName &name) {
	blocked_libraries.erase(name);
	return this;
}

bool LuaSandboxConfig::is_library_blocked(const StringName &name) const {
	return blocked_libraries.has(name);
}

PackedStringArray LuaSandboxConfig::get_blocked_libraries() const {
	PackedStringArray arr;
	for (const StringName &name : blocked_libraries) {
		arr.append(name);
	}
	return arr;
}

Ref<LuaSandboxConfig> LuaSandboxConfig::block_library_function(const StringName &func) {
	blocked_library_functions.insert(func);
	return this;
}

Ref<LuaSandboxConfig> LuaSandboxConfig::unblock_library_function(const StringName &func) {
	blocked_library_functions.erase(func);
	return this;
}

bool LuaSandboxConfig::is_library_function_blocked(const StringName &func) const {
	return blocked_library_functions.has(func);
}

PackedStringArray LuaSandboxConfig::get_blocked_library_functions() const {
	PackedStringArray arr;
	for (const StringName &func : blocked_library_functions) {
		arr.append(func);
	}
	return arr;
}

} //namespace luagdextension
