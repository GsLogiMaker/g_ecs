
#ifndef GF_TAG_H
#define GF_TAG_H

#include "registerable_entity.h"
#include "world.h"

#include <flecs.h>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot {

	class GFTag : public GFRegisterableEntity {
		GDCLASS(GFTag, GFRegisterableEntity)

	public:
		/// Create a new named module
		GFTag(GFWorld* world):
			GFRegisterableEntity(
				ecs_new(
					world->raw()
				),
				world
			)
		{}
		/// Create from existing ID
		GFTag(ecs_entity_t id, GFWorld* world):
			GFRegisterableEntity(
				id,
				world
			)
		{}
		GFTag():
			GFRegisterableEntity(GFWorld::singleton())
		{}

		~GFTag();

		// --------------------------------------
		// --- Exposed
		// --------------------------------------
		OVERRIDE_ENTITY_SELF_METHODS(GFTag);

		static Ref<GFTag> new_in_world(GFWorld* world);
		static Ref<GFTag> from(const Variant module, GFWorld*);
		static Ref<GFTag> from_id(ecs_entity_t, GFWorld*);

		// --------------------------------------
		// --- Unexposed
		// --------------------------------------

	protected:
		static void _bind_methods();


	private:

	};

}

#endif
