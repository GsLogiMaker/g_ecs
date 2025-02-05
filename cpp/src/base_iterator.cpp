
#include "base_iterator.h"
#include "entity.h"
#include "godot_cpp/variant/array.hpp"
#include "querylike_builder.h"
#include "world.h"

#include <stdint.h>
#include <flecs.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

GFBaseIterator::~GFBaseIterator() {
	if (!is_done()) {
		ecs_iter_fini(&iterator);
	}
}

// --------------------------------------
// --- Exposed ---
// --------------------------------------

bool GFBaseIterator::_iter_init(Variant arg) {
	return _iter_next(arg);
}

bool GFBaseIterator::_iter_next(Variant arg) {
	return GFBaseIterator::next();
}

Variant GFBaseIterator::_iter_get(Variant arg) {
	GFWorld* w = get_world();
	return GFEntity::from_id(iterator.entities[index-1], w);
}

GFWorld* GFBaseIterator::get_world() const {
	return world;
}

bool GFBaseIterator::is_done() const {
	return (iter_flags & 1 << 0) != 0;
}

bool GFBaseIterator::is_started() const {
	return (iter_flags & 1 << 1) != 0;
}

Array GFBaseIterator::into_array() {
	Array arr = Array();
	while (_iter_next({})) {
		arr.append(_iter_get({}));
	}
	return arr;
}


// --------------------------------------
// --- Unexposed
// --------------------------------------

QueryIterationContext* GFBaseIterator::get_context() {
	return static_cast<QueryIterationContext*>(
		iterator.query->binding_ctx
	);
}


bool GFBaseIterator::next() {
	NEXT(
		ecs_iter_next,
		[]{},
		[]{}
	);
}
void GFBaseIterator::set_done(bool value) {
	uint8_t new_value = ((uint8_t) value) << 0;
	uint8_t other_flags = iter_flags & ~new_value;
	iter_flags = other_flags | new_value;
}

void GFBaseIterator::set_started(bool value) {
	uint8_t new_value = ((uint8_t) value) << 1;
	uint8_t other_flags = iter_flags & ~new_value;
	iter_flags = other_flags | new_value;
}

// --------------------------------------
// --- Protected ---
// --------------------------------------

void GFBaseIterator::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("_iter_init", "arg"), &GFBaseIterator::_iter_init);
	godot::ClassDB::bind_method(D_METHOD("_iter_next", "arg"), &GFBaseIterator::_iter_next);
	godot::ClassDB::bind_method(D_METHOD("_iter_get", "arg"), &GFBaseIterator::_iter_get);
	godot::ClassDB::bind_method(D_METHOD("get_world"), &GFBaseIterator::get_world);
	godot::ClassDB::bind_method(D_METHOD("is_done"), &GFBaseIterator::is_done);
	godot::ClassDB::bind_method(D_METHOD("is_started"), &GFBaseIterator::is_started);
	godot::ClassDB::bind_method(D_METHOD("into_array"), &GFBaseIterator::into_array);
}
