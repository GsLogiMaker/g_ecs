
#include "entity.h"
#include "godot_cpp/classes/script.hpp"
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

using namespace godot;

GFEntity::GFEntity() {
}
GFEntity::~GFEntity() {
}

Ref<GFEntity> GFEntity::spawn(GFWorld* world_) {
	GFWorld* world = GFWorld::world_or_singleton(world_);
	return from_id(ecs_new(world->raw()), world);
}
Ref<GFEntity> GFEntity::from(Variant entity, GFWorld* world) {
	return from_id(world->coerce_id(entity), world);
}
Ref<GFEntity> GFEntity::from_id(ecs_entity_t id, GFWorld* world) {
	return from_id_template<GFEntity>(id, world);
}

Ref<GFEntity> GFEntity::add_component(Variant component, Variant data) {
	GFWorld* w = get_world();

	ecs_entity_t c_id = w->coerce_id(component);

	if (ECS_IS_PAIR(c_id)) {
		add_pair(
			ECS_PAIR_FIRST(c_id),
			ECS_PAIR_SECOND(c_id),
			data
		);
		return Ref(this);
	}

	if (!ecs_has_id(w->raw(), c_id, ecs_id(EcsComponent))) {
		ERR(Ref(this),
			"Failed to add component to entity\n",
			"ID coerced from ", component, " is not a component"
		);
	}

	ecs_add_id(w->raw(), get_id(), c_id);

	if (data != Variant()) {
		set_component(c_id, data);
	}

	return Ref(this);
}

Ref<GFEntity> GFEntity::add_entity(Variant entity, Variant data) {
	GFWorld* w = get_world();

	ecs_add_id(w->raw(), get_id(), w->coerce_id(entity));
	if (data != Variant()) {
		set_component(entity, data);
	}

	return Ref(this);
}

Ref<GFEntity> GFEntity::add_pair(Variant first, Variant second, Variant data) {
	GFWorld* w = get_world();

	ecs_entity_t first_id = w->coerce_id(first);
	ecs_entity_t second_id = w->coerce_id(second);
	ecs_entity_t pair_id = w->pair_ids(first_id, second_id);
	add_entity(pair_id, data);

	return Ref(this);
}

Ref<GFEntity> GFEntity::add_tag(Variant tag) {
	GFWorld* w = get_world();

	ecs_entity_t tag_id = w->coerce_id(tag);
	if (ecs_has(w->raw(), tag_id, EcsComponent)) {
		ERR(Ref(this),
			"Failed to add tag to entity\n",
			"ID, ", tag_id, "is a component, not a tag\n"
			"(Tags are any non-component entity)"
		);
	}

	ecs_add_id(w->raw(), get_id(), tag_id);

	return Ref(this);
}

Ref<GFComponent> GFEntity::get_component(Variant component) {
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

Ref<GFEntity> GFEntity::set_component(Variant component, Variant data) {
	GFWorld* world = get_world();

	ecs_entity_t id = world->coerce_id(component);

	if (!ECS_IS_PAIR(id)) {
		if (!ecs_has(world->raw(), id, EcsComponent)) {
			ERR(Ref(this),
				"Failed to set data in component\n",
				"ID, ", id, "is not a component"
			);
		}
	} else {
		ecs_entity_t first_id = ECS_PAIR_FIRST(id);
		ecs_entity_t second_id = ECS_PAIR_SECOND(id);
		if (
			!ecs_has_id(world->raw(), first_id, ecs_id(EcsComponent))
			&& !ecs_has_id(world->raw(), second_id, ecs_id(EcsComponent))
		) {
			// ID is not a component, err
			ERR(Ref(this),
				"Failed to set data in pair\n",
				"Neither ID ", first_id,
				" nor ", second_id, "are components"
			);
		}
	}

	// TODO: change build_data_from_variant to a static method
	Ref(memnew(GFComponent(get_id(), id, world)))
		->build_data_from_variant(
			data,
			ecs_get_mut_id(world->raw(), get_id(), id)
		);
	ecs_modified_id(world->raw(), get_id(), id);

	return Ref(this);
}

void GFEntity::delete_() {
	ecs_delete(get_world()->raw(), get_id());
}

ecs_entity_t GFEntity::get_id() { return id; }
String GFEntity::get_path() {
	return String(ecs_get_path_w_sep(
		get_world()->raw(),
		0,
		get_id(),
		"/",
		"/root/"
	));
}
GFWorld* GFEntity::get_world() { return Object::cast_to<GFWorld>(
	UtilityFunctions::instance_from_id(world_instance_id)
); }

bool GFEntity::is_alive() {
	if (get_world() == nullptr) {
		return false;
	}
	if (!UtilityFunctions::is_instance_id_valid(world_instance_id)) {
		return false;
	}
	if (is_pair()) {
		if (!ecs_is_alive(get_world()->raw(), ECS_PAIR_FIRST(get_id()))) {
			return false;
		}
		if (!ecs_is_alive(get_world()->raw(), ECS_PAIR_SECOND(get_id()))) {
			return false;
		}
	} else {
		if (!ecs_is_alive(get_world()->raw(), get_id())) {
			return false;
		}
	}

	return true;
}
bool GFEntity::is_pair() {
	return ecs_id_is_pair(get_id());
}

String GFEntity::get_name() {
	return String(ecs_get_name(get_world()->raw(), get_id()));
}
Ref<GFEntity> GFEntity::set_name(String name_) {
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

Ref<GFPair> GFEntity::pair(Variant second) {
	return GFPair::from_id(pair_id(get_world()->coerce_id(second)), get_world());
}
ecs_entity_t GFEntity::pair_id(ecs_entity_t second) {
	return get_world()->pair_ids(get_id(), second);
}

// ----------------------------------------------
// --- Unexposed ---
// ----------------------------------------------

void GFEntity::set_id(ecs_entity_t value) { id = value; }
void GFEntity::set_world(GFWorld* value) { world_instance_id = value->get_instance_id(); }

void GFEntity::_bind_methods() {
	godot::ClassDB::bind_static_method(GFEntity::get_class_static(), D_METHOD("spawn", "world"), &GFEntity::spawn, nullptr);
	godot::ClassDB::bind_static_method(GFEntity::get_class_static(), D_METHOD("from", "entity", "world"), &GFEntity::from, nullptr);
	godot::ClassDB::bind_static_method(GFEntity::get_class_static(), D_METHOD("from_id", "id", "world"), &GFEntity::from_id, nullptr);

	godot::ClassDB::bind_method(D_METHOD("add_component", "component", "data"), &GFEntity::add_component, nullptr);
	godot::ClassDB::bind_method(D_METHOD("add_entity", "entity", "data"), &GFEntity::add_entity, nullptr);
	godot::ClassDB::bind_method(D_METHOD("add_pair", "first", "second", "data"), &GFEntity::add_pair, nullptr);
	godot::ClassDB::bind_method(D_METHOD("add_tag", "tag"), &GFEntity::add_tag);
	godot::ClassDB::bind_method(D_METHOD("get_component", "component"), &GFEntity::get_component);
	godot::ClassDB::bind_method(D_METHOD("set_component", "component", "value"), &GFEntity::set_component);

	godot::ClassDB::bind_method(D_METHOD("delete"), &GFEntity::delete_);

	godot::ClassDB::bind_method(D_METHOD("get_id"), &GFEntity::get_id);
	godot::ClassDB::bind_method(D_METHOD("get_path"), &GFEntity::get_path);
	godot::ClassDB::bind_method(D_METHOD("get_world"), &GFEntity::get_world);
	godot::ClassDB::bind_method(D_METHOD("get_name"), &GFEntity::get_name);

	godot::ClassDB::bind_method(D_METHOD("is_alive"), &GFEntity::is_alive);
	godot::ClassDB::bind_method(D_METHOD("is_pair"), &GFEntity::is_pair);

	godot::ClassDB::bind_method(D_METHOD("pair", "second"), &GFEntity::pair);
	godot::ClassDB::bind_method(D_METHOD("pair_id", "second_id"), &GFEntity::pair_id);

	godot::ClassDB::bind_method(D_METHOD("set_name", "name"), &GFEntity::set_name);
}
