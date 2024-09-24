
#include "component.h"
#include "component_builder.h"
#include "entity.h"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/packed_int64_array.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "registerable_entity.h"
#include "utils.h"

#include <cstddef>
#include <cstdint>
#include <stdint.h>
#include <flecs.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

GFComponent::GFComponent() {
}
GFComponent::~GFComponent() {
}

Ref<GFComponent> GFComponent::spawn(GFWorld* world_) {
	ERR(NULL,
		"Could not instantiate ", get_class_static(), "\n",
		"Use ", get_class_static(), ".from or ",
		get_class_static(), ".from_id instead."
	);
}
Ref<GFComponent> GFComponent::from(Variant comp, Variant entity, GFWorld* world) {
	return from_id(world->coerce_id(comp), world->coerce_id(entity), world);
}
Ref<GFComponent> GFComponent::from_id(ecs_entity_t comp, ecs_entity_t entity, GFWorld* world) {
	if (!ecs_has_id(world->raw(), comp, ecs_id(EcsComponent))) {
		ERR(nullptr,
			"Could not instantiate ", get_class_static(), "\n",
			"ID is not a component"
		);
	}
	Ref<GFComponent> component = from_id_template<GFComponent>(comp, world);
	component->set_source_id(entity);
	return component;
}

void GFComponent::_register_internal() {
	// Build component
	Ref<GFComponentBuilder> b = get_world()->component_builder();
	if (GDVIRTUAL_IS_OVERRIDDEN(_build)) {
		b->set_entity(get_id());
		GDVIRTUAL_CALL(_build, b);
		if (!b->is_built()) {
			b->build();
		}
	}

	// Call super method
	GFRegisterableEntity::_register_internal();
}

void GFComponent::setm(String member, Variant value) {
	ecs_world_t* raw = get_world()->raw();

	// Get member data
	const EcsMember* member_data = get_member_data(member);
	if (member_data == nullptr) {
		// Member data is null. This should never happen.
		ERR(/**/,
			"Member metadata is null"
		);
	}
	void* member_ptr = get_member_ptr_mut_at(member_data->offset);
	if (member_ptr == nullptr) {
		ERR(/**/,
			"Member pointer is null"
		);
	}

	// Return member
	Utils::set_type_from_variant(value, member_data->type, raw, member_ptr);
	ecs_modified_id(get_world()->raw(), get_source_id(), get_id());
}

Variant GFComponent::getm(String member) {
	ecs_world_t* raw = get_world()->raw();

	// Get member data
	const EcsMember* member_data = get_member_data(member);
	if (member_data == nullptr) {
		// Member data is null. This should never happen.
		ERR(nullptr,
			"Member metadata is null"
		);
	}
	void* member_value_ptr = get_member_ptr_mut_at(member_data->offset);
	if (member_value_ptr == nullptr) {
		ERR(nullptr,
			"Member value is null"
		);
	}

	return member_value_as_type(member_value_ptr, member_data->type);
}

void* GFComponent::get_member_ptr_mut_at(int offset) {
	ecs_world_t* raw = get_world()->raw();
	int8_t* bytes = (int8_t*) ecs_get_mut_id(raw, get_source_id(), get_id());
	return (void*) &bytes[offset];
}

const EcsMember* GFComponent::get_member_data(String member) {
	ecs_world_t* raw = get_world()->raw();
	const char* c_str = member.utf8().get_data();

	// Get member ID
	ecs_entity_t member_id = ecs_lookup_child(raw, get_id(), c_str);

	if (member_id == 0) {
		ERR(nullptr,
			"No member named \"",
			member,
			"\" found in component \"",
			ecs_get_name(raw, get_id()),
			"\""
		);
	}

	// Get member data
	const EcsMember* member_data = ecs_get(raw, member_id, EcsMember);

	return member_data;
}

Variant GFComponent::member_value_as_primitive(
	void* ptr,
	ecs_primitive_kind_t primitive
) {
	switch (primitive) {
		case ecs_primitive_kind_t::EcsBool: return *(bool*) ptr;
		case ecs_primitive_kind_t::EcsChar: return *(char*) ptr;
		case ecs_primitive_kind_t::EcsByte: return *(uint8_t*) ptr;
		case ecs_primitive_kind_t::EcsU8: return *(uint8_t*) ptr;
		case ecs_primitive_kind_t::EcsU16: return *(uint16_t*) ptr;
		case ecs_primitive_kind_t::EcsU32: return *(uint32_t*) ptr;
		case ecs_primitive_kind_t::EcsU64: return *(uint64_t*) ptr;
		case ecs_primitive_kind_t::EcsI8: return *(int8_t*) ptr;
		case ecs_primitive_kind_t::EcsI16: return *(int16_t*) ptr;
		case ecs_primitive_kind_t::EcsI32: return *(int32_t*) ptr;
		case ecs_primitive_kind_t::EcsI64: return *(int64_t*) ptr;
		case ecs_primitive_kind_t::EcsF32: return *(float*) ptr;
		case ecs_primitive_kind_t::EcsF64: return *(double*) ptr;
		case ecs_primitive_kind_t::EcsUPtr: ERR(nullptr, "Can't get primitive\nCan't handle uptr");
		case ecs_primitive_kind_t::EcsIPtr: ERR(nullptr, "Can't get primitive\nCan't handle iptr");
		case ecs_primitive_kind_t::EcsString: return *(char**) ptr;
		case ecs_primitive_kind_t::EcsEntity: return *(ecs_entity_t*) ptr;
		case ecs_primitive_kind_t::EcsId: return *(ecs_entity_t*) ptr;
		default: ERR(nullptr, "Can't get primitive\nUnknown primitive type");
	}
}

Variant GFComponent::member_value_as_type(
	void* ptr,
	ecs_entity_t type
) {
	ecs_world_t* raw = get_world()->raw();
	Variant::Type vari_type = get_world()->id_to_variant_type(type);

	switch (vari_type) {
	case(Variant::Type::NIL): {
		// Member is not a Godot type. Try to get from Flecs primitive
		if (ecs_has_id(raw, type, ecs_id(EcsPrimitive))) {
			return member_value_as_primitive(
				ptr,
				ecs_get(raw, type, EcsPrimitive)->kind
			);
		}

		ERR(nullptr,
			"Can't convert type ", ecs_get_name(raw, type), " to Variant"
		);
	}
	case(Variant::Type::BOOL): return Variant( *(bool*) ptr );
	case(Variant::Type::INT): return Variant( *(int64_t*) ptr );
	case(Variant::Type::FLOAT): return Variant( *(double*) ptr );
	case(Variant::Type::STRING): return Variant( *(String*) ptr );
	case(Variant::Type::VECTOR2): return Variant( *(Vector2*) ptr );
	case(Variant::Type::VECTOR2I): return Variant( *(Vector2i*) ptr );
	case(Variant::Type::RECT2): return Variant( *(Rect2*) ptr );
	case(Variant::Type::RECT2I): return Variant( *(Rect2i*) ptr );
	case(Variant::Type::VECTOR3): return Variant( *(Vector3*) ptr );
	case(Variant::Type::VECTOR3I): return Variant( *(Vector3i*) ptr );
	case(Variant::Type::TRANSFORM2D): return Variant( *(Transform2D*) ptr );
	case(Variant::Type::VECTOR4): return Variant( *(Vector4*) ptr );
	case(Variant::Type::VECTOR4I): return Variant( *(Vector4i*) ptr );
	case(Variant::Type::PLANE): return Variant( *(Plane*) ptr );
	case(Variant::Type::QUATERNION): return Variant( *(Quaternion*) ptr );
	case(Variant::Type::AABB): return Variant( *(AABB*) ptr );
	case(Variant::Type::BASIS): return Variant( *(Basis*) ptr );
	case(Variant::Type::TRANSFORM3D): return Variant( *(Transform3D*) ptr );
	case(Variant::Type::PROJECTION): return Variant( *(Projection*) ptr );
	case(Variant::Type::COLOR): return Variant( *(Color*) ptr );
	case(Variant::Type::STRING_NAME): return Variant( *(StringName*) ptr );
	case(Variant::Type::NODE_PATH): return Variant( *(NodePath*) ptr );
	case(Variant::Type::RID): return Variant( *(RID*) ptr );
	case(Variant::Type::OBJECT): return Variant( *(Variant*) ptr );
	case(Variant::Type::CALLABLE): return Variant( *(Callable*) ptr );
	case(Variant::Type::SIGNAL): return Variant( *(Signal*) ptr );
	case(Variant::Type::DICTIONARY): return *(Dictionary*) ptr;
	case(Variant::Type::ARRAY): return *(Array*) ptr;
	case(Variant::Type::PACKED_BYTE_ARRAY): return Variant( *(PackedByteArray*) ptr );
	case(Variant::Type::PACKED_INT32_ARRAY): return Variant( *(PackedInt32Array*) ptr );
	case(Variant::Type::PACKED_INT64_ARRAY): return Variant( *(PackedInt64Array*) ptr );
	case(Variant::Type::PACKED_FLOAT32_ARRAY): return Variant( *(PackedFloat32Array*) ptr );
	case(Variant::Type::PACKED_FLOAT64_ARRAY): return Variant( *(PackedFloat64Array*) ptr );
	case(Variant::Type::PACKED_STRING_ARRAY): return Variant( *(PackedStringArray*) ptr );
	case(Variant::Type::PACKED_VECTOR2_ARRAY): return Variant( *(PackedVector2Array*) ptr );
	case(Variant::Type::PACKED_VECTOR3_ARRAY): return Variant( *(PackedVector3Array*) ptr );
	case(Variant::Type::PACKED_COLOR_ARRAY): return Variant( *(PackedColorArray*) ptr );
	case(Variant::Type::PACKED_VECTOR4_ARRAY): return Variant( *(PackedVector4Array*) ptr );
	case(Variant::Type::VARIANT_MAX): throw "Can't get type VARIANT_MAX";
	}

	throw "Unreachable";
}

Ref<GFEntity> GFComponent::get_source_entity() {
	return GFEntity::from(get_source_id(), get_world());
}

ecs_entity_t GFComponent::get_source_id() {
	return source_entity_id;
}

int GFComponent::get_data_size() {
	return ecs_get(get_world()->raw(), get_id(), EcsComponent)->size;
}
int GFComponent::get_data_alignment() {
	return ecs_get(get_world()->raw(), get_id(), EcsComponent)->alignment;
}

bool GFComponent::is_alive() {
	GFEntity* entity = this;
	return entity->is_alive()
		&& ecs_has_id(get_world()->raw(), get_source_id(), get_id());
}

void GFComponent::build_data_from_variant(
	Variant vari,
	void* output
) {
	ecs_world_t* raw = get_world()->raw();

	const EcsStruct* struct_data = ecs_get(raw, get_id(), EcsStruct);
	if (struct_data == nullptr) {
		ERR(/**/,
			"Could not build data from Variant\n",
			"Component is not a struct."
		);
	}

	switch (vari.get_type()) {
		case Variant::DICTIONARY: {
			Dictionary dict = vari;
			Array keys = dict.keys();
			for (int i=0; i != keys.size(); i++) {
				String member_name = keys[i];
				Variant value = dict[member_name];
				const EcsMember* member_data = get_member_data(member_name);
				if (member_data == nullptr) {
					// No member with that name exists, skip
					continue;
				}
				void* member_ptr = (void*)(
					((uint8_t*)output) + member_data->offset
				);
				// Set member from dictionary
				Utils::set_type_from_variant(value, member_data->type, raw, member_ptr);
			}
			break;
		}
		case Variant::ARRAY: {
			Array arr = vari;
			for (int i=0; i != arr.size() && i != ecs_vec_size(&struct_data->members); i++) {
				// Iterate the combined sizes of the passed array and the members vector
				Variant value = arr[i];

				ecs_member_t* member_data = ecs_vec_get_t(&struct_data->members, ecs_member_t, i);
				void* member_ptr = (void*)(
					((uint8_t*)output) + member_data->offset
				);
				// Set member from array
				Utils::set_type_from_variant(value, member_data->type, raw, member_ptr);
			}
			break;
		}
		default: ERR(/**/,
			"Could not build data from Variant\n",
			"Variant type ", Variant::get_type_name(vari.get_type()),
			" can't be built into component data."
		);
	}
}

void GFComponent::set_source_id(ecs_entity_t id) {
	source_entity_id = id;
}

Ref<GFRegisterableEntity> GFComponent::new_internal() {
	return Ref(memnew(GFComponent));
}

void GFComponent::_bind_methods() {
	GDVIRTUAL_BIND(_build, "b");
	godot::ClassDB::bind_method(D_METHOD("_register_internal"), &GFComponent::_register_internal);

	godot::ClassDB::bind_static_method(GFComponent::get_class_static(), D_METHOD("spawn", "world"), &GFComponent::spawn, nullptr);
	godot::ClassDB::bind_static_method(GFComponent::get_class_static(), D_METHOD("from", "component", "world"), &GFComponent::from, nullptr);
	godot::ClassDB::bind_static_method(GFComponent::get_class_static(), D_METHOD("from_id", "id", "world"), &GFComponent::from_id, nullptr);

	godot::ClassDB::bind_method(D_METHOD("getm", "member"), &GFComponent::getm);
	godot::ClassDB::bind_method(D_METHOD("setm", "member", "value"), &GFComponent::setm);

	godot::ClassDB::bind_method(D_METHOD("get_source_entity"), &GFComponent::get_source_entity);
	godot::ClassDB::bind_method(D_METHOD("get_source_id"), &GFComponent::get_source_id);
	godot::ClassDB::bind_method(D_METHOD("get_data_size"), &GFComponent::get_data_size);
	godot::ClassDB::bind_method(D_METHOD("get_data_alignment"), &GFComponent::get_data_alignment);
	godot::ClassDB::bind_method(D_METHOD("is_alive"), &GFComponent::is_alive);

	godot::ClassDB::bind_static_method(get_class_static(), D_METHOD("_new_internal"), &GFComponent::new_internal);
}
