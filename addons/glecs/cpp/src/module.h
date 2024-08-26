
#ifndef FECS_MODULE_H
#define FECS_MODULE_H

#include "entity.h"
#include "registerable_entity.h"

#include <flecs.h>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot {

	class GFModule : public GFRegisterableEntity {
		GDCLASS(GFModule, GFRegisterableEntity)

	public:
		GFModule();
		GFModule(ecs_entity_t pair_id, GFWorld* world):
			GFRegisterableEntity(pair_id, world)
			{}
		~GFModule();

		// --------------------------------------
		// --- Exposed
		// --------------------------------------

		static Ref<GFModule> spawn(String name, GFWorld*);
		static Ref<GFModule> from(Variant module, GFWorld*);
		static Ref<GFModule> from_id(ecs_entity_t, GFWorld*);

		static Ref<GFRegisterableEntity> new_internal();

		// --------------------------------------
		// --- Unexposed
		// --------------------------------------

	protected:
		void _register_internal(GFWorld*);
		static void _bind_methods();


	private:
	};

}

#endif
