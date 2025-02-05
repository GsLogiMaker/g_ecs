
#include "entity.h"
#include "entity_iterator.h"
#include "godot_cpp/classes/script.hpp"
#include "godot_cpp/core/memory.hpp"
#include "godot_cpp/variant/array.hpp"
#include "utils.h"
// needed here because entity.h does not include
// component.h, but uses forward declaration instead
#include "component.h"
#include "pair.h"

#include <flecs.h>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include "godot_cpp/variant/variant.hpp"
#include "world.h"

using namespace godot;

Ref<GFEntity> GFEntity::new_in_world(GFWorld* world) {
	return memnew(GFEntity(world));
}
Ref<GFEntity> GFEntity::from(const Variant entity, GFWorld* world) {
	return memnew(GFEntity(world->coerce_id(entity), world));
}
Ref<GFEntity> GFEntity::from_id(ecs_entity_t id, GFWorld* world) {
	return setup_template<GFEntity>(memnew(GFEntity(id, world)));
}

Ref<GFEntity> GFEntity::add_child(const Variant entity) {
	ecs_entity_t id = get_world()->coerce_id(entity);
	if (!get_world()->id_set_parent(id, get_id())) {
		return nullptr;
	}
	return Ref(this);
}

Ref<GFEntity> GFEntity::add_component(
	const Variant** args, GDExtensionInt arg_count, GDExtensionCallError &error
) {
	if (arg_count < 1) {
		// Too few arguments, return with error.
		error.error = GDExtensionCallErrorType::GDEXTENSION_CALL_ERROR_TOO_FEW_ARGUMENTS;
		error.argument = arg_count;
		error.expected = 1;
		return Ref(this);
	}

	// Parse arguments
	Array members = Array();
	members.resize(arg_count);
	for (int i=0; i != arg_count; i++) {
		members[i] = *args[i];
	}
	Variant comopnent = members.pop_front();

	add_componentv(comopnent, members);

	return Ref(this);
}

Ref<GFEntity> GFEntity::add_componentv(const Variant component, const Array members) {
	GFWorld* w = get_world();

	ecs_entity_t c_id = w->coerce_id(component);

	if (ecs_has_id(w->raw(), get_id(), c_id)) {
		ERR(Ref(this),
			"Can't add component to entity\n",
			"ID coerced from ", component, " is already added to ", get_id()
		)
	}

	set_componentv(c_id, members);

	return Ref(this);
}

Ref<GFEntity> GFEntity::add_pair(
	const Variant** args, GDExtensionInt arg_count, GDExtensionCallError &error
) {
	if (arg_count < 2) {
		// Too few arguments, return with error.
		error.error = GDExtensionCallErrorType::GDEXTENSION_CALL_ERROR_TOO_FEW_ARGUMENTS;
		error.argument = arg_count;
		error.expected = 2;
		return Ref(this);
	}

	// Parse arguments
	Array members = Array();
	members.resize(arg_count);
	for (int i=0; i != arg_count; i++) {
		members[i] = *args[i];
	}
	Variant first = members.pop_front();
	Variant sec = members.pop_front();

	add_pairv(first, sec, members);

	return Ref(this);
}
Ref<GFEntity> GFEntity::add_pairv(
	const Variant first,
	const Variant second,
	const Array members
) {
	GFWorld* w = get_world();

	ecs_entity_t first_id = w->coerce_id(first);
	ecs_entity_t second_id = w->coerce_id(second);
	ecs_entity_t pair_id = w->pair_ids(first_id, second_id);
	if (
		members.size() != 0
		|| ecs_has_id(w->raw(), pair_id, FLECS_IDEcsComponentID_)
	) {
		// Add pair as a component
		add_componentv(pair_id, members);
	} else {
		// Add pair as a dataless tag
		add_tag(pair_id);
	}

	return Ref(this);
}

Ref<GFEntity> GFEntity::add_tag(const Variant tag) {
	GFWorld* w = get_world();

	ecs_entity_t tag_id = w->coerce_id(tag);

	if (ecs_has(w->raw(), tag_id, EcsComponent)) {
		ERR(Ref(this),
			"Failed to add tag to entity\n",
			"	ID, ", tag_id, " is a component, not a tag\n"
			"	(Tags are any entity with no data)"
		);
	}

	ecs_add_id(w->raw(), get_id(), tag_id);

	return Ref(this);
}

Ref<GFEntity> GFEntity::emit(
	const Variant target_entity,
	const Array components,
	const Array event_members
) {
	ecs_entity_t componet_id = 0;
	const EcsComponent* comp_data = nullptr;
	ecs_type_t type = {0};
	int8_t* event_data = nullptr;

	// Setup type
	ecs_type_t* type_ptr = &type;
	if (components.size() == 0) {
		type_ptr = nullptr;
	} else {
		comp_data = ecs_get(
			get_world()->raw(),
			get_id(),
			EcsComponent
		);
		componet_id = get_world()->coerce_id(components[0]);
		type.array = &componet_id;
		type.count = 1;
	}

	// Setup parameter
	if (comp_data != nullptr) {
		event_data = new int8_t[comp_data->size];
		GFComponent::build_data_from_members(
			event_members,
			event_data,
			get_id(),
			get_world()
		);
	}

	// Emit
	ecs_event_desc_t desc = {
		.event = get_id(),
		.ids = type_ptr,
		.entity = get_world()->coerce_id(target_entity),
		.param = static_cast<void*>(event_data)
	};
	ecs_emit(
		get_world()->raw(),
		&desc
	);

	// Cleanup
	if (event_data != nullptr) {
		delete [] event_data;
	}

	return Ref(this);
}

Ref<GFComponent> GFEntity::get_component(const Variant component) const {
	Ref<GFComponent> c = GFComponent::from_id(
		get_world()->coerce_id(component),
		get_id(),
		get_world()
	);

	if (c == nullptr) {
		ERR(nullptr,
			"Component ID ", c->get_id(), " is not alive."
		);
	}
	if (!ecs_has_id(get_world()->raw(), id, c->get_id())) {
		ERR(nullptr,
			"Could not find attached component ID ", c->get_id(), " on entity"
		);
	}

	return c;
}

Ref<GFComponent> GFEntity::get_pair(
	const Variant first,
	const Variant second
) const {
	ecs_entity_t first_id = get_world()->coerce_id(first);
	ecs_entity_t second_id = get_world()->coerce_id(second);
	return get_component(ecs_pair(first_id, second_id));
}

bool GFEntity::has_entity(const Variant entity) const {
	return ecs_has_id(get_world()->raw(), get_id(), get_world()->coerce_id(entity));
}

bool GFEntity::has_pair(const Variant first, const Variant second) const {
	return ecs_has_id(get_world()->raw(), get_id(), get_world()->pair_ids(
		get_world()->coerce_id(first),
		get_world()->coerce_id(second)
	));
}

bool GFEntity::has_child(const String path) const {
	return ecs_lookup_path_w_sep(
		get_world()->raw(),
		get_id(),
		path.utf8(),
		"/",
		"/root/",
		false
	) != 0;
}

Ref<GFEntity> GFEntity::set_component(
	const Variant** args, GDExtensionInt arg_count, GDExtensionCallError &error
) {
	if (arg_count < 1) {
		// Too few arguments, return with error.
		error.error = GDExtensionCallErrorType::GDEXTENSION_CALL_ERROR_TOO_FEW_ARGUMENTS;
		error.argument = arg_count;
		error.expected = 1;
		return Ref(this);
	}

	// Parse arguments
	Array members = Array();
	members.resize(arg_count);
	for (int i=0; i != arg_count; i++) {
		members[i] = *args[i];
	}
	Variant comopnent = members.pop_front();

	return set_componentv(comopnent, members);
}

Ref<GFEntity> GFEntity::set_componentv(
	const Variant component,
	const Array members
) {
	GFWorld* w = get_world();

	ecs_entity_t c_id = w->coerce_id(component);

	if (!ecs_has_id(w->raw(), get_id(), c_id)) {
		// Component is not present in entity, add it

		if (ECS_IS_PAIR(c_id)) {
			// ID is a pair, perform pair specific checks
			ecs_entity_t first_id = ECS_PAIR_FIRST(c_id);
			ecs_entity_t second_id = ECS_PAIR_SECOND(c_id);
			if (
				!ecs_has_id(w->raw(), first_id, ecs_id(EcsComponent))
				&& !ecs_has_id(w->raw(), second_id, ecs_id(EcsComponent))
			) {
				// ID is not a component, error
				ERR(Ref(this),
					"Failed to set data in pair\n",
					"Neither ID ", first_id,
					" nor ", second_id, "are components"
				);
			}

		} else if (!ecs_has_id(w->raw(), c_id, ecs_id(EcsComponent))) {
			// Error, passed variant is not a real component
			ERR(Ref(this),
				"Failed to add component to entity\n",
				"ID coerced from ", component, " is not a component"
			);

		}

		ecs_add_id(w->raw(), get_id(), c_id);
	}

	GFComponent::build_data_from_members(
		members,
		ecs_get_mut_id(w->raw(), get_id(), c_id),
		c_id,
		get_world()
	);
	ecs_modified_id(w->raw(), get_id(), c_id);

	return Ref(this);
}

Ref<GFEntity> GFEntity::set_pair(
	const Variant** args, GDExtensionInt arg_count, GDExtensionCallError &error
) {
	if (arg_count < 1) {
		// Too few arguments, return with error.
		error.error = GDExtensionCallErrorType::GDEXTENSION_CALL_ERROR_TOO_FEW_ARGUMENTS;
		error.argument = arg_count;
		error.expected = 1;
		return Ref(this);
	}

	// Parse arguments
	Array members = Array();
	members.resize(arg_count);
	for (int i=0; i != arg_count; i++) {
		members[i] = *args[i];
	}
	Variant first = members.pop_front();
	Variant sec = members.pop_front();

	return set_pairv(first, sec, members);
}

Ref<GFEntity> GFEntity::set_pairv (
	const Variant first,
	const Variant second,
	const Array members
) {
	ecs_entity_t first_id = get_world()->coerce_id(first);
	ecs_entity_t second_id = get_world()->coerce_id(second);
	set_componentv(ecs_pair(first_id, second_id), members);

	return Ref(this);
}

void GFEntity::delete_() const {
	ecs_delete(get_world()->raw(), get_id());
}

ecs_entity_t GFEntity::get_id() const { return id; }
String GFEntity::get_path() const {
	return String(ecs_get_path_w_sep(
		get_world()->raw(),
		0,
		get_id(),
		"/",
		"/root/"
	));
}
GFWorld* GFEntity::get_world() const { return Object::cast_to<GFWorld>(
	UtilityFunctions::instance_from_id(world_instance_id)
); }

bool GFEntity::is_alive() const {
	if (get_world() == nullptr) {
		return false;
	}
	if (!UtilityFunctions::is_instance_id_valid(world_instance_id)) {
		return false;
	}

	return get_world()->is_id_alive(get_id());
}
bool GFEntity::is_pair() const {
	return ecs_id_is_pair(get_id());
}

Ref<GFEntityIterator> GFEntity::iter_children() const {
	GFWorld* w = get_world();
	ecs_iter_t iter = ecs_children(w->raw(), get_id());
	return Ref(memnew(GFEntityIterator(iter, w)));
}

Ref<GFEntity> GFEntity::get_child(const String name) const {
	ecs_entity_t id = ecs_lookup_path_w_sep(
		get_world()->raw(),
		get_id(),
		name.utf8(),
		"/",
		"/root/",
		false
	);
	if (id == 0) {
		return nullptr;
	}
	return GFEntity::from_id(id, get_world());
}

TypedArray<GFEntity> GFEntity::get_children() const {
	// Entity iterator should only ever iterate over
	// entities, so we can safely cast it to a typed array.
	Array arr = iter_children()->into_array();
	return arr;
}

String GFEntity::get_name() const {
	CHECK_ENTITY_ALIVE(get_id(), get_world(), "",
		"Failed to get name of entity\n"
	);
	return String(ecs_get_name(get_world()->raw(), get_id()));
}

Ref<GFEntity> GFEntity::get_parent() const {
	ecs_entity_t parent = ecs_get_parent(
		get_world()->raw(),
		get_id()
	);

	if (parent == 0) {
		return nullptr;
	}

	return GFEntity::from_id(parent, get_world());
}

Ref<GFEntity> GFEntity::set_name(const String name_) {
	ecs_entity_t parent = ecs_get_parent(
		get_world()->raw(),
		get_id()
	);

	if (ecs_lookup_child(
		get_world()->raw(),
		parent,
		name_.utf8()
	) == 0) {
		// No name conflicts, set name and return
		ecs_set_name(get_world()->raw(), get_id(), name_.utf8());
		return Ref(this);
	}

	int trailing_digits = 0;
	for (int i=0; i != name_.length(); i++) {
		char32_t digit = name_[name_.length()-i-1];
		if (digit >= '0' && digit >= '9') {
			trailing_digits++;
		} else {
			break;
		}
	}

	String number = name_.substr(name_.length()-1-trailing_digits);
	String base_name = name_.substr(0, name_.length()-1-trailing_digits);
	String name = name_;
	do {
		int name_int = number.to_int();
		name_int += 1;
		number = String::num_uint64(name_int);
		name = base_name + number;
	} while (ecs_lookup_child(
		get_world()->raw(),
		parent,
		name.utf8()
	));

	ecs_set_name(get_world()->raw(), get_id(), name.utf8());
	return Ref(this);
}

Ref<GFEntity> GFEntity::set_parent(const Variant entity) {
	ecs_entity_t id = get_world()->coerce_id(entity);
	if (!get_world()->id_set_parent(get_id(), id)) {
		return nullptr;
	}
	return Ref(this);
}

Ref<GFPair> GFEntity::pair(const Variant second) const {
	return GFPair::from_id(pair_id(get_world()->coerce_id(second)), get_world());
}
ecs_entity_t GFEntity::pair_id(const ecs_entity_t second) const {
	return get_world()->pair_ids(get_id(), second);
}

Ref<GFEntity> GFEntity::remove_component(const Variant entity, const Variant second) {
	GFWorld* w = get_world();

	ecs_entity_t id = w->coerce_id(entity);
	CHECK_ENTITY_ALIVE(id, w, nullptr,
		"Failed to remove component\n"
	);

	if (second != Variant()) {
		ecs_entity_t second_id = w->coerce_id(second);
		CHECK_ENTITY_ALIVE(second_id, w, nullptr,
			"Failed to remove component\n"
		);
		id = ecs_pair(id, second_id);
	}

	if (!ecs_has_id(w->raw(), get_id(), id)) {
		ERR(nullptr,
			"Failed to remove component\n",
			"	Component ID ", id, " is not attached to entity"
		);
	}

	ecs_remove_id(get_world()->raw(), get_id(), id);

	return Ref(this);
}

String GFEntity::_to_string() const {
	return get_world()->id_to_text(get_id());
}

// ----------------------------------------------
// --- Unexposed ---
// ----------------------------------------------

void GFEntity::set_id(const ecs_entity_t value) { id = value; }
void GFEntity::set_world(const GFWorld* value) { world_instance_id = value->get_instance_id(); }

void GFEntity::_bind_methods() {
	REGISTER_ENTITY_SELF_METHODS(GFEntity);

	godot::ClassDB::bind_static_method(GFEntity::get_class_static(), D_METHOD("new_in_world", "world"), &GFEntity::new_in_world);
	godot::ClassDB::bind_static_method(GFEntity::get_class_static(), D_METHOD("from", "entity", "world"), &GFEntity::from, nullptr);
	godot::ClassDB::bind_static_method(GFEntity::get_class_static(), D_METHOD("from_id", "id", "world"), &GFEntity::from_id, nullptr);

	godot::ClassDB::bind_method(D_METHOD("get", "component"), &GFEntity::get_component);
	godot::ClassDB::bind_method(D_METHOD("get_pair", "first", "second"), &GFEntity::get_pair);

	godot::ClassDB::bind_method(D_METHOD("delete"), &GFEntity::delete_);

	godot::ClassDB::bind_method(D_METHOD("get_child", "path"), &GFEntity::get_child);
	godot::ClassDB::bind_method(D_METHOD("get_children"), &GFEntity::get_children);
	godot::ClassDB::bind_method(D_METHOD("get_id"), &GFEntity::get_id);
	godot::ClassDB::bind_method(D_METHOD("get_name"), &GFEntity::get_name);
	godot::ClassDB::bind_method(D_METHOD("get_parent"), &GFEntity::get_parent);
	godot::ClassDB::bind_method(D_METHOD("get_path"), &GFEntity::get_path);
	godot::ClassDB::bind_method(D_METHOD("get_world"), &GFEntity::get_world);

	godot::ClassDB::bind_method(D_METHOD("has_child", "path"), &GFEntity::has_child);
	godot::ClassDB::bind_method(D_METHOD("has_entity", "entity"), &GFEntity::has_entity);
	godot::ClassDB::bind_method(D_METHOD("has_pair", "first", "second"), &GFEntity::has_pair);

	godot::ClassDB::bind_method(D_METHOD("is_alive"), &GFEntity::is_alive);
	godot::ClassDB::bind_method(D_METHOD("is_pair"), &GFEntity::is_pair);
	godot::ClassDB::bind_method(D_METHOD("iter_children"), &GFEntity::iter_children);

	godot::ClassDB::bind_method(D_METHOD("pair", "second"), &GFEntity::pair);
	godot::ClassDB::bind_method(D_METHOD("pair_id", "second_id"), &GFEntity::pair_id);
}
