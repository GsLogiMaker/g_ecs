
#include "component.h"
#include "component_builder.h"
#include "entity.h"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/core/error_macros.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/packed_int64_array.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "registerable_entity.h"
#include "utils.h"
#include "world.h"

#include <cstdint>
#include <stdint.h>
#include <flecs.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

Ref<GFComponent> GFComponent::from(
	const Variant comp,
	const Variant entity,
	GFWorld* world
) {
	world = GFWorld::world_or_singleton(world);
	return from_id(world->coerce_id(comp), world->coerce_id(entity), world);
}
Ref<GFComponent> GFComponent::from_id(
	ecs_entity_t comp,
	ecs_entity_t entity,
	GFWorld* world
) {
	world = GFWorld::world_or_singleton(world);

	Ref<GFComponent> comp_ref = memnew(GFComponent(entity, comp, world));
	return setup_template<GFComponent>(comp_ref);
}

Ref<GFComponent> GFComponent::from_id_no_source(
	ecs_entity_t comp,
	GFWorld* world
) {
	world = GFWorld::world_or_singleton(world);

	Ref<GFComponent> comp_ref = Ref(memnew(GFComponent(0, comp, world)));
	comp_ref->update_script();
	return comp_ref;
}

bool GFComponent::_get(StringName property, Variant& out) {
	if (Object::_get(property, out)) {
		return true;
	}
	out = getm(property);
	return out != Variant();
}

void GFComponent::_get_property_list(List<PropertyInfo>* p_list) {
	GFWorld* w = get_world();

	ecs_entity_t main_id = w->get_main_id(get_id());
	if (!w->is_id_alive(main_id)) {
		return;
	}

	const EcsStruct* struct_c = ecs_get(w->raw(), main_id, EcsStruct);
	for (int member_i=0; member_i != struct_c->members.count; member_i++) {
		const ecs_member_t* member = ecs_vec_get_t(&struct_c->members, ecs_member_t, member_i);

		p_list->push_back(PropertyInfo(
			w->id_into_variant_type(member->type),
			member->name,
			PROPERTY_HINT_NONE,
			"",
			PROPERTY_USAGE_EDITOR
		));
	}
}

bool GFComponent::_set(StringName property, Variant value) {
	if (Object::_set(property, value)) {
		return true;
	}
	return setm(property, value);
}

void GFComponent::_register_internal() {
	// Build component
	Ref<GFComponentBuilder> b = memnew(GFComponentBuilder(get_world()));
	if (GDVIRTUAL_IS_OVERRIDDEN(_build)) {
		b->set_target_entity(get_id());
		GDVIRTUAL_CALL(_build, b);
		if (!b->is_built()) {
			b->build();
		}
	}

	// Call super method
	GFRegisterableEntity::_register_internal();
}

bool GFComponent::setm(const String member, const Variant value) const {
	if (!setm_no_notify(member, value)) {
		return false;
	}
	ecs_modified_id(get_world()->raw(), get_source_id(), get_id());
	return true;
}

bool GFComponent::setm_no_notify(const String member, const Variant value) const {
	GFWorld* w = get_world();

	// Get member data
	const EcsMember* member_data = get_member_data(member);
	if (member_data == nullptr) {
		// Member data is null. This should never happen.
		ERR(false,
			"Member metadata is null"
		);
	}
	void* member_ptr = get_member_ptr_mut_at(member_data->offset);
	if (member_ptr == nullptr) {
		ERR(false,
			"Member pointer is null"
		);
	}

	Utils::set_type_from_variant(value, member_data->type, w, member_ptr);
	return true;
}

Variant GFComponent::getm(const String member) const {
	ecs_world_t* raw = get_world()->raw();

	if (!ecs_has_id(raw, get_source_id(), get_id())) {
		ERR(nullptr, "Failed to get member",
			"\n	Entity ",
			get_source_entity(),
			" does not have component ",
			get_world()->id_to_text(get_id())
		);
	}

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

void* GFComponent::get_member_ptr_mut_at(int offset) const {
	ecs_world_t* raw = get_world()->raw();
	int8_t* bytes = static_cast<int8_t*>(
		ecs_get_mut_id(raw, get_source_id(), get_id())
	);
	return static_cast<void*>(&bytes[offset]);
}

const EcsMember* GFComponent::get_member_data(const String member) const {
	ecs_world_t* raw = get_world()->raw();
	const char* c_str = member.utf8().get_data();

	ecs_entity_t main_id = get_world()->get_main_id(get_id());

	// Get member ID
	ecs_entity_t member_id = ecs_lookup_child(raw, main_id, c_str);

	if (member_id == 0) {
		ERR(nullptr,
			"No member named \"",
			member,
			"\" found in component ",
			get_world()->id_to_text(get_id())
		);
	}

	// Get member data
	const EcsMember* member_data = ecs_get(raw, member_id, EcsMember);

	return member_data;
}

Variant GFComponent::member_value_as_primitive(
	const void* ptr,
	ecs_primitive_kind_t primitive
) const {
	switch (primitive) {
		case ecs_primitive_kind_t::EcsBool: return *static_cast<const bool*>(ptr);
		case ecs_primitive_kind_t::EcsChar: return *static_cast<const char*>(ptr);
		case ecs_primitive_kind_t::EcsByte: return *static_cast<const uint8_t*>(ptr);
		case ecs_primitive_kind_t::EcsU8: return *static_cast<const uint8_t*>(ptr);
		case ecs_primitive_kind_t::EcsU16: return *static_cast<const uint16_t*>(ptr);
		case ecs_primitive_kind_t::EcsU32: return *static_cast<const uint32_t*>(ptr);
		case ecs_primitive_kind_t::EcsU64: return *static_cast<const uint64_t*>(ptr);
		case ecs_primitive_kind_t::EcsI8: return *static_cast<const int8_t*>(ptr);
		case ecs_primitive_kind_t::EcsI16: return *static_cast<const int16_t*>(ptr);
		case ecs_primitive_kind_t::EcsI32: return *static_cast<const int32_t*>(ptr);
		case ecs_primitive_kind_t::EcsI64: return *static_cast<const int64_t*>(ptr);
		case ecs_primitive_kind_t::EcsF32: return *static_cast<const float*>(ptr);
		case ecs_primitive_kind_t::EcsF64: return *static_cast<const double*>(ptr);
		case ecs_primitive_kind_t::EcsUPtr: ERR(nullptr, "Can't get primitive\n	Can't handle uptr");
		case ecs_primitive_kind_t::EcsIPtr: ERR(nullptr, "Can't get primitive\n	Can't handle iptr");
		case ecs_primitive_kind_t::EcsString: return *static_cast<char* const *>(ptr);
		case ecs_primitive_kind_t::EcsEntity: return *static_cast<const ecs_entity_t*>(ptr);
		case ecs_primitive_kind_t::EcsId: return *static_cast<const ecs_entity_t*>(ptr);
		default: ERR(nullptr, "Can't get primitive\n	Unknown primitive type: ", primitive);
	}
}

Variant GFComponent::member_value_as_type(
	const void* ptr,
	ecs_entity_t type
) const {
	GFWorld* w = get_world();

	Variant::Type vari_type = get_world()->id_as_variant_type(type);
	if (vari_type == Variant::NIL) {
		const EcsPrimitive* primi_c = ecs_get(w->raw(), type, EcsPrimitive);
		if (primi_c != nullptr) {
			return member_value_as_primitive(ptr, primi_c->kind);
		}
	}

	switch (vari_type) {
	case(Variant::Type::NIL): return nullptr;
	case(Variant::Type::BOOL): return *static_cast<const bool*>(ptr);
	case(Variant::Type::INT): return *static_cast<const int64_t*>(ptr);
	case(Variant::Type::FLOAT): return *static_cast<const double*>(ptr);
	case(Variant::Type::STRING): return *static_cast<const String*>(ptr);
	case(Variant::Type::VECTOR2): return *static_cast<const Vector2*>(ptr);
	case(Variant::Type::VECTOR2I): return *static_cast<const Vector2i*>(ptr);
	case(Variant::Type::RECT2): return *static_cast<const Rect2*>(ptr);
	case(Variant::Type::RECT2I): return *static_cast<const Rect2i*>(ptr);
	case(Variant::Type::VECTOR3): return *static_cast<const Vector3*>(ptr);
	case(Variant::Type::VECTOR3I): return *static_cast<const Vector3i*>(ptr);
	case(Variant::Type::TRANSFORM2D): return *static_cast<const Transform2D*>(ptr);
	case(Variant::Type::VECTOR4): return *static_cast<const Vector4*>(ptr);
	case(Variant::Type::VECTOR4I): return *static_cast<const Vector4i*>(ptr);
	case(Variant::Type::PLANE): return *static_cast<const Plane*>(ptr);
	case(Variant::Type::QUATERNION): return *static_cast<const Quaternion*>(ptr);
	case(Variant::Type::AABB): return *static_cast<const AABB*>(ptr);
	case(Variant::Type::BASIS): return *static_cast<const Basis*>(ptr);
	case(Variant::Type::TRANSFORM3D): return *static_cast<const Transform3D*>(ptr);
	case(Variant::Type::PROJECTION): return *static_cast<const Projection*>(ptr);
	case(Variant::Type::COLOR): return *static_cast<const Color*>(ptr);
	case(Variant::Type::STRING_NAME): return *static_cast<const StringName*>(ptr);
	case(Variant::Type::NODE_PATH): return *static_cast<const NodePath*>(ptr);
	case(Variant::Type::RID): return *static_cast<const RID*>(ptr);
	case(Variant::Type::OBJECT): return *static_cast<const Variant*>(ptr);
	case(Variant::Type::CALLABLE): return *static_cast<const Callable*>(ptr);
	case(Variant::Type::SIGNAL): return *static_cast<const Signal*>(ptr);
	case(Variant::Type::DICTIONARY): return *static_cast<const Dictionary*>(ptr);
	case(Variant::Type::ARRAY): return *static_cast<const Array*>(ptr);
	case(Variant::Type::PACKED_BYTE_ARRAY): return *static_cast<const PackedByteArray*>(ptr);
	case(Variant::Type::PACKED_INT32_ARRAY): return *static_cast<const PackedInt32Array*>(ptr);
	case(Variant::Type::PACKED_INT64_ARRAY): return *static_cast<const PackedInt64Array*>(ptr);
	case(Variant::Type::PACKED_FLOAT32_ARRAY): return *static_cast<const PackedFloat32Array*>(ptr);
	case(Variant::Type::PACKED_FLOAT64_ARRAY): return *static_cast<const PackedFloat64Array*>(ptr);
	case(Variant::Type::PACKED_STRING_ARRAY): return *static_cast<const PackedStringArray*>(ptr);
	case(Variant::Type::PACKED_VECTOR2_ARRAY): return *static_cast<const PackedVector2Array*>(ptr);
	case(Variant::Type::PACKED_VECTOR3_ARRAY): return *static_cast<const PackedVector3Array*>(ptr);
	case(Variant::Type::PACKED_COLOR_ARRAY): return *static_cast<const PackedColorArray*>(ptr);
	case(Variant::Type::PACKED_VECTOR4_ARRAY): return *static_cast<const PackedVector4Array*>(ptr);
	case(Variant::Type::VARIANT_MAX): ERR(nullptr, "Failed to get member\n	Member type VARIANT_MAX is invalid");
	}

	ERR(nullptr, "Unreachable");
}

String GFComponent::_to_string() const {
	return get_world()->id_to_text(get_source_id())
		+ "." + get_world()->id_to_text(get_id());
}

Ref<GFEntity> GFComponent::get_source_entity() const {
	return GFEntity::from(get_source_id(), get_world());
}

ecs_entity_t GFComponent::get_source_id() const {
	return source_entity_id;
}

int GFComponent::get_data_size() const {
	const EcsComponent* component_data = ecs_get(
		get_world()->raw(),
		get_id(),
		EcsComponent
	);
	if (component_data == nullptr) {
		return 0;
	}
	return component_data->size;
}
int GFComponent::get_data_alignment() const {
	const EcsComponent* component_data = ecs_get(
		get_world()->raw(),
		get_id(),
		EcsComponent
	);
	if (component_data == nullptr) {
		return 0;
	}
	return component_data->alignment;
}

bool GFComponent::is_alive() const {
	const GFEntity* entity = this;
	return entity->is_alive()
		&& ecs_has_id(get_world()->raw(), get_source_id(), get_id());
}

void GFComponent::build_data_from_variant(
	Variant vari,
	void* output
) const {
	GFWorld* w = get_world();

	const EcsStruct* struct_data = ecs_get(w->raw(), get_id(), EcsStruct);
	if (struct_data == nullptr) {
		ERR(/**/,
			"Could not build data from Variant\n",
			"	Component is not a struct."
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
				void* member_ptr = static_cast<void*>(
					static_cast<uint8_t*>(output) + member_data->offset
				);
				// Set member from dictionary
				Utils::set_type_from_variant(value, member_data->type, w, member_ptr);
			}
			break;
		}
		case Variant::ARRAY: {
			Array arr = vari;
			for (int i=0; i != arr.size() && i != ecs_vec_size(&struct_data->members); i++) {
				// Iterate the combined sizes of the passed array and the members vector
				Variant value = arr[i];

				ecs_member_t* member_data = ecs_vec_get_t(&struct_data->members, ecs_member_t, i);
				void* member_ptr = static_cast<void*>(
					static_cast<uint8_t*>(output) + member_data->offset
				);
				// Set member from array
				Utils::set_type_from_variant(value, member_data->type, w, member_ptr);
			}
			break;
		}
		default: ERR(/**/,
			"Could not build data from Variant\n",
			"	Variant type ", Variant::get_type_name(vari.get_type()),
			" can't be built into component data."
		);
	}
}

const EcsComponent* GFComponent::get_component_ptr(
	const GFWorld* world,
	ecs_entity_t id
) {
	return ecs_get(world->raw(), world->get_main_id(id), EcsComponent);
}

const EcsStruct* GFComponent::get_struct_ptr(
	const GFWorld* world,
	ecs_entity_t id
) {
	return ecs_get(world->raw(), world->get_main_id(id), EcsStruct);
}

void GFComponent::build_data_from_members(
	const Array members,
	void* output,
	ecs_entity_t component_id,
	const GFWorld* world
) {
	const EcsStruct* struct_data = GFComponent::get_struct_ptr(world, component_id);
	if (struct_data == nullptr) {
		ERR(/**/,
			"Could not build data from Array\n",
			"	Entity ", world->id_to_text(component_id), " is not a struct."
		);
	}

	for (int i=0; i != members.size() && i != ecs_vec_size(&struct_data->members); i++) {
		// Iterate the combined sizes of the passed array and the members vector
		Variant value = members[i];

		ecs_member_t* member_data = /* Get member metadata */ ecs_vec_get_t(
			&struct_data->members,
			ecs_member_t,
			i
		);
		void* member_ptr = /* Get member pointer */ static_cast<void*>(
			static_cast<uint8_t*>(output) + member_data->offset
		);
		// Set member value
		Utils::set_type_from_variant(value, member_data->type, world, member_ptr);
	}
}

void GFComponent::set_source_id(ecs_entity_t id) {
	source_entity_id = id;
}

void GFComponent::_bind_methods() {
	REGISTER_ENTITY_SELF_METHODS(GFComponent);

	GDVIRTUAL_BIND(_build, "b");
	godot::ClassDB::bind_method(D_METHOD("_register_internal"), &GFComponent::_register_internal);

	godot::ClassDB::bind_static_method(get_class_static(), D_METHOD("from", "component", "world"), &GFComponent::from, nullptr);
	godot::ClassDB::bind_static_method(get_class_static(), D_METHOD("from_id", "id", "world"), &GFComponent::from_id, nullptr);

	godot::ClassDB::bind_method(D_METHOD("getm", "member"), &GFComponent::getm);
	godot::ClassDB::bind_method(D_METHOD("setm", "member", "value"), &GFComponent::setm);
	godot::ClassDB::bind_method(D_METHOD("setm_no_notify", "member", "value"), &GFComponent::setm_no_notify);

	godot::ClassDB::bind_method(D_METHOD("get_source_entity"), &GFComponent::get_source_entity);
	godot::ClassDB::bind_method(D_METHOD("get_source_id"), &GFComponent::get_source_id);
	godot::ClassDB::bind_method(D_METHOD("get_data_size"), &GFComponent::get_data_size);
	godot::ClassDB::bind_method(D_METHOD("get_data_alignment"), &GFComponent::get_data_alignment);
	godot::ClassDB::bind_method(D_METHOD("is_alive"), &GFComponent::is_alive);
	godot::ClassDB::bind_method(D_METHOD("_to_string"), &GFComponent::_to_string);
}
