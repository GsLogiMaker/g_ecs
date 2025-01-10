
#include "tag.h"
#include "utils.h"
#include <stdint.h>
#include <flecs.h>
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/array.hpp"
#include "world.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

GFTag::~GFTag() {
}

Ref<GFTag> GFTag::new_in_world(GFWorld* world) {
	return memnew(GFTag(world));
}

Ref<GFTag> GFTag::from(Variant tag, GFWorld* world) {
	ecs_entity_t tag_id = world->coerce_id(tag);
	return from_id(tag_id, world);
}

Ref<GFTag> GFTag::from_id(ecs_entity_t tag_id, GFWorld* world_) {
	GFWorld* world = GFWorld::world_or_singleton(world_);
	Ref<GFTag> ett = memnew(GFTag(
		tag_id,
		world
	));
	if (!ett->is_alive()) {
		ERR(nullptr,
			"Could not instantiate tag from ID\n",
			"World/ID is not valid/alive"
		);
	}
	return ett;
}


void GFTag::_bind_methods() {
	godot::ClassDB::bind_static_method(get_class_static(), D_METHOD("new_in_world", "world"), &GFTag::new_in_world);
	godot::ClassDB::bind_static_method(get_class_static(), D_METHOD("from", "tag", "world"), &GFTag::from, nullptr);
	godot::ClassDB::bind_static_method(get_class_static(), D_METHOD("from_id", "tag_id", "world"), &GFTag::from_id, nullptr);
}
