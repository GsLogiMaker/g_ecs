
#include "query.h"
#include "query_iterator.h"

#include <stdint.h>
#include <flecs.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

// --------------------------------------
// --- Exposed
// --------------------------------------

GFWorld* GFQuery::get_world() const { return Object::cast_to<GFWorld>(
	UtilityFunctions::instance_from_id(world_instance_id)
); }

Ref<GFQueryIterator> GFQuery::iter() {
	ecs_iter_t iter = ecs_query_iter(get_world()->raw(), this->query);
	return Ref(memnew(GFQueryIterator(
		iter,
		get_world()
	)));
}

// --------------------------------------
// --- Unexposed
// --------------------------------------

void GFQuery::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("get_world"), &GFQuery::get_world);
	godot::ClassDB::bind_method(D_METHOD("iter"), &GFQuery::iter);
}
