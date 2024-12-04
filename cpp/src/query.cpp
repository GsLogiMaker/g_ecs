
#include "query.h"
#include "query_iterator.h"

#include <stdint.h>
#include <flecs.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

GFQuery::~GFQuery() {
	ecs_query_fini(query);
}

// --------------------------------------
// --- Exposed
// --------------------------------------

GFWorld* GFQuery::get_world() {
	return world;
}

Ref<GFQueryIterator> GFQuery::iterate() {
	return Ref(memnew(GFQueryIterator(
		Ref(this),
		ecs_query_iter(this->world->raw(), this->query)
	)));
}

// --------------------------------------
// --- Unexposed
// --------------------------------------

void GFQuery::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("get_world"), &GFQuery::get_world);
	godot::ClassDB::bind_method(D_METHOD("iterate"), &GFQuery::iterate);
}
