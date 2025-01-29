
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
		GFRegisterableEntity():
			GFRegisterableEntity(GFWorld::singleton())
		{}
		GFRegisterableEntity(const GFWorld* world):
			GFEntity(world)
		{}
		GFRegisterableEntity(ecs_entity_t id, const GFWorld* world):
			GFEntity(id, world)
		{}
		~GFRegisterableEntity();

		// --------------------------------------------------------
		// --- Exposed ---
		// --------------------------------------------------------

		OVERRIDE_ENTITY_SELF_METHODS(GFRegisterableEntity);

		GDVIRTUAL1(_register, GFWorld*)

		static Ref<GFRegisterableEntity> new_in_world(const GFWorld*);
		static Ref<GFRegisterableEntity> from_id(ecs_entity_t id, const GFWorld* world);
		static Ref<GFRegisterableEntity> from_script(const Ref<Script>, const GFWorld*);

		// --------------------------------------------------------
		// --- Unexposed ---
		// --------------------------------------------------------

		void register_in_world();
		// Runs internal register setup code
		void call_internal_register();
		// Runs user register setup code
		void call_user_register();

	protected:
		void _register_internal();
		void _register_user();
		static void _bind_methods();

	private:
	};

}

#endif
