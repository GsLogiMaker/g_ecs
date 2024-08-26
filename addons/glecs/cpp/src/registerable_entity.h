
#ifndef REGISTERABLE_ENTITY_H
#define REGISTERABLE_ENTITY_H

#include "entity.h"
#include "world.h"

#include <flecs.h>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/gdvirtual.gen.inc>

namespace godot {

	class GFRegisterableEntity : public GFEntity {
		GDCLASS(GFRegisterableEntity, GFEntity)

	public:
		GFRegisterableEntity();
		GFRegisterableEntity(ecs_entity_t id, GFWorld* world):
			GFEntity(id, world) {}
		~GFRegisterableEntity();

		// --------------------------------------------------------
		// --- Exposed ---
		// --------------------------------------------------------

		GDVIRTUAL1(_register, GFWorld*)
		static Ref<GFRegisterableEntity> new_internal();

		// --------------------------------------------------------
		// --- Unexposed ---
		// --------------------------------------------------------

		void register_in_world(GFWorld* world);

	protected:
		void _register_internal(GFWorld*);
		static void _bind_methods();

	private:
	};

}

#endif
