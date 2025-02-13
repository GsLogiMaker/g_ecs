
#ifndef GLECS_UTILS_H
#define GLECS_UTILS_H

#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/variant.hpp"
#include <cctype>
#include <flecs.h>
#include <godot_cpp/variant/utility_functions.hpp>

#define WARN(...)	\
	UtilityFunctions::print_rich("[color='INDIAN_RED']GLECS: ", __VA_ARGS__);	\
	UtilityFunctions::push_error(__VA_ARGS__);
#define ERR(return_value, ...)	\
	WARN(__VA_ARGS__);	\
	return return_value;

#define CHECK_ENTITY_ALIVE(entity, gf_world, return_value, ...)	\
	if (!gf_world->is_id_alive(entity)) {	\
		ERR(return_value,	\
			__VA_ARGS__,	\
			"	ID ", entity, " is not alive"	\
		);	\
	}

#define CHECK_NOT_HAS_CHILD(entity, child_name, gf_world, return_value, err)	\
	if (gf_world->id_has_child(entity, child_name)) {	\
		ERR(return_value,	\
			err,	\
			"	", gf_world->id_to_text(entity) ," already has a child named	\"", child_name, "\""	\
		);	\
	}

#define CHECK_NOT_PAIR(entity, gf_world, return_value, ...)	\
	if (!gf_world->is_id_alive(entity)) {	\
		ERR(return_value,	\
			__VA_ARGS__,	\
			"	Entity ", gf_world->id_to_text(entity), " is a pair"	\
		);	\
	}

template <typename T, typename E>
class Result {

public:
	Result(T value_): value(value_) {
		_is_ok = true;
	}
	Result(E error_): error(error_) {
		_is_ok = false;
	}
	~Result() {
		if (is_ok()) {
			value.~T();
		} else {
			error.~E();
		}
	}

	static Result Ok(T value) {
		return Result(value);
	}
	static Result Err(E error) {
		return Result(error);
	}

	bool is_ok() {
		return _is_ok;
	}
	T unwrap() {
		if (!is_ok()) {
			throw "Tried to convert Error to Ok";
		}
		return value;
	};
	E unwrap_err() {
		if (is_ok()) {
			throw "Tried to convert Ok to Error";
		}
		return error;
	};

private:
	bool _is_ok;
	union {
        T value;
        E error;
    };
};


namespace godot {
	typedef Result<ecs_entity_t, String> EntityResult;
	typedef Result<int8_t, String> VoidResult;

	class Utils {
	public:
		static VoidResult check_variant_matches(Variant value, Variant::Type type) {
			if (value.get_type() == Variant::NIL && type == Variant::OBJECT) {
				return VoidResult::Ok(0);
			}
			if (value.get_type() != type) {
				return VoidResult(
					String("Expected variant value \"") + String(value)
					+ String("\" to be of type ") + Variant::get_type_name(type)
					+ String(", but it is of type ")
					+ Variant::get_type_name(value.get_type())
				);
			}
			return VoidResult::Ok(0);
		}

		/// Converts a Variant::Type to an Entity ID
		static Variant primitive_value_to_variant(const void*, ecs_primitive_kind_t);
		static Variant::Type primitive_type_to_variant(ecs_primitive_kind_t);
		static EntityResult variant_type_to_id(Variant::Type type);
		static bool can_convert_type_to_primitive(Variant::Type type, ecs_primitive_kind_t primi);

		static String into_pascal_case(String str);

		static void set_gd_struct_from_variant(const Variant value, const ecs_entity_t, void* out);
		static void set_primitive_from_variant(const Variant value, const ecs_primitive_kind_t, void* out);
		static void set_type_from_variant(const Variant value, const ecs_entity_t, const ecs_world_t*, void* out);

	};

}

#endif
