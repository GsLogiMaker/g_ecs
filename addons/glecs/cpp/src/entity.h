
#ifndef GL_ENTITY_H
#define GL_ENTITY_H

#include "world.h"

#include <flecs.h>
#include <godot_cpp/classes/ref_counted.hpp>

namespace godot {

	// Predefine instead of include to avoid cyclic dependencies
	class GlComponent;

	class GlEntity : public RefCounted {
		GDCLASS(GlEntity, RefCounted)

	public:
		GlEntity();
		GlEntity(ecs_entity_t id_, GlWorld* world_): id(id_), world(world_) {}
		GlEntity(GlEntity& ett): GlEntity(ett.get_id(), ett.get_world()) {}
		~GlEntity();

		// --------------------------------------
		// --- Exposed ---
		// --------------------------------------

		static Ref<GlEntity> spawn(GlWorld*);
		static Ref<GlEntity> from(Variant, GlWorld*);

		Ref<GlEntity> add_component(Variant);
		Ref<GlComponent> get_component(Variant);

		bool is_alive();

		ecs_entity_t get_id();
		GlWorld* get_world();

		// --------------------------------------
		// --- Unexposed ---
		// --------------------------------------

		void set_id(ecs_entity_t);
		void set_world(GlWorld*);

	protected:
		static void _bind_methods();

	private:
		ecs_entity_t id;
		GlWorld* world;
	};

}

#endif
