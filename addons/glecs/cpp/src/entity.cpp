
#include "entity.h"
#include "utils.h"
// needed here because entity.h does not include
// component.h, but uses forward declaration instead
#include "component.h"

#include <flecs.h>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include "godot_cpp/variant/variant.hpp"

using namespace godot;

GlEntity::GlEntity() {
}
GlEntity::~GlEntity() {
}

Ref<GlEntity> GlEntity::spawn(GlWorld* world) {
	GlWorld* world_ = world;
	if (world_ == nullptr) {
		// world_ = GlWorld::singleton();
	}

	Ref<GlEntity> e = Variant(memnew(GlEntity(
		ecs_new(world_->raw()),
		world_
	)));
	return e;
}
Ref<GlEntity> GlEntity::from(Variant entity, GlWorld* world) {
	GlWorld* world_ = world;
	if (world_ == nullptr) {
		// world_ = GlWorld::singleton();
	}

	Ref<GlEntity> e = memnew(GlEntity(
		world_->coerce_id(entity),
		world_
	));

	if (!e->is_alive()) {
		return Variant(nullptr);
	}

	return e;
}

Ref<GlEntity> GlEntity::add_component(Variant component) {
	GlWorld* w = get_world();
	ecs_entity_t component_id = w->coerce_id(component);
	ecs_add_id(w->raw(), get_id(), component_id);

	return Ref(this);
}

Ref<GlComponent> GlEntity::get_component(Variant component) {
	ecs_entity_t component_id = world->coerce_id(component);
	Ref<GlComponent> c = Variant(memnew(GlComponent));
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

bool GlEntity::is_alive() {
	return world != nullptr
		&& ObjectDB::get_instance(world->get_instance_id())
		&& ecs_is_alive(world->raw(), get_id());
}

ecs_entity_t GlEntity::get_id() { return id; }
GlWorld* GlEntity::get_world() { return world; }

// ----------------------------------------------
// --- Unexposed ---
// ----------------------------------------------

void GlEntity::set_id(ecs_entity_t value) { id = value; }
void GlEntity::set_world(GlWorld* value) { world = value; }

void GlEntity::_bind_methods() {
	godot::ClassDB::bind_static_method(GlEntity::get_class_static(), D_METHOD("spawn", "world"), &GlEntity::spawn, nullptr);
	godot::ClassDB::bind_static_method(GlEntity::get_class_static(), D_METHOD("from", "id", "world"), &GlEntity::from, nullptr);

	godot::ClassDB::bind_method(D_METHOD("add_component", "component"), &GlEntity::add_component);
	godot::ClassDB::bind_method(D_METHOD("get_component", "component"), &GlEntity::get_component);

	godot::ClassDB::bind_method(D_METHOD("get_id"), &GlEntity::get_id);
	godot::ClassDB::bind_method(D_METHOD("get_world"), &GlEntity::get_world);
}
