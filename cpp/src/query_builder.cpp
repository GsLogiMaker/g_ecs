

#include "query_builder.h"
#include "godot_cpp/variant/callable.hpp"
#include "world.h"
#include "query.h"
#include "utils.h"

#include <stdlib.h>
#include <flecs.h>

using namespace godot;

GFQueryBuilder::~GFQueryBuilder() {
}

Ref<GFQueryBuilder> GFQueryBuilder::new_in_world(GFWorld* world) {
	return memnew(GFQueryBuilder(world));
}

Ref<GFQuery> GFQueryBuilder::build() {
	const char* FAILED_TO_BUILD = "Failed to build query\n";
	if (is_built()) {
		ERR(nullptr,
			FAILED_TO_BUILD,
			"Query builder was already built"
		);
	}
	built = true;

	setup_ctx(Callable());

	query_desc.entity = ecs_new(get_world()->raw());
	auto query = ecs_query_init(
		get_world()->raw(),
		&query_desc
	);

	return Ref(memnew(GFQuery(get_world(), query)));
}

// **********************************************
// *** PROTECTED ***
// **********************************************

void GFQueryBuilder::_bind_methods() {
	godot::ClassDB::bind_static_method(get_class_static(), D_METHOD("new_in_world", "world"), &GFQueryBuilder::new_in_world);
	godot::ClassDB::bind_method(D_METHOD("build"), &GFQueryBuilder::build);
}

// **********************************************
// *** PRIVATE ***
// **********************************************
