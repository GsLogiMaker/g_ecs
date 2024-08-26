
#ifndef COMPONENT_H
#define COMPONENT_H

#include "component_builder.h"
#include "entity.h"
#include "godot_cpp/core/gdvirtual.gen.inc"
#include "registerable_entity.h"

#include <flecs.h>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot {

	class GFComponent : public GFRegisterableEntity {
		GDCLASS(GFComponent, GFRegisterableEntity)

	public:
		GFComponent();
		GFComponent(ecs_entity_t entity, ecs_entity_t component, GFWorld* world):
			source_entity_id(entity),
			GFRegisterableEntity(component, world) {}
		~GFComponent();

		// --------------------------------------
		// --- Exposed
		// --------------------------------------

		GDVIRTUAL1(_build, Ref<GFComponentBuilder>)

		Variant getm(String);
		void setm(String, Variant);

		Ref<GFEntity> get_source_entity();
		ecs_entity_t get_source_id();

		// --------------------------------------
		// --- Unexposed
		// --------------------------------------

		void _register_internal(GFWorld* world);

		void set_source_id(ecs_entity_t id);

		static Ref<GFRegisterableEntity> new_internal();

	protected:
		static void _bind_methods();

		void* get_member_ptr_mut_at(int offset);
		const EcsMember* get_member_data(String);

		Variant member_value_as_primitive(void*, ecs_primitive_kind_t);
		Variant member_value_as_type(void*, ecs_entity_t);
		void set_member_value_as_primitive(void*, Variant, ecs_primitive_kind_t);
		void set_member_value_as_type(void*, Variant, ecs_entity_t);


	private:
		ecs_entity_t source_entity_id {0};
	};

}

#endif
