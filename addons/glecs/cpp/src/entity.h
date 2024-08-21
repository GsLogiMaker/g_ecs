
#ifndef GL_ENTITY_H
#define GL_ENTITY_H

#include "godot_cpp/variant/variant.hpp"
#include "world.h"

#include <cassert>
#include <flecs.h>
#include <godot_cpp/classes/ref_counted.hpp>

namespace godot {

	// Predefine instead of include to avoid cyclic dependencies
	class GFComponent;
	class GFPair;

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

		static Ref<GFEntity> new_(GFWorld*);
		static Ref<GFEntity> from(Variant, GFWorld*);
		static Ref<GFEntity> from_id(ecs_entity_t, GFWorld*);

		Ref<GFEntity> add_component(Variant);
		Ref<GFComponent> get_component(Variant);

		ecs_entity_t get_id();
		GFWorld* get_world();

		bool is_alive();
		bool is_pair();

		Ref<GFEntity> set_name(String);
		String get_name();

		Ref<GFPair> pair(Variant second);
		ecs_entity_t pair_id(ecs_entity_t second_id);

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
