

#include "query_builder.h"
#include "world.h"
#include "query.h"
#include "utils.h"

#include <stdlib.h>
#include <flecs.h>

using namespace godot;

GlQueryBuilder::GlQueryBuilder() {
}

GlQueryBuilder::~GlQueryBuilder() {
}

Ref<GlQuery> GlQueryBuilder::build() {
	const char* FAILED_TO_BUILD = "Failed to build query\n";
	if (is_built()) {
		ERR(nullptr,
			FAILED_TO_BUILD,
			"Query builder was already built"
		);
	}
	built = true;

	query_desc.entity = ecs_new(get_world()->raw());
	auto query = ecs_query_init(
		get_world()->raw(),
		&query_desc
	);

	return Ref(memnew(GlQuery(get_world(), query)));
}

// **********************************************
// *** PROTECTED ***
// **********************************************

void GlQueryBuilder::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("build"), &GlQueryBuilder::build);
}

// **********************************************
// *** PRIVATE ***
// **********************************************
