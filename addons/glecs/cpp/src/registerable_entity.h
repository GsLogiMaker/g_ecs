
#ifndef REGISTERABLE_ENTITY_H
#define REGISTERABLE_ENTITY_H

#include "entity.h"

#include <flecs.h>
#include <godot_cpp/classes/ref_counted.hpp>

namespace godot {

	class GFRegisterableEntity : public GFEntity {
		GDCLASS(GFRegisterableEntity, GFEntity)

	public:
		GFRegisterableEntity();
		GFRegisterableEntity(ecs_entity_t id, GFWorld* world):
			GFEntity(id, world) {}
		~GFRegisterableEntity();

	protected:
		static void _bind_methods();

	private:
	};

}

#endif
