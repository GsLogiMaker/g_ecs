
#ifndef GL_ENTITY_H
#define GL_ENTITY_H

#include "world.h"

#include <flecs.h>
#include <godot_cpp/classes/ref_counted.hpp>

namespace godot {

	// Predefine instead of include to avoid cyclic dependencies
	class GFComponent;

	class GFEntity : public RefCounted {
		GDCLASS(GFEntity, RefCounted)

	public:
		GFEntity();
		GFEntity(ecs_entity_t id_, GFWorld* world_): id(id_), world(world_) {}
		GFEntity(GFEntity& ett): GFEntity(ett.get_id(), ett.get_world()) {}
		~GFEntity();

		// --------------------------------------
		// --- Exposed ---
		// --------------------------------------

		static Ref<GFEntity> spawn(GFWorld*);
		static Ref<GFEntity> from(Variant, GFWorld*);

		Ref<GFEntity> add_component(Variant);
		Ref<GFComponent> get_component(Variant);

		ecs_entity_t get_id();
		GFWorld* get_world();
		bool is_alive();

		Ref<GFEntity> set_name(String);
		String get_name();

		// --------------------------------------
		// --- Unexposed ---
		// --------------------------------------

		void set_id(ecs_entity_t);
		void set_world(GFWorld*);

	protected:
		static void _bind_methods();

	private:
		ecs_entity_t id;
		GFWorld* world;
	};

}

#endif
