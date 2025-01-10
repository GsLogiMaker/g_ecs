
#ifndef COMPONENT_H
#define COMPONENT_H

#include "component_builder.h"
#include "entity.h"
#include "godot_cpp/core/gdvirtual.gen.inc"
#include "registerable_entity.h"
#include "world.h"

#include <flecs.h>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot {

	class GFComponent : public GFRegisterableEntity {
		GDCLASS(GFComponent, GFRegisterableEntity)

	public:
		GFComponent(ecs_entity_t component, GFWorld* world):
			GFRegisterableEntity(component, world),
			source_entity_id(0)
		{}
		GFComponent(ecs_entity_t entity, ecs_entity_t component, GFWorld* world):
			source_entity_id(entity),
			GFRegisterableEntity(component, world)
		{}
		GFComponent():
			source_entity_id(0),
			GFRegisterableEntity(0, GFWorld::singleton())
		{}
		~GFComponent();

		// --------------------------------------
		// --- Exposed
		// --------------------------------------

		GDVIRTUAL1(_build, Ref<GFComponentBuilder>)

		static Ref<GFComponent> from(Variant c, Variant e, GFWorld*);
		static Ref<GFComponent> from_id(ecs_entity_t c, ecs_entity_t e, GFWorld*);
		static Ref<GFComponent> from_id_no_source(ecs_entity_t comp, GFWorld* world);

		Variant getm(String) const;
		void setm(String, Variant) const;

		Ref<GFEntity> get_source_entity() const;
		ecs_entity_t get_source_id() const;
		int get_data_size() const;
		int get_data_alignment() const;

		bool is_alive() const;
		String _to_string() const;

		// --------------------------------------
		// --- Unexposed
		// --------------------------------------

		void _register_internal();

		void build_data_from_variant(Variant, void* output) const;
		static void build_data_from_members(Array, void*, ecs_entity_t, GFWorld*);
		static const EcsComponent* get_component_ptr(GFWorld*, ecs_entity_t);
		static const EcsStruct* get_struct_ptr(GFWorld*, ecs_entity_t);
		void set_source_id(ecs_entity_t id);


	protected:
		static void _bind_methods();

		void* get_member_ptr_mut_at(int offset) const;
		const EcsMember* get_member_data(String) const;

		Variant member_value_as_primitive(void*, ecs_primitive_kind_t) const;
		Variant member_value_as_type(void*, ecs_entity_t) const;

	private:
		ecs_entity_t source_entity_id {0};
	};

}

#endif
