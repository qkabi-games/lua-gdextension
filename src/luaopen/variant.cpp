/**
 * Copyright (C) 2026 Gil Barbosa Reis.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the “Software”), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <sol/sol.hpp>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "../LuaState.hpp"
#include "../LuaSandboxConfig.hpp"
#include "../utils/Class.hpp"
#include "../utils/DictionaryIterator.hpp"
#include "../utils/IndexedIterator.hpp"
#include "../utils/ObjectIterator.hpp"
#include "../utils/VariantType.hpp"
#include "../utils/convert_godot_lua.hpp"
#include "../utils/convert_godot_std.hpp"
#include "../utils/function_wrapper.hpp"
#include "../utils/method_bind_impl.hpp"
#include "../utils/string_names.hpp"

using namespace godot;

namespace luagdextension {

template<Variant::Operator VarOperator>
sol::object evaluate_binary_operator(sol::this_state state, const sol::stack_object& a, const sol::stack_object& b) {
	bool is_valid;
	Variant result;
	Variant var_a = to_variant(a);
	Variant var_b = to_variant(b);
	Variant::evaluate(VarOperator, var_a, var_b, result, is_valid);
	if (!is_valid) {
		CharString a_str = get_type_name(var_a).ascii();
		CharString b_str = get_type_name(var_b).ascii();
		luaL_error(
			state,
			"Invalid call to operator '%s' between %s and %s.",
			get_operator_name(VarOperator),
			a_str.get_data(),
			b_str.get_data()
		);
	}
	return to_lua(state, result);
}

template<Variant::Operator VarOperator>
sol::object evaluate_unary_operator(sol::this_state state, const sol::stack_object& a) {
	bool is_valid;
	Variant result;
	Variant var_a = to_variant(a);
	Variant::evaluate(VarOperator, var_a, Variant(), result, is_valid);
	if (!is_valid) {
		CharString a_str = get_type_name(var_a).ascii();
		luaL_error(
			state,
			"Invalid call to operator %s with type %s.",
			get_operator_name(VarOperator),
			a_str.get_data()
		);
	}
	return to_lua(state, result);
}

sol::object variant__index(sol::this_state state, const Variant& variant, const sol::stack_object& key) {
	bool is_valid;
	if (key.get_type() == sol::type::string) {
		StringName string_name = key.as<StringName>();
		if (Variant::has_member(variant.get_type(), string_name)) {
			return to_lua(state, variant.get_named(string_name, is_valid));
		}
		else if (variant.has_method(string_name)) {
			return sol::make_object(state, VariantMethodBind(variant, string_name));
		}
	}

	Variant result = variant.get(to_variant(key), &is_valid);
	return to_lua(state, result);
}

void variant__newindex(sol::this_state state, Variant& variant, const sol::stack_object& key, const sol::stack_object& value) {
	bool is_valid;
	Variant var_key = to_variant(key);
	Variant var_value = to_variant(value);
	variant.set(var_key, var_value, &is_valid);
	if (!is_valid) {
		CharString key_str = var_key.stringify().utf8();
		CharString variant_str = get_type_name(variant).ascii();
		luaL_error(
			state,
			"Could not set value for key '%s' with an object of type %s",
			key_str.get_data(),
			variant_str.get_data()
		);
	}
}

sol::object variant__length(sol::this_state state, Variant& variant) {
	return variant_call_string_name(state, variant, string_names->size, sol::variadic_args(state, 0));
}

String variant__concat(const sol::stack_object& a, const sol::stack_object& b) {
	return String(to_variant(a)) + String(to_variant(b));
}

std::tuple<sol::object, sol::object> variant__pairs(sol::this_state state, const Variant& variant) {
	if (variant.get_type() == Variant::DICTIONARY) {
		return DictionaryIterator::dictionary_pairs(state, variant);
	}

	if (IndexedIterator::supports_indexed_pairs(variant)) {
		return IndexedIterator::indexed_pairs(state, variant);
	}

	return ObjectIterator::object_pairs(state, variant);
}

VariantType variant_get_type(const sol::stack_object& self) {
	return VariantType(to_variant(self).get_type());
}

bool variant_is(const sol::stack_object& self, const sol::stack_object& type) {
	Variant variant = to_variant(self);
	if (type.get_type() == sol::type::nil) {
		return variant.get_type() == Variant::NIL;
	}
	else if (type.get_type() == sol::type::string) {
		if (variant.get_type() == Variant::OBJECT) {
			Object *obj = variant;
			return obj->is_class(type.as<String>());
		}
		else {
			return Variant::get_type_name(variant.get_type()) == type.as<String>();
		}
	}
	else if (type.is<VariantType>()) {
		return variant.get_type() == type.as<VariantType>().get_type();
	}
	else if (type.is<Class>() && variant.get_type() == Variant::OBJECT) {
		Object *obj = variant;
		return obj->is_class(type.as<Class>().get_name());
	}
	return false;
}

sol::object variant__call(sol::this_state state, const Variant& variant, sol::variadic_args args) {
	if (variant.get_type() != Variant::CALLABLE) {
		luaL_error(state, "attempt to call a %s value", get_type_name(variant).ascii().get_data());
	}
	Variant result = callable_call(variant, args);
	return to_lua(state, result);
}

}

using namespace luagdextension;

static void register_variant_type(sol::state_view &state, LuaSandboxConfig *config, const char *name, Variant::Type type) {
	if (config && config->is_variant_type_blocked(name)) {
		return;
	}
	state.set(name, VariantType(type));
}

extern "C" int luaopen_godot_variant(lua_State *L) {
	sol::state_view state = L;

	LuaSandboxConfig *config = nullptr;
	if (LuaState *lua_state = LuaState::find_lua_state(state)) {
		config = lua_state->get_sandbox_config().ptr();
	}

	state.new_usertype<Variant>(
		"Variant",
		sol::call_constructor, sol::constructors<
			Variant(),
			Variant(const Variant &other),
			Variant(Variant &&other),
			Variant(bool v),
			Variant(int64_t v),
			Variant(float v),
			Variant(double v),
			Variant(const char *v)
		>(),
		"booleanize", wrap_function(L, +[](const Variant& v) { return v.booleanize(); }),
		"duplicate", wrap_function(L, +[](const Variant& self) { return self.duplicate(); }),
		"call", &variant_call,
		"pcall", &variant_pcall,
		"get_type", &variant_get_type,
		"get_type_name", wrap_function(L, &get_type_name),
		"hash", wrap_function(L, +[](const Variant& self) { return self.hash(); }),
		"recursive_hash", wrap_function(L, +[](const Variant& self, int recursion_count) { return self.recursive_hash(recursion_count); }),
		"hash_compare", wrap_function(L, +[](const Variant& self, const Variant& other) { return self.hash_compare(other); }),
		"is", &variant_is,
		// comparison
		sol::meta_function::equal_to, &evaluate_binary_operator<Variant::OP_EQUAL>,
		sol::meta_function::less_than, &evaluate_binary_operator<Variant::OP_LESS>,
		sol::meta_function::less_than_or_equal_to, &evaluate_binary_operator<Variant::OP_LESS_EQUAL>,
		// mathematic
		sol::meta_function::addition, &evaluate_binary_operator<Variant::OP_ADD>,
		sol::meta_function::subtraction, &evaluate_binary_operator<Variant::OP_SUBTRACT>,
		sol::meta_function::multiplication, &evaluate_binary_operator<Variant::OP_MULTIPLY>,
		sol::meta_function::division, &evaluate_binary_operator<Variant::OP_DIVIDE>,
		sol::meta_function::power_of, &evaluate_binary_operator<Variant::OP_POWER>,
		sol::meta_function::modulus, &evaluate_binary_operator<Variant::OP_MODULE>,
		sol::meta_function::unary_minus, &evaluate_unary_operator<Variant::OP_NEGATE>,
		// bitwise
		sol::meta_function::bitwise_left_shift, &evaluate_binary_operator<Variant::OP_SHIFT_LEFT>,
		sol::meta_function::bitwise_right_shift, &evaluate_binary_operator<Variant::OP_SHIFT_RIGHT>,
		sol::meta_function::bitwise_and, &evaluate_binary_operator<Variant::OP_BIT_AND>,
		sol::meta_function::bitwise_or, &evaluate_binary_operator<Variant::OP_BIT_OR>,
		sol::meta_function::bitwise_xor, &evaluate_binary_operator<Variant::OP_BIT_XOR>,
		sol::meta_function::bitwise_not, &evaluate_unary_operator<Variant::OP_BIT_NEGATE>,
		// misc
		sol::meta_function::call, &variant__call,
		sol::meta_function::index, &variant__index,
		sol::meta_function::new_index, &variant__newindex,
		sol::meta_function::length, &variant__length,
		sol::meta_function::concatenation, &variant__concat,
		sol::meta_function::pairs, &variant__pairs,
		sol::meta_function::to_string, &Variant::stringify
	);

	VariantMethodBind::register_usertype(state);
	VariantType::register_usertype(state);

	state.set("typeof", &variant_get_type);

	// atomic types
	state.set("bool", VariantType(Variant::BOOL));
	state.set("int", VariantType(Variant::INT));
	state.set("float", VariantType(Variant::FLOAT));
	state.set("String", VariantType(Variant::STRING));
	// Add String methods as a fallback for Lua strings
	state.do_string("if string then setmetatable(string, { __index = String }) end");

	// math types
	register_variant_type(state, config, "Vector2", Variant::VECTOR2);
	register_variant_type(state, config, "Vector2i", Variant::VECTOR2I);
	register_variant_type(state, config, "Rect2", Variant::RECT2);
	register_variant_type(state, config, "Rect2i", Variant::RECT2I);
	register_variant_type(state, config, "Vector3", Variant::VECTOR3);
	register_variant_type(state, config, "Vector3i", Variant::VECTOR3I);
	register_variant_type(state, config, "Transform2D", Variant::TRANSFORM2D);
	register_variant_type(state, config, "Vector4", Variant::VECTOR4);
	register_variant_type(state, config, "Vector4i", Variant::VECTOR4I);
	register_variant_type(state, config, "Plane", Variant::PLANE);
	register_variant_type(state, config, "Quaternion", Variant::QUATERNION);
	register_variant_type(state, config, "AABB", Variant::AABB);
	register_variant_type(state, config, "Basis", Variant::BASIS);
	register_variant_type(state, config, "Transform3D", Variant::TRANSFORM3D);
	register_variant_type(state, config, "Projection", Variant::PROJECTION);

	// misc types
	register_variant_type(state, config, "Color", Variant::COLOR);
	register_variant_type(state, config, "StringName", Variant::STRING_NAME);
	register_variant_type(state, config, "NodePath", Variant::NODE_PATH);
	register_variant_type(state, config, "RID", Variant::RID);
	register_variant_type(state, config, "Callable", Variant::CALLABLE);
	register_variant_type(state, config, "Signal", Variant::SIGNAL);
	register_variant_type(state, config, "Dictionary", Variant::DICTIONARY);
	register_variant_type(state, config, "Array", Variant::ARRAY);

	// typed arrays
	register_variant_type(state, config, "PackedByteArray", Variant::PACKED_BYTE_ARRAY);
	register_variant_type(state, config, "PackedInt32Array", Variant::PACKED_INT32_ARRAY);
	register_variant_type(state, config, "PackedInt64Array", Variant::PACKED_INT64_ARRAY);
	register_variant_type(state, config, "PackedFloat32Array", Variant::PACKED_FLOAT32_ARRAY);
	register_variant_type(state, config, "PackedFloat64Array", Variant::PACKED_FLOAT64_ARRAY);
	register_variant_type(state, config, "PackedStringArray", Variant::PACKED_STRING_ARRAY);
	register_variant_type(state, config, "PackedVector2Array", Variant::PACKED_VECTOR2_ARRAY);
	register_variant_type(state, config, "PackedVector3Array", Variant::PACKED_VECTOR3_ARRAY);
	register_variant_type(state, config, "PackedColorArray", Variant::PACKED_COLOR_ARRAY);
	register_variant_type(state, config, "PackedVector4Array", Variant::PACKED_VECTOR4_ARRAY);

	return 0;
}
