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
#ifndef __LUA_SANDBOX_CONFIG_HPP__
#define __LUA_SANDBOX_CONFIG_HPP__

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/templates/hash_set.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

using namespace godot;

namespace luagdextension {

class LuaSandboxConfig : public RefCounted {
	GDCLASS(LuaSandboxConfig, RefCounted);

private:
	HashSet<StringName> blocked_variant_types;
	HashSet<StringName> blocked_utility_functions;
	HashSet<StringName> blocked_classes;
	HashSet<StringName> blocked_singletons;
	HashSet<StringName> blocked_enums;
	HashSet<StringName> blocked_libraries;
	HashSet<StringName> blocked_library_functions;

protected:
	static void _bind_methods();

public:
	static Ref<LuaSandboxConfig> create();
	static Ref<LuaSandboxConfig> with_defaults();

	Ref<LuaSandboxConfig> block_variant_type(const StringName &type);
	Ref<LuaSandboxConfig> unblock_variant_type(const StringName &type);
	bool is_variant_type_blocked(const StringName &type) const;
	PackedStringArray get_blocked_variant_types() const;

	Ref<LuaSandboxConfig> block_utility_function(const StringName &func);
	Ref<LuaSandboxConfig> unblock_utility_function(const StringName &func);
	bool is_utility_function_blocked(const StringName &func) const;
	PackedStringArray get_blocked_utility_functions() const;

	Ref<LuaSandboxConfig> block_class(const StringName &cls);
	Ref<LuaSandboxConfig> unblock_class(const StringName &cls);
	bool is_class_blocked(const StringName &cls) const;
	PackedStringArray get_blocked_classes() const;

	Ref<LuaSandboxConfig> block_singleton(const StringName &name);
	Ref<LuaSandboxConfig> unblock_singleton(const StringName &name);
	bool is_singleton_blocked(const StringName &name) const;
	PackedStringArray get_blocked_singletons() const;

	Ref<LuaSandboxConfig> block_enum(const StringName &name);
	Ref<LuaSandboxConfig> unblock_enum(const StringName &name);
	bool is_enum_blocked(const StringName &name) const;
	PackedStringArray get_blocked_enums() const;

	Ref<LuaSandboxConfig> block_library(const StringName &name);
	Ref<LuaSandboxConfig> unblock_library(const StringName &name);
	bool is_library_blocked(const StringName &name) const;
	PackedStringArray get_blocked_libraries() const;

	Ref<LuaSandboxConfig> block_library_function(const StringName &func);
	Ref<LuaSandboxConfig> unblock_library_function(const StringName &func);
	bool is_library_function_blocked(const StringName &func) const;
	PackedStringArray get_blocked_library_functions() const;
};

} //namespace luagdextension

#endif
