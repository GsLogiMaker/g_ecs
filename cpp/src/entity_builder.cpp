

#include "entity_builder.h"
#include "querylike_builder.h"
#include <godot_cpp/core/class_db.hpp>
#include "godot_cpp/variant/callable.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "utils.h"
#include "world.h"

#include <stdlib.h>
#include <flecs.h>

using namespace godot;


// **************************************
// *** Exposed ***
// **************************************

Ref<GFEntityBuilder> GFEntityBuilder::new_in_world(GFWorld* world) {
	return Ref(memnew(GFEntityBuilder(world)));
}

Ref<GFEntityBuilder> GFEntityBuilder::add_entity(Variant entity) {
	ecs_entity_t id = world->coerce_id(entity);
	CHECK_ENTITY_ALIVE(id, world, this,
		"Failed to entity in builder\n"
	);
	ids.append(id);

	return this;
}

Ref<GFEntityBuilder> GFEntityBuilder::add_pair(Variant first, Variant second) {
	ecs_entity_t first_id = world->coerce_id(first);
	CHECK_ENTITY_ALIVE(first_id, world, this,
		"Failed to add first of pair in builder\n"
	);
	ecs_entity_t second_id = world->coerce_id(second);
	CHECK_ENTITY_ALIVE(second_id, world, this,
		"Failed to add second of pair in builder\n"
	);

	ids.append(ecs_pair(first_id, second_id));

	return this;
}

Ref<GFEntity> GFEntityBuilder::build() {
	CharString name_utf8 = name.utf8();
	desc.name = name_utf8.get_data();

	ids.append(0); // Add null terminator (required by ecs_entity_desc_t)
	desc.add = reinterpret_cast<const ecs_id_t*>(ids.ptr());

	ecs_entity_t id = ecs_entity_init(
		world->raw(),
		&desc
	);

	// Reset volatile pointers/data
	desc.name = nullptr;
	desc.add = nullptr;
	ids.remove_at(ids.size()-1); // Remove null terminator

	built_count += 1;

	return GFEntity::from_id(id, world);
}

GFWorld* GFEntityBuilder::get_world() {
	return world;
}

Ref<GFEntityBuilder> GFEntityBuilder::set_target_entity(Variant entity) {
	ecs_entity_t id = world->coerce_id(entity);

	CHECK_ENTITY_ALIVE(id, world, this,
		"Failed to set entity in builder\n"
	);

	desc.id = id;
	return this;
}

Ref<GFEntityBuilder> GFEntityBuilder::set_name(String name_) {
	name = name_;
	return this;
}

Ref<GFEntityBuilder> GFEntityBuilder::set_parent(Variant parent) {
	ecs_entity_t parent_id = world->coerce_id(parent);
	CHECK_ENTITY_ALIVE(parent_id, world, this,
		"Failed to set parent in builder\n"
	);
	desc.parent = parent_id;
	return this;
}

// **************************************
// *** Unexposed ***
// **************************************

void GFEntityBuilder::set_world(GFWorld* world_) {
	world = world_;
}

// **********************************************
// *** PROTECTED ***
// **********************************************

void GFEntityBuilder::_bind_methods() {
	godot::ClassDB::bind_static_method(GFEntityBuilder::get_class_static(), D_METHOD("new_in_world", "world"), &GFEntityBuilder::new_in_world);
	godot::ClassDB::bind_method(D_METHOD("add_entity", "entity"), &GFEntityBuilder::add_entity);
	godot::ClassDB::bind_method(D_METHOD("add_pair", "first", "second"), &GFEntityBuilder::add_pair);
	godot::ClassDB::bind_method(D_METHOD("build"), &GFEntityBuilder::build);
	godot::ClassDB::bind_method(D_METHOD("get_world"), &GFEntityBuilder::get_world);
	godot::ClassDB::bind_method(D_METHOD("set_target_entity", "entity"), &GFEntityBuilder::set_target_entity);
	godot::ClassDB::bind_method(D_METHOD("set_name", "name"), &GFEntityBuilder::set_name);
	godot::ClassDB::bind_method(D_METHOD("set_parent", "parent"), &GFEntityBuilder::set_parent);

}

// **********************************************
// *** PRIVATE ***
// **********************************************
