
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
