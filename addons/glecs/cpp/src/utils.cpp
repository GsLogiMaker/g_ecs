
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
	case EcsBool: return (bool*)(primi_ptr);
	case EcsU8: return (uint8_t*)(primi_ptr);
	case EcsU16: return (uint16_t*)(primi_ptr);
	case EcsU32: return (uint32_t*)(primi_ptr);
	case EcsU64: return (uint64_t*)(primi_ptr);
	case EcsI8: return (int8_t*)(primi_ptr);
	case EcsI16: return (int16_t*)(primi_ptr);
	case EcsI32: return (int32_t*)(primi_ptr);
	case EcsI64: return (int64_t*)(primi_ptr);
	case EcsF32: return (real_t*)(primi_ptr);
	case EcsF64: return (double*)(primi_ptr);
	case EcsId: return (ecs_entity_t*)(primi_ptr);
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

void Utils::set_gd_struct_from_variant(
	const Variant value,
	const ecs_entity_t gd_struct,
	void* out
) {
	Variant::Type type = GFWorld::id_to_variant_type(gd_struct);
	switch (type) {
        case Variant::NIL: ERR(/**/, "Can't set nil");
        case Variant::BOOL: *((bool*)out) = value; break;
        case Variant::INT: *((int64_t*)out) = value; break;
        case Variant::FLOAT: *(double*)(out) = value; break;
        case Variant::STRING: *((String*)out) = value; break;
        case Variant::VECTOR2: *((Vector2*)out) = value; break;
        case Variant::VECTOR2I: *((Vector2i*)out) = value; break;
        case Variant::RECT2: *((Rect2*)out) = value; break;
        case Variant::RECT2I: *((Rect2i*)out) = value; break;
        case Variant::VECTOR3: *((Vector3*)out) = value; break;
        case Variant::VECTOR3I: *((Vector3i*)out) = value; break;
        case Variant::TRANSFORM2D: *((Transform2D*)out) = value; break;
        case Variant::VECTOR4: *((Vector4*)out) = value; break;
        case Variant::VECTOR4I: *((Vector4i*)out) = value; break;
        case Variant::PLANE: *((Plane*)out) = value; break;
        case Variant::QUATERNION: *((Quaternion*)out) = value; break;
        case Variant::AABB: *((AABB*)out) = value; break;
        case Variant::BASIS: *((Basis*)out) = value; break;
        case Variant::TRANSFORM3D: *((Transform3D*)out) = value; break;
        case Variant::PROJECTION: *((Projection*)out) = value; break;
        case Variant::COLOR: *((Color*)out) = value; break;
        case Variant::STRING_NAME: *((StringName*)out) = value; break;
        case Variant::NODE_PATH: *((NodePath*)out) = value; break;
        case Variant::RID: *((RID*)out) = value; break;
        case Variant::OBJECT: *((Variant*)out) = value; break;
        case Variant::CALLABLE: *((Callable*)out) = value; break;
        case Variant::SIGNAL: *((Signal*)out) = value; break;
        case Variant::DICTIONARY: *((Dictionary*)out) = value; break;
        case Variant::ARRAY: *((Array*)out) = value; break;
        case Variant::PACKED_BYTE_ARRAY: *((PackedByteArray*)out) = value; break;
        case Variant::PACKED_INT32_ARRAY: *((PackedInt32Array*)out) = value; break;
        case Variant::PACKED_INT64_ARRAY: *((PackedInt64Array*)out) = value; break;
        case Variant::PACKED_FLOAT32_ARRAY: *((PackedFloat32Array*)out) = value; break;
        case Variant::PACKED_FLOAT64_ARRAY: *((PackedFloat64Array*)out) = value; break;
        case Variant::PACKED_STRING_ARRAY: *((PackedStringArray*)out) = value; break;
        case Variant::PACKED_VECTOR2_ARRAY: *((PackedVector2Array*)out) = value; break;
        case Variant::PACKED_VECTOR3_ARRAY: *((PackedVector3Array*)out) = value; break;
        case Variant::PACKED_COLOR_ARRAY: *((PackedColorArray*)out) = value; break;
        case Variant::PACKED_VECTOR4_ARRAY: *((PackedVector4Array*)out) = value; break;
        case Variant::VARIANT_MAX: ERR(/**/, "Can't set Variant::VARIANT_MAX");
	}
}

void Utils::set_primitive_from_variant(
	const Variant value,
	const ecs_primitive_kind_t primi_kind,
	void* out
) {
	switch (primi_kind) {
	case EcsBool: *(bool*)(out) = value; break;
	case EcsChar: ERR(/**/, "Can not convert to char primitive from Variant type ", Variant::get_type_name(value.get_type()));
	case EcsByte: ERR(/**/, "Can not convert to byte primitive from Variant type ", Variant::get_type_name(value.get_type()));
	case EcsU8: *(uint8_t*)(out) = value; break;
	case EcsU16: *(uint16_t*)(out) = value; break;
	case EcsU32: *(uint32_t*)(out) = value; break;
	case EcsU64: *(uint64_t*)(out) = value; break;
	case EcsI8: *(int8_t*)(out) = value; break;
	case EcsI16: *(int16_t*)(out) = value; break;
	case EcsI32: *(int32_t*)(out) = value; break;
	case EcsI64: *(int64_t*)(out) = value; break;
	case EcsF32: *(real_t*)(out) = value; break;
	case EcsF64: *(double*)(out) = value; break;
	case EcsUPtr: ERR(/**/, "Can not convert to UPointer primitive from Variant type ", Variant::get_type_name(value.get_type()));
	case EcsIPtr: ERR(/**/, "Can not convert to IPointer primitive from Variant type ", Variant::get_type_name(value.get_type()));
	case EcsString: ERR(/**/, "Can not convert to string primitive from Variant type ", Variant::get_type_name(value.get_type()));
	case EcsEntity: ERR(/**/, "Can not convert to entity primitive from Variant type ", Variant::get_type_name(value.get_type()));
	case EcsId: *(ecs_entity_t*)(out) = value; break;
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

	set_gd_struct_from_variant(value, type, out);
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
