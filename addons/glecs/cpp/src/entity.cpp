
#include "entity.h"
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

Ref<GFEntity> GFEntity::spawn(GFWorld* world) {
	return from_id(ecs_new(world->raw()), world);
}
Ref<GFEntity> GFEntity::from(Variant entity, GFWorld* world) {
	return from_id(world->coerce_id(entity), world);
}
Ref<GFEntity> GFEntity::from_id(ecs_entity_t id, GFWorld* world_) {
	GFWorld* world = world_;
	if (world == nullptr) {
		// world = GlWorld::singleton();
	}
	Ref<GFEntity> e = memnew(GFEntity(
		id,
		world
	));
	if (!e->is_alive()) {
		ERR(nullptr,
			"Could not instantiate entity from ID\n",
			"World/ID is not valid/alive"
		);
	}

	return e;
}

Ref<GFEntity> GFEntity::add_component(Variant component) {
	GFWorld* w = get_world();
	ecs_entity_t component_id = w->coerce_id(component);
	ecs_add_id(w->raw(), get_id(), component_id);

	return Ref(this);
}

Ref<GFComponent> GFEntity::get_component(Variant component) {
	ecs_entity_t component_id = world->coerce_id(component);
	Ref<GFComponent> c = Variant(memnew(GFComponent));
	c->set_world(world);
	c->set_id(component_id);

	if (!c->is_alive()) {
		ERR(nullptr,
			"Component ID ", component_id, " is not alive."
		);
	}
	if (!ecs_has_id(world->raw(), id, component_id)) {
		ERR(nullptr,
			"Could not find attached component ID ", component_id, " on entity"
		);
	}

	c->set_source_id(id);

	return c;
}

ecs_entity_t GFEntity::get_id() { return id; }
GFWorld* GFEntity::get_world() { return world; }

bool GFEntity::is_alive() {
	if (world == nullptr) {
		return false;
	}
	if (!ObjectDB::get_instance(world->get_instance_id())) {
		return false;
	}
	if (is_pair()) {
		if (!ecs_is_alive(world->raw(), ECS_PAIR_FIRST(get_id()))) {
			return false;
		}
		if (!ecs_is_alive(world->raw(), ECS_PAIR_SECOND(get_id()))) {
			return false;
		}
	} else {
		if (!ecs_is_alive(world->raw(), get_id())) {
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
	return GFPair::from_id(pair_id(world->coerce_id(second)), world);
}
ecs_entity_t GFEntity::pair_id(ecs_entity_t second) {
	return world->pair_ids(get_id(), second);
}

// ----------------------------------------------
// --- Unexposed ---
// ----------------------------------------------

void GFEntity::set_id(ecs_entity_t value) { id = value; }
void GFEntity::set_world(GFWorld* value) { world = value; }

void GFEntity::_bind_methods() {
	godot::ClassDB::bind_static_method(GFEntity::get_class_static(), D_METHOD("spawn", "world"), &GFEntity::spawn, nullptr);
	godot::ClassDB::bind_static_method(GFEntity::get_class_static(), D_METHOD("from", "entity", "world"), &GFEntity::from, nullptr);
	godot::ClassDB::bind_static_method(GFEntity::get_class_static(), D_METHOD("from_id", "id", "world"), &GFEntity::from_id, nullptr);

	godot::ClassDB::bind_method(D_METHOD("add_component", "component"), &GFEntity::add_component);
	godot::ClassDB::bind_method(D_METHOD("get_component", "component"), &GFEntity::get_component);

	godot::ClassDB::bind_method(D_METHOD("get_id"), &GFEntity::get_id);
	godot::ClassDB::bind_method(D_METHOD("get_world"), &GFEntity::get_world);
	godot::ClassDB::bind_method(D_METHOD("get_name"), &GFEntity::get_name);

	godot::ClassDB::bind_method(D_METHOD("is_alive"), &GFEntity::is_alive);
	godot::ClassDB::bind_method(D_METHOD("is_pair"), &GFEntity::is_pair);

	godot::ClassDB::bind_method(D_METHOD("pair", "second"), &GFEntity::pair);
	godot::ClassDB::bind_method(D_METHOD("pair_id", "second_id"), &GFEntity::pair_id);

	godot::ClassDB::bind_method(D_METHOD("set_name", "name"), &GFEntity::set_name);
}
