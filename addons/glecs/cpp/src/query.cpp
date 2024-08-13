
#include "query.h"
#include "query_iterator.h"

#include <stdint.h>
#include <flecs.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

GlQuery::GlQuery() {

}
GlQuery::~GlQuery() {
	ecs_query_fini(query);
}

// --------------------------------------
// --- Exposed
// --------------------------------------

GlWorld* GlQuery::get_world() {
	return world;
}

Ref<GlQueryIterator> GlQuery::iterate() {
	return Ref(memnew(GlQueryIterator(
		Ref(this),
		ecs_query_iter(this->world->raw(), this->query)
	)));
}

// --------------------------------------
// --- Unexposed
// --------------------------------------

void GlQuery::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("get_world"), &GlQuery::get_world);
	godot::ClassDB::bind_method(D_METHOD("iterate"), &GlQuery::iterate);
}
