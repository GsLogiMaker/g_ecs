
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
		GFRegisterableEntity(): GFEntity() {}
		GFRegisterableEntity(GFWorld* world): GFEntity(world) {}
		GFRegisterableEntity(ecs_entity_t id, GFWorld* world):
			GFEntity(id, world) {}
		~GFRegisterableEntity();

		// --------------------------------------------------------
		// --- Exposed ---
		// --------------------------------------------------------

		GDVIRTUAL1(_register, GFWorld*)
		static Ref<GFRegisterableEntity> new_internal();

		static Ref<GFRegisterableEntity> new_in_world(GFWorld*);
		static Ref<GFRegisterableEntity> from_id(ecs_entity_t id, GFWorld* world);
		static Ref<GFRegisterableEntity> from_script(Ref<Script>, GFWorld*);

		// --------------------------------------------------------
		// --- Unexposed ---
		// --------------------------------------------------------

		void register_in_world(GFWorld* world);
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
