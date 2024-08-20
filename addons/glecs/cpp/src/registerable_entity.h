
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
		virtual void _register_internal(GFWorld*);
		void test_func();

		// --------------------------------------------------------
		// --- Unexposed ---
		// --------------------------------------------------------

		void register_in_world(GFWorld* world, ecs_entity_t id);

	protected:
		static void _bind_methods();

	private:
	};

}

#endif
