
#ifndef GL_ENTITY_H
#define GL_ENTITY_H

#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "utils.h"
#include "world.h"

#include <cassert>
#include <cstdint>
#include <flecs.h>
#include <godot_cpp/classes/ref_counted.hpp>

namespace godot {

	// Predefine instead of include to avoid cyclic dependencies
	class GFComponent;
	class GFPair;

	class GFEntity : public RefCounted {
		GDCLASS(GFEntity, RefCounted)

	public:
		// New entity in global world
		GFEntity();
		// New entity in specific world
		GFEntity(GFWorld* world) ;
		// Reference an entity
		GFEntity(ecs_entity_t id_, GFWorld* world_):
			id(id_),
			world_instance_id(world_->get_instance_id())
		{}
		// Copy an entity reference
		GFEntity(GFEntity& ett): GFEntity(ett.get_id(), ett.get_world()) {}
		~GFEntity();

		// --------------------------------------
		// --- Exposed ---
		// --------------------------------------

		static Ref<GFEntity> new_in_world(GFWorld*);
		static Ref<GFEntity> from(Variant, GFWorld*);
		static Ref<GFEntity> from_id(ecs_entity_t, GFWorld*);

		Ref<GFEntity> add_child(Variant entity);
		Ref<GFEntity> add_component(const Variant**, GDExtensionInt, GDExtensionCallError&);
		Ref<GFEntity> _add_component(Variant, Array);
		Ref<GFEntity> set_component(const Variant**, GDExtensionInt, GDExtensionCallError&);
		Ref<GFEntity> _set_component(Variant, Array);

		Ref<GFEntity> add_pair(const Variant**, GDExtensionInt, GDExtensionCallError&);
		Ref<GFEntity> _add_pair(Variant, Variant, Array);
		Ref<GFEntity> set_pair(const Variant**, GDExtensionInt, GDExtensionCallError&);
		Ref<GFEntity> _set_pair(Variant, Variant, Array);

		Ref<GFEntity> add_tag(Variant);

		Ref<GFEntity> emit(Variant, Array, Array);

		Ref<GFComponent> get_component(Variant);
		Ref<GFComponent> get_pair(Variant, Variant);

		void delete_();

		ecs_entity_t get_id();
		String get_path();
		GFWorld* get_world();

		bool is_alive();
		bool is_pair();

		Ref<GFEntity> set_name(String);
		String get_name();

		Ref<GFPair> pair(Variant second);
		ecs_entity_t pair_id(ecs_entity_t second_id);

		String to_string();

		// --------------------------------------
		// --- Unexposed ---
		// --------------------------------------

		/// Assigns the correct script to this entity according to its ID.
		void update_script() {
			Ref<Script> script = get_world()->get_registered_script(get_id());
			if (script == nullptr) {
				return;
			}
			if (!ClassDB::is_parent_class(
				get_class(),
				script->get_instance_base_type()
			)) {
				return;
			}
			set_script(script);
		}

		template<typename T>
		static Ref<T> setup_template(Ref<T> e) {
			e->update_script();

			if (!e->is_alive()) {
				ERR(nullptr,
					"Could not instantiate ", T::get_class_static(), " from ID\n",
					"ID ", e->to_string(), " is not valid in world ", e->get_world()
				);
			}

			return e;
		}


		void set_id(ecs_entity_t);
		void set_world(GFWorld*);

	protected:
		static void _bind_methods();

	private:
		ecs_entity_t id {0};
		uint64_t world_instance_id {0};
	};

}

#endif
