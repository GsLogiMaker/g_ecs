
#include "query_iterator.h"
#include "component.h"

#include <stdint.h>
#include <flecs.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

GlQueryIterator::GlQueryIterator() {
}

GlQueryIterator::~GlQueryIterator() {
	if (!done) {
		ecs_iter_fini(&iterator);
	}
}

// --------------------------------------
// --- Exposed ---
// --------------------------------------

bool GlQueryIterator::_iter_init(Variant arg) {
	done = false;
	done = !ecs_query_next(&iterator);
	return !done;
}
bool GlQueryIterator::_iter_next(Variant arg) {
	index += 1;
	if (index == iterator.count) {
		index = 0;
		done = !ecs_query_next(&iterator);
	}
	return !done;
}
Variant GlQueryIterator::_iter_get(Variant arg) {
	Array arr = Array();
	for (int i=0; i != iterator.field_count; i++) {
		arr.append(Ref(memnew(GlComponent(
			*iterator.entities,
			iterator.ids[i],
			query->get_world()
		))));
	}
	return arr;
}

GlWorld* GlQueryIterator::get_world() {
	return query->get_world();
}

// --------------------------------------
// --- Protected ---
// --------------------------------------

void GlQueryIterator::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("_iter_init", "arg"), &GlQueryIterator::_iter_init);
	godot::ClassDB::bind_method(D_METHOD("_iter_next", "arg"), &GlQueryIterator::_iter_next);
	godot::ClassDB::bind_method(D_METHOD("_iter_get", "arg"), &GlQueryIterator::_iter_get);
	godot::ClassDB::bind_method(D_METHOD("get_world"), &GlQueryIterator::get_world);
}
