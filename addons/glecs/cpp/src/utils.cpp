
#include "utils.h"
#include "godot_cpp/core/defs.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "world.h"

#include <flecs.h>

using namespace godot;

Variant::Type Utils::primitive_type_to_variant(ecs_primitive_kind_t primi_kind) {
	switch (primi_kind) {
	case EcsBool: return Variant::BOOL;
	case EcsU8: return Variant::INT;
	case EcsU16: return Variant::INT;
	case EcsU32: return Variant::INT;
	case EcsU64: return Variant::INT;
	case EcsI8: return Variant::INT;
	case EcsI16: return Variant::INT;
	case EcsI32: return Variant::INT;
	case EcsI64: return Variant::INT;
	case EcsF32: return Variant::FLOAT;
	case EcsF64: return Variant::FLOAT;
	case EcsString: return Variant::STRING;
	case EcsId: return Variant::INT;
	case EcsChar: ERR(Variant::NIL, "Can't handle char primitive type");
	case EcsByte: ERR(Variant::NIL, "Can't handle byte primitive type");
	case EcsUPtr: ERR(Variant::NIL, "Can't handle UPointer primitive type");
	case EcsIPtr: ERR(Variant::NIL, "Can't handle IPointer primitive type");
	case EcsEntity: ERR(Variant::NIL, "Can't handle entity primitive type");
	}
	ERR(Variant::NIL,
		"Unreachable\n"
	);
}

Variant Utils::primitive_value_to_variant(const void* primi_ptr, ecs_primitive_kind_t primi_kind) {
	switch (primi_kind) {
	case EcsBool: return Variant(); return reinterpret_cast<const bool*>(primi_ptr);
	case EcsU8: return reinterpret_cast<const uint8_t*>(primi_ptr);
	case EcsU16: return reinterpret_cast<const uint16_t*>(primi_ptr);
	case EcsU32: return reinterpret_cast<const uint32_t*>(primi_ptr);
	case EcsU64: return reinterpret_cast<const uint64_t*>(primi_ptr);
	case EcsI8: return reinterpret_cast<const int8_t*>(primi_ptr);
	case EcsI16: return reinterpret_cast<const int16_t*>(primi_ptr);
	case EcsI32: return reinterpret_cast<const int32_t*>(primi_ptr);
	case EcsI64: return reinterpret_cast<const int64_t*>(primi_ptr);
	case EcsF32: return reinterpret_cast<const real_t*>(primi_ptr);
	case EcsF64: return reinterpret_cast<const double*>(primi_ptr);
	case EcsId: return reinterpret_cast<const ecs_entity_t*>(primi_ptr);
	case EcsChar: ERR(Variant(), "Can not convert char primitive type to Variant");
	case EcsByte: ERR(Variant(), "Can not convert byte primitive type to Variant");
	case EcsUPtr: ERR(Variant(), "Can not convert UPointer primitive type to Variant");
	case EcsIPtr: ERR(Variant(), "Can not convert IPointer primitive type to Variant");
	case EcsString: ERR(Variant(), "Can not convert string primitive type to Variant");
	case EcsEntity: ERR(Variant(), "Can not convert entity primitive type to Variant");
	}
	ERR(Variant(),
		"Unreachable\n"
	);
}

/// Converts a godot::Variant::Type to Flecs's closest Entity type
EntityResult Utils::variant_type_to_id(Variant::Type type) {
	switch (type) {
		case Variant::BOOL: return EntityResult::Ok(ecs_id(ecs_bool_t));
		case Variant::INT: return EntityResult::Ok(ecs_id(ecs_i64_t));
		case Variant::FLOAT: return EntityResult::Ok(ecs_id(ecs_f64_t));
		case Variant::VECTOR2: return EntityResult::Ok(GFWorld::glecs_meta_vector2);
		case Variant::VECTOR2I: return EntityResult::Ok(GFWorld::glecs_meta_vector2i);
		case Variant::RECT2: return EntityResult::Ok(GFWorld::glecs_meta_rect2);
		case Variant::RECT2I: return EntityResult::Ok(GFWorld::glecs_meta_rect2i);
		case Variant::VECTOR3: return EntityResult::Ok(GFWorld::glecs_meta_vector3);
		case Variant::VECTOR3I: return EntityResult::Ok(GFWorld::glecs_meta_vector3i);
		case Variant::TRANSFORM2D: return EntityResult::Ok(GFWorld::glecs_meta_transform2d);
		case Variant::VECTOR4: return EntityResult::Ok(GFWorld::glecs_meta_vector4);
		case Variant::VECTOR4I: return EntityResult::Ok(GFWorld::glecs_meta_vector4i);
		case Variant::PLANE: return EntityResult::Ok(GFWorld::glecs_meta_plane);
		case Variant::QUATERNION: return EntityResult::Ok(GFWorld::glecs_meta_quaternion);
		case Variant::AABB: return EntityResult::Ok(GFWorld::glecs_meta_aabb);
		case Variant::BASIS: return EntityResult::Ok(GFWorld::glecs_meta_basis);
		case Variant::TRANSFORM3D: return EntityResult::Ok(GFWorld::glecs_meta_transform3d);
		case Variant::PROJECTION: return EntityResult::Ok(GFWorld::glecs_meta_projection);
		case Variant::COLOR: return EntityResult::Ok(GFWorld::glecs_meta_color);

		case Variant::STRING_NAME: return EntityResult::Ok(GFWorld::glecs_meta_string_name);
		case Variant::NODE_PATH: return EntityResult::Ok(GFWorld::glecs_meta_node_path);
		case Variant::RID: return EntityResult::Ok(GFWorld::glecs_meta_rid);
		case Variant::OBJECT: return EntityResult::Ok(GFWorld::glecs_meta_object);
		case Variant::CALLABLE: return EntityResult::Ok(GFWorld::glecs_meta_callable);
		case Variant::SIGNAL: return EntityResult::Ok(GFWorld::glecs_meta_signal);
		case Variant::DICTIONARY: return EntityResult::Ok(GFWorld::glecs_meta_dictionary);
		case Variant::ARRAY: return EntityResult::Ok(GFWorld::glecs_meta_array);
		case Variant::PACKED_INT32_ARRAY: return EntityResult::Ok(GFWorld::glecs_meta_packed_int32_array);
		case Variant::PACKED_INT64_ARRAY: return EntityResult::Ok(GFWorld::glecs_meta_packed_int64_array);
		case Variant::PACKED_FLOAT32_ARRAY: return EntityResult::Ok(GFWorld::glecs_meta_packed_float32_array);
		case Variant::PACKED_FLOAT64_ARRAY: return EntityResult::Ok(GFWorld::glecs_meta_packed_float64_array);
		case Variant::PACKED_STRING_ARRAY: return EntityResult::Ok(GFWorld::glecs_meta_packed_string_array);
		case Variant::PACKED_VECTOR2_ARRAY: return EntityResult::Ok(GFWorld::glecs_meta_packed_vector2_array);
		case Variant::PACKED_VECTOR3_ARRAY: return EntityResult::Ok(GFWorld::glecs_meta_packed_vector3_array);
		case Variant::PACKED_COLOR_ARRAY: return EntityResult::Ok(GFWorld::glecs_meta_packed_color_array);

		default:
			return EntityResult::Err(
				String("Could not convert Variant type ")
				+ Variant::get_type_name(type)
				+ " to entity ID"
			);
	}
}

void Utils::set_gd_struct_from_variant(const Variant value, const ecs_entity_t gd_struct, void* out) {
	Variant::Type type = GFWorld::id_to_variant_type(gd_struct);
	switch (type) {
        case Variant::NIL: ERR(/**/, "Can't set nil");
        case Variant::BOOL: *reinterpret_cast<bool*>(out) = value; break;
        case Variant::INT: *reinterpret_cast<int64_t*>(out) = value; break;
        case Variant::FLOAT: *reinterpret_cast<double*>(out) = value; break;
        case Variant::STRING: *reinterpret_cast<String*>(out) = value; break;
        case Variant::VECTOR2: *reinterpret_cast<Vector2*>(out) = value; break;
        case Variant::VECTOR2I: *reinterpret_cast<Vector2i*>(out) = value; break;
        case Variant::RECT2: *reinterpret_cast<Rect2*>(out) = value; break;
        case Variant::RECT2I: *reinterpret_cast<Rect2i*>(out) = value; break;
        case Variant::VECTOR3: *reinterpret_cast<Vector3*>(out) = value; break;
        case Variant::VECTOR3I: *reinterpret_cast<Vector3i*>(out) = value; break;
        case Variant::TRANSFORM2D: *reinterpret_cast<Transform2D*>(out) = value; break;
        case Variant::VECTOR4: *reinterpret_cast<Vector4*>(out) = value; break;
        case Variant::VECTOR4I: *reinterpret_cast<Vector4i*>(out) = value; break;
        case Variant::PLANE: *reinterpret_cast<Plane*>(out) = value; break;
        case Variant::QUATERNION: *reinterpret_cast<Quaternion*>(out) = value; break;
        case Variant::AABB: *reinterpret_cast<AABB*>(out) = value; break;
        case Variant::BASIS: *reinterpret_cast<Basis*>(out) = value; break;
        case Variant::TRANSFORM3D: *reinterpret_cast<Transform3D*>(out) = value; break;
        case Variant::PROJECTION: *reinterpret_cast<Projection*>(out) = value; break;
        case Variant::COLOR: *reinterpret_cast<Color*>(out) = value; break;
        case Variant::STRING_NAME: *reinterpret_cast<StringName*>(out) = value; break;
        case Variant::NODE_PATH: *reinterpret_cast<NodePath*>(out) = value; break;
        case Variant::RID: *reinterpret_cast<RID*>(out) = value; break;
        case Variant::OBJECT: *reinterpret_cast<Variant*>(out) = value; break;
        case Variant::CALLABLE: *reinterpret_cast<Callable*>(out) = value; break;
        case Variant::SIGNAL: *reinterpret_cast<Signal*>(out) = value; break;
        case Variant::DICTIONARY: *reinterpret_cast<Dictionary*>(out) = value; break;
        case Variant::ARRAY: *reinterpret_cast<Array*>(out) = value; break;
        case Variant::PACKED_BYTE_ARRAY: *reinterpret_cast<PackedByteArray*>(out) = value; break;
        case Variant::PACKED_INT32_ARRAY: *reinterpret_cast<PackedInt32Array*>(out) = value; break;
        case Variant::PACKED_INT64_ARRAY: *reinterpret_cast<PackedInt64Array*>(out) = value; break;
        case Variant::PACKED_FLOAT32_ARRAY: *reinterpret_cast<PackedFloat32Array*>(out) = value; break;
        case Variant::PACKED_FLOAT64_ARRAY: *reinterpret_cast<PackedFloat64Array*>(out) = value; break;
        case Variant::PACKED_STRING_ARRAY: *reinterpret_cast<PackedStringArray*>(out) = value; break;
        case Variant::PACKED_VECTOR2_ARRAY: *reinterpret_cast<PackedVector2Array*>(out) = value; break;
        case Variant::PACKED_VECTOR3_ARRAY: *reinterpret_cast<PackedVector3Array*>(out) = value; break;
        case Variant::PACKED_COLOR_ARRAY: *reinterpret_cast<PackedColorArray*>(out) = value; break;
        case Variant::PACKED_VECTOR4_ARRAY: *reinterpret_cast<PackedVector4Array*>(out) = value; break;
        case Variant::VARIANT_MAX: ERR(/**/, "Can't set Variant::VARIANT_MAX");
	}
}

void Utils::set_primitive_from_variant(
	const Variant value,
	const ecs_primitive_kind_t primi_kind,
	void* out
) {
	switch (primi_kind) {
	case EcsBool: *reinterpret_cast<bool*>(out) = value; break;
	case EcsChar: ERR(/**/, "Can not convert to char primitive from Variant type ", Variant::get_type_name(value.get_type()));
	case EcsByte: ERR(/**/, "Can not convert to byte primitive from Variant type ", Variant::get_type_name(value.get_type()));
	case EcsU8: *reinterpret_cast<uint8_t*>(out) = value; break;
	case EcsU16: *reinterpret_cast<uint16_t*>(out) = value; break;
	case EcsU32: *reinterpret_cast<uint32_t*>(out) = value; break;
	case EcsU64: *reinterpret_cast<uint64_t*>(out) = value; break;
	case EcsI8: *reinterpret_cast<int8_t*>(out) = value; break;
	case EcsI16: *reinterpret_cast<int16_t*>(out) = value; break;
	case EcsI32: *reinterpret_cast<int32_t*>(out) = value; break;
	case EcsI64: *reinterpret_cast<int64_t*>(out) = value; break;
	case EcsF32: *reinterpret_cast<real_t*>(out) = value; break;
	case EcsF64: *reinterpret_cast<double*>(out) = value; break;
	case EcsUPtr: ERR(/**/, "Can not convert to UPointer primitive from Variant type ", Variant::get_type_name(value.get_type()));
	case EcsIPtr: ERR(/**/, "Can not convert to IPointer primitive from Variant type ", Variant::get_type_name(value.get_type()));
	case EcsString: ERR(/**/, "Can not convert to string primitive from Variant type ", Variant::get_type_name(value.get_type()));
	case EcsEntity: ERR(/**/, "Can not convert to entity primitive from Variant type ", Variant::get_type_name(value.get_type()));
	case EcsId: *reinterpret_cast<ecs_entity_t*>(out) = value; break;
	}
}

void Utils::set_type_from_variant(
	const Variant value,
	const ecs_entity_t type,
	const ecs_world_t* world,
	void* out
) {
	const EcsPrimitive* primi = ecs_get(world, type, EcsPrimitive);
	if (primi != nullptr) {
		set_primitive_from_variant(value, primi->kind, out);
		return;
	}

	const EcsStruct* stru = ecs_get(world, type, EcsStruct);
	if (stru != nullptr) {
		set_gd_struct_from_variant(value, type, out);
		return;
	}

	ERR(/**/,
		"Could not set entity type from variant\n",
		"Entity type is not a primitive or struct."
	);
}

String Utils::into_pascal_case(String str) {
	String new_str = "";
	bool next_upper = true;
	for (int i=0; i != str.length(); i++) {
		char32_t character = str[i];
		if (!isalpha(character) && !(character >= '0' && character <= '9')) {
			//Character is not letter or number, don't add to new
			next_upper = true;
			continue;
		}
		if (next_upper) {
			// Add uppercase letter
			new_str += toupper(character);
			next_upper = false;
		} else {
			// Add normal letter
			new_str += tolower(character);
		}
		if (!isalpha(character)) {
			next_upper = true;
		}
	}
	return new_str;
}
