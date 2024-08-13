
#ifndef QUERY_H
#define QUERY_H

#include "world.h"
#include <flecs.h>
#include <godot_cpp/classes/ref_counted.hpp>

namespace godot {

	class GlQueryIterator;

	class GlQuery : public RefCounted {
		GDCLASS(GlQuery, RefCounted)

	public:
		GlQuery();
		GlQuery(GlWorld* world, ecs_query_t* query): world(world), query(query) {}
		~GlQuery();

		// --------------------------------------
		// --- Exposed
		// --------------------------------------

		GlWorld* get_world();

		Ref<GlQueryIterator> iterate();

		// --------------------------------------
		// --- Unexposed
		// --------------------------------------

	protected:
		static void _bind_methods();

	private:
		GlWorld* world;
		ecs_query_t* query;
	};

}

#endif
