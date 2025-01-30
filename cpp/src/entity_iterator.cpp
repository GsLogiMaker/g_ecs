
#include "entity_iterator.h"
#include "godot_cpp/variant/array.hpp"

#include <stdint.h>
#include <flecs.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

// --------------------------------------
// --- Exposed ---
// --------------------------------------

bool GFEntityIterator::_iter_next(Variant arg) {
	return GFEntityIterator::next();
}

// --------------------------------------
// --- Unexposed
// --------------------------------------

bool GFEntityIterator::next() {
	NEXT(
		ecs_each_next,
		[]{},
		[]{}
	);
}

// --------------------------------------
// --- Protected ---
// --------------------------------------

void GFEntityIterator::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("_iter_next", "arg"), &GFEntityIterator::_iter_next);
}
