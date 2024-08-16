
#include "query_iterator.h"
#include "godot_cpp/variant/array.hpp"
#include "querylike_builder.h"
#include "utils.h"

#include <stdint.h>
#include <flecs.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

GlQueryIterator::GlQueryIterator() {
}

GlQueryIterator::~GlQueryIterator() {
	if (!is_done()) {
		ecs_iter_fini(&iterator);
	}
}

// --------------------------------------
// --- Exposed ---
// --------------------------------------

bool GlQueryIterator::_iter_init(Variant arg) {
	return _iter_next(arg);
}
bool GlQueryIterator::_iter_next(Variant arg) {
	if (is_done()) {
		ERR(false,
			"Can't get next in GlQueryIterator\n",
			"Iterator is already exhausted"
		);
	}

	QueryIterationContext* ctx = (QueryIterationContext*) iterator.query->binding_ctx;

	if (index == iterator.count) {
		index = 0;
		set_done(!ecs_query_next(&iterator));
		if (is_done()) {
			return false;
		}
		ctx->update_component_terms(&iterator);
	}
	ctx->update_component_entities(&iterator, index);

	index += 1;
	return !is_done();
}
Variant GlQueryIterator::_iter_get(Variant arg) {
	QueryIterationContext* ctx = (QueryIterationContext*) iterator.query->binding_ctx;
	return ctx->comp_ref_args;
}

GlWorld* GlQueryIterator::get_world() {
	return query->get_world();
}

bool GlQueryIterator::is_done() {
	return (iter_flags & 1 << 0) != 0;
}
bool GlQueryIterator::is_started() {
	return (iter_flags & 1 << 1) != 0;
}

void GlQueryIterator::set_done(bool value) {
	uint8_t new_value = ((uint8_t) value) << 0;
	uint8_t other_flags = iter_flags & ~new_value;
	iter_flags = other_flags | new_value;
}
void GlQueryIterator::set_started(bool value) {
	uint8_t new_value = ((uint8_t) value) << 1;
	uint8_t other_flags = iter_flags & ~new_value;
	iter_flags = other_flags | new_value;
}

// --------------------------------------
// --- Unexposed
// --------------------------------------


// --------------------------------------
// --- Protected ---
// --------------------------------------

void GlQueryIterator::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("_iter_init", "arg"), &GlQueryIterator::_iter_init);
	godot::ClassDB::bind_method(D_METHOD("_iter_next", "arg"), &GlQueryIterator::_iter_next);
	godot::ClassDB::bind_method(D_METHOD("_iter_get", "arg"), &GlQueryIterator::_iter_get);
	godot::ClassDB::bind_method(D_METHOD("get_world"), &GlQueryIterator::get_world);
	godot::ClassDB::bind_method(D_METHOD("is_done"), &GlQueryIterator::is_done);
	godot::ClassDB::bind_method(D_METHOD("is_started"), &GlQueryIterator::is_started);
}
