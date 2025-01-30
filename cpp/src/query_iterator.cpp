
#include "base_iterator.h"
#include "query_iterator.h"
#include "godot_cpp/variant/array.hpp"
#include "querylike_builder.h"

#include <stdint.h>
#include <flecs.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

GFQueryIterator::~GFQueryIterator() {
	if (!is_done()) {
		ecs_iter_fini(&iterator);
	}
}

// --------------------------------------
// --- Exposed ---
// --------------------------------------

bool GFQueryIterator::_iter_next(Variant arg) {
	return GFQueryIterator::next();
}

// --------------------------------------
// --- Unexposed
// --------------------------------------

bool GFQueryIterator::next() {
	QueryIterationContext* ctx = static_cast<QueryIterationContext*>(
		iterator.query->binding_ctx
	);
	ecs_iter_t* it = &iterator;
	int i = index;
	auto per_table = [ctx, it]{ ctx->update_component_terms(it); };
	auto per_entity = [ctx, it, i]{ ctx->update_component_entities(it, i); };
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
	godot::ClassDB::bind_method(D_METHOD("_iter_next", "arg"), &GFQueryIterator::_iter_next);
}
