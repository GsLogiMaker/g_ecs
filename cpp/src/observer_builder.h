
#ifndef OBSERVER_BUILDER_H
#define OBSERVER_BUILDER_H

#include <querylike_builder.h>

#include <flecs.h>
#include <godot_cpp/classes/ref_counted.hpp>

namespace godot {

	// Predefine instead of include to avoid cyclic dependencies
	class GFWorld;

	class GFObserverBuilder : public GFQuerylikeBuilder {
		GDCLASS(GFObserverBuilder, GFQuerylikeBuilder)

	public:
		GFObserverBuilder(GFWorld* world): GFQuerylikeBuilder(world) {}
		GFObserverBuilder(): GFObserverBuilder(GFWorld::singleton()) {}
		~GFObserverBuilder();

		// **************************************
		// *** Exposed ***
		// **************************************

		static Ref<GFObserverBuilder> new_in_world(GFWorld*);

		void for_each(Callable callable);
		Ref<GFObserverBuilder> set_events_varargs(
			const Variant** args,
			GDExtensionInt arg_count,
			GDExtensionCallError& error
		);
		Ref<GFObserverBuilder> set_event(int index, Variant event);

		// **************************************
		// *** Unexposed ***
		// **************************************

	protected:
		static void _bind_methods();

	private:
		ecs_entity_t events[FLECS_EVENT_DESC_MAX];
	};

}

#endif
