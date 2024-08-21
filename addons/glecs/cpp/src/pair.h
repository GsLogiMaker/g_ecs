
#ifndef PAIR_H
#define PAIR_H

#include "component_builder.h"
#include "entity.h"

#include <cassert>
#include <flecs.h>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot {

	class GFPair : public GFEntity {
		GDCLASS(GFPair, GFEntity)

	public:
		GFPair();
		GFPair(ecs_entity_t first, ecs_entity_t second, GFWorld* world):
			GFEntity(ecs_make_pair(first, second), world)
			{}
		GFPair(ecs_entity_t pair_id, GFWorld* world):
			GFEntity(pair_id, world)
			{}
		~GFPair();

		// --------------------------------------
		// --- Exposed
		// --------------------------------------

		static Ref<GFPair> new_();
		static Ref<GFPair> from(Variant first, Variant second, GFWorld*);
		static Ref<GFPair> from_id(ecs_entity_t, GFWorld*);
		static Ref<GFPair> from_ids(ecs_entity_t, ecs_entity_t, GFWorld*);

		Ref<GFEntity> first();
		ecs_entity_t first_id();
		Ref<GFEntity> second();
		ecs_entity_t second_id();

		// --------------------------------------
		// --- Unexposed
		// --------------------------------------

	protected:
		static void _bind_methods();


	private:
	};

}

#endif
