

#include "observer_builder.h"
#include "querylike_builder.h"
#include "world.h"
#include "utils.h"
#include <flecs.h>

using namespace godot;

GFObserverBuilder::GFObserverBuilder() {
}

GFObserverBuilder::~GFObserverBuilder() {
}

void GFObserverBuilder::for_each(Callable callable) {
	const char* FAILED_TO_BUILD = "Failed to build observer\n";
	if (is_built()) {
		ERR(/**/,
			FAILED_TO_BUILD,
			"Observer builder was already built"
		);
	}
	built = true;

	setup_ctx(callable);

	ecs_entity_t obs_id = ecs_new(get_world()->raw());
	ecs_observer_desc_t desc = {
		.entity = obs_id,
		.query = query_desc,
		.events = {*events},
		.callback = QueryIterationContext::iterator_callback
	};
	ecs_observer_init(get_world()->raw(), &desc);
}

Ref<GFObserverBuilder> GFObserverBuilder::set_event(int index, Variant event) {
	events[index] = get_world()->coerce_id(event);
	return Ref(this);
}

Ref<GFObserverBuilder> GFObserverBuilder::set_events_varargs(
	const Variant** args,
	int64_t arg_count,
	GDExtensionCallError& error
) {
	if (arg_count > FLECS_EVENT_DESC_MAX) {
		// TODO: utilize call error
		ERR(Ref(this),
			"Failed to set terms in observer builder",
			"Max term count is ", FLECS_EVENT_DESC_MAX, ", but ",
			arg_count, " terms were passed"
		);
	}
	for (int i=0; i != FLECS_EVENT_DESC_MAX; i++) {
		if (i < arg_count) {
			events[i] = get_world()->coerce_id(*args[i]);
		} else {
			events[i] = 0;
		}
	}
	return Ref(this);
}

// **********************************************
// *** PROTECTED ***
// **********************************************



void GFObserverBuilder::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("for_each", "callback"), &GFObserverBuilder::for_each);
	godot::ClassDB::bind_method(D_METHOD("set_event", "index", "event"), &GFObserverBuilder::set_event);
	ClassDB::bind_vararg_method(METHOD_FLAGS_DEFAULT, "set_events", &GFObserverBuilder::set_events_varargs);
}

// **********************************************
// *** PRIVATE ***
// **********************************************
