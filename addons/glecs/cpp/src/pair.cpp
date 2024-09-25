
#include "pair.h"
#include "entity.h"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/array.hpp"
#include "utils.h"

#include <stdint.h>
#include <flecs.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

GFPair::GFPair() {
}
GFPair::~GFPair() {
}

Ref<GFPair> GFPair::spawn() {
	ERR(nullptr,
		"Couldn't instantiate entity\n",
		"Pairs cannot be instantiated. Use \"from\" instead"
	);
}

Ref<GFPair> GFPair::from(Variant first, Variant second, GFWorld* world) {
	ecs_entity_t first_id = world->coerce_id(first);
	ecs_entity_t second_id = world->coerce_id(second);
	return from_ids(first_id, second_id, world);
}

Ref<GFPair> GFPair::from_id(ecs_entity_t pair_id, GFWorld* world_) {
	GFWorld* world = GFWorld::world_or_singleton(world_);
	if (!ECS_IS_PAIR(pair_id)) {
		ERR(nullptr,
			"Could not instantiate pair from ID\n",
			"ID is not a pair"
		);
	}
	Ref<GFPair> pair = memnew(GFPair(
		pair_id,
		world
	));
	if (!pair->is_alive()) {
		ERR(nullptr,
			"Could not instantiate pair from ID\n",
			"World/ID is not valid/alive"
		);
	}
	return pair;
}

Ref<GFPair> GFPair::from_ids(ecs_entity_t first_id, ecs_entity_t second_id, GFWorld* world) {
	return from_id(world->pair_ids(first_id, second_id), world);
}

Ref<GFEntity> GFPair::first() {
	return GFEntity::from(first_id(), get_world());
}
ecs_entity_t GFPair::first_id() {
	return ECS_PAIR_FIRST(get_id());
}
Ref<GFEntity> GFPair::second() {
	return GFEntity::from(second_id(), get_world());
}
ecs_entity_t GFPair::second_id() {
	return ECS_PAIR_SECOND(get_id());
}

void GFPair::_bind_methods() {
	godot::ClassDB::bind_static_method(get_class_static(), D_METHOD("spawn"), &GFPair::spawn);
	godot::ClassDB::bind_static_method(get_class_static(), D_METHOD("from", "first", "second", "world"), &GFPair::from, nullptr);
	godot::ClassDB::bind_static_method(get_class_static(), D_METHOD("from_id", "pair_id", "world"), &GFPair::from_id, nullptr);
	godot::ClassDB::bind_static_method(get_class_static(), D_METHOD("from_ids", "first", "second", "world"), &GFPair::from_ids, nullptr);

	godot::ClassDB::bind_method(D_METHOD("first"), &GFPair::first);
	godot::ClassDB::bind_method(D_METHOD("first_id"), &GFPair::first_id);
	godot::ClassDB::bind_method(D_METHOD("second"), &GFPair::second);
	godot::ClassDB::bind_method(D_METHOD("second_id"), &GFPair::second_id);
}
