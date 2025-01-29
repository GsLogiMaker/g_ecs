
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
		GFComponent(ecs_entity_t component, const GFWorld* world):
			GFRegisterableEntity(component, world),
			source_entity_id(0)
		{}
		GFComponent(ecs_entity_t entity, ecs_entity_t component, const GFWorld* world):
			source_entity_id(entity),
			GFRegisterableEntity(component, world)
		{}
		GFComponent():
			source_entity_id(0),
			GFRegisterableEntity(0, GFWorld::singleton())
		{}
		~GFComponent() {};

		// --------------------------------------
		// --- Exposed
		// --------------------------------------

		OVERRIDE_ENTITY_SELF_METHODS(GFComponent);

		GDVIRTUAL1(_build, Ref<GFComponentBuilder>)

		static Ref<GFComponent> from(const Variant c, const Variant e, GFWorld*);
		static Ref<GFComponent> from_id(ecs_entity_t c, ecs_entity_t e, GFWorld*);
		static Ref<GFComponent> from_id_no_source(ecs_entity_t comp, GFWorld* world);

		Variant getm(const String) const;
		void setm(const String, const Variant) const;

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

		void build_data_from_variant(const Variant, void* output) const;
		static void build_data_from_members(const Array, void*, ecs_entity_t, const GFWorld*);
		static const EcsComponent* get_component_ptr(const GFWorld*, ecs_entity_t);
		static const EcsStruct* get_struct_ptr(const GFWorld*, ecs_entity_t);
		void set_source_id(ecs_entity_t id);


	protected:
		static void _bind_methods();

		void* get_member_ptr_mut_at(int offset) const;
		const EcsMember* get_member_data(const String) const;

		Variant member_value_as_primitive(const void*, ecs_primitive_kind_t) const;
		Variant member_value_as_type(const void*, ecs_entity_t) const;

	private:
		ecs_entity_t source_entity_id {0};
	};

}

#endif
