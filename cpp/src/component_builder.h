
#ifndef COMPONENT_Builder_H
#define COMPONENT_Builder_H

#include "entity_builder.h"
#include "gdextension_interface.h"
#include "world.h"
#include <flecs.h>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot {

	// Predefine instead of include to avoid cyclic dependencies
	class GFWorld;

	class GFComponentBuilder : public GFEntityBuilder {
		GDCLASS(GFComponentBuilder, GFEntityBuilder)

	public:
		GFComponentBuilder(const GFWorld* world):
			GFEntityBuilder(world),
			component_desc({0}),
			struct_desc({0}),
			member_names(Array())
		{}
		GFComponentBuilder():
			GFEntityBuilder(GFWorld::singleton())
		{}
		~GFComponentBuilder();

		// **************************************
		// *** Exposed ***
		// **************************************

		OVERRIDE_ENTITY_BUILDER_SELF_METHODS(GFComponentBuilder);

		Ref<GFComponentBuilder> add_member(const String, Variant::Type);
		int get_member_count() const;
		bool is_built() const;

		// Overriding
		static Ref<GFComponentBuilder> new_in_world(const GFWorld*);
		Ref<GFEntity> build();

	protected:
		static void _bind_methods();

	private:
		ecs_component_desc_t component_desc {0};
		ecs_struct_desc_t struct_desc {0};
		/// @brief A list of the allocated names of members
		/// Also used to find the number of added of members
		Array member_names;

		static void ctor(void*, int32_t, const ecs_type_info_t*);
		static void dtor(void*, int32_t, const ecs_type_info_t*);
		static void copy(void*, const void*, int32_t, const ecs_type_info_t*);
		static void move(void*, void*, int32_t, const ecs_type_info_t*);

	};

	class HooksBindingContext {
	public:
		GDObjectInstanceID world_instance_id;

		HooksBindingContext(const GFWorld* world):
			world_instance_id(world->get_instance_id()
		) {};
		~HooksBindingContext() {};

		GFWorld* get_world() const;
	};

}

#endif
