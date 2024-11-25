

#include "system_builder.h"
#include "godot_cpp/classes/object.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "querylike_builder.h"
#include "world.h"
#include "utils.h"

#include <stdlib.h>
#include <flecs.h>

using namespace godot;

GFSystemBuilder::GFSystemBuilder() {
}

GFSystemBuilder::~GFSystemBuilder() {
}

void GFSystemBuilder::for_each(Callable callable) {
	const char* FAILED_TO_BUILD = "Failed to build system\n";
	if (is_built()) {
		ERR(/**/,
			FAILED_TO_BUILD,
			"System builder was already built"
		);
	}
	built = true;

	setup_ctx(callable);

	ecs_entity_t sys_id = ecs_new(get_world()->raw());
	ecs_add_id(get_world()->raw(), sys_id, ecs_dependson(EcsOnUpdate));
	ecs_system_desc_t desc = {
		.entity = sys_id,
		.query = query_desc,
		.callback = QueryIterationContext::iterator_callback
	};
	ecs_system_init(get_world()->raw(), &desc);

}

// **********************************************
// *** PROTECTED ***
// **********************************************



void GFSystemBuilder::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("for_each", "callback"), &GFSystemBuilder::for_each);
}

// **********************************************
// *** PRIVATE ***
// **********************************************
