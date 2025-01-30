
#ifndef QUERY_H
#define QUERY_H

#include "gdextension_interface.h"
#include "world.h"
#include <flecs.h>
#include <godot_cpp/classes/ref_counted.hpp>

namespace godot {

	class GFQueryIterator;

	class GFQuery : public RefCounted {
		GDCLASS(GFQuery, RefCounted)

	public:
		GFQuery(const GFWorld* world, ecs_query_t* query):
			world_instance_id(world->get_instance_id()),
			query(query)
		{}
		GFQuery():
			GFQuery(nullptr, nullptr)
		{}
		~GFQuery() {
			ecs_query_fini(query);
		};

		// --------------------------------------
		// --- Exposed
		// --------------------------------------

		GFWorld* get_world() const;

		Ref<GFQueryIterator> iter();

		// --------------------------------------
		// --- Unexposed
		// --------------------------------------

	protected:
		static void _bind_methods();

	private:
		GDObjectInstanceID world_instance_id;
		ecs_query_t* query;
	};

}

#endif
