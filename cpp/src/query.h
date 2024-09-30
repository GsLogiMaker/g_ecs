
#ifndef QUERY_H
#define QUERY_H

#include "world.h"
#include <flecs.h>
#include <godot_cpp/classes/ref_counted.hpp>

namespace godot {

	class GFQueryIterator;

	class GFQuery : public RefCounted {
		GDCLASS(GFQuery, RefCounted)

	public:
		GFQuery();
		GFQuery(GFWorld* world, ecs_query_t* query): world(world), query(query) {}
		~GFQuery();

		// --------------------------------------
		// --- Exposed
		// --------------------------------------

		GFWorld* get_world();

		Ref<GFQueryIterator> iterate();

		// --------------------------------------
		// --- Unexposed
		// --------------------------------------

	protected:
		static void _bind_methods();

	private:
		GFWorld* world;
		ecs_query_t* query;
	};

}

#endif
