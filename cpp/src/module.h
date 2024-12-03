
#ifndef FECS_MODULE_H
#define FECS_MODULE_H

#include "entity.h"
#include "registerable_entity.h"
#include "world.h"

#include <flecs.h>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot {

	class GFModule : public GFRegisterableEntity {
		GDCLASS(GFModule, GFRegisterableEntity)

	public:
		/// Create a new named module
		GFModule(const char* name, GFWorld* world):
			GFRegisterableEntity(
				ecs_module_init(
					world->raw(),
					"",
					0
				),
				world
			)
		{}
		/// Create a new named module in default world
		GFModule(const char* name):
			GFRegisterableEntity(GFWorld::singleton())
		{}
		/// Create new unnamed module
		GFModule(GFWorld* world): GFModule("", world)
		{}
		/// Create new unnamed module in default world
		GFModule(): GFModule("")
		{}

		/// Reference an existing module
		GFModule(ecs_entity_t module_id, GFWorld* world):
			GFRegisterableEntity(module_id, world)
		{}

		~GFModule();

		// --------------------------------------
		// --- Exposed
		// --------------------------------------

		static Ref<GFModule> new_in_world(GFWorld* world);
		static Ref<GFModule> new_named_in_world(String name, GFWorld*);
		static Ref<GFModule> from(Variant module, GFWorld*);
		static Ref<GFModule> from_id(ecs_entity_t, GFWorld*);

		static Ref<GFRegisterableEntity> new_internal();

		// --------------------------------------
		// --- Unexposed
		// --------------------------------------

	protected:
		void _register_internal();
		void _register_user();
		static void _bind_methods();


	private:
		bool variant_is_registerable_script(Variant constant) {
			Ref<Script> const_script = constant;
			if (const_script == nullptr) {
				// Constant is not a script, skip it
				return false;
			}
			if (!ClassDB::is_parent_class(
				const_script->get_instance_base_type(),
				GFRegisterableEntity::get_class_static()
			)) {
				// Constant is not a registerable entity script, skip it
				return false;
			}
			return true;
		}

	};

}

#endif
