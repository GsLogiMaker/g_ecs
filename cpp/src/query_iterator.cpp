
#include "query_iterator.h"
#include "query_iteration_context.h"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "querylike_builder.h"

#include <stdint.h>
#include <flecs.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

// --------------------------------------
// --- Exposed ---
// --------------------------------------

bool GFQueryIterator::_iter_init(Variant arg) {
	return GFQueryIterator::_iter_next(arg);
}

bool GFQueryIterator::_iter_next(Variant arg) {
	return GFQueryIterator::next();
}

Variant GFQueryIterator::_iter_get(Variant arg) {
	QueryIterationContext* ctx = static_cast<QueryIterationContext*>(
		iterator.query->binding_ctx
	);
	return ctx->comp_ref_args;
}

// --------------------------------------
// --- Unexposed
// --------------------------------------

bool GFQueryIterator::next() {
	GFQueryIterator* self = this;
	auto per_table = [self]{
		self->get_context()->update_component_terms(&self->iterator);
	};
	auto per_entity = [self]{
		self->get_context()->update_component_entities(
			&self->iterator,
			self->index
		);
	};
	NEXT(
		ecs_query_next,
		per_table,
		per_entity
	);
}


// --------------------------------------
// --- Protected ---
// --------------------------------------

void GFQueryIterator::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("_iter_init", "arg"), &GFQueryIterator::_iter_init);
	godot::ClassDB::bind_method(D_METHOD("_iter_next", "arg"), &GFQueryIterator::_iter_next);
	godot::ClassDB::bind_method(D_METHOD("_iter_get", "arg"), &GFQueryIterator::_iter_get);
}
