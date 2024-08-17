
#ifndef COMPONENT_Builder_H
#define COMPONENT_Builder_H

#include <flecs.h>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot {

	// Predefine instead of include to avoid cyclic dependencies
	class GFWorld;

	class GFComponentBuilder : public RefCounted {
		GDCLASS(GFComponentBuilder, RefCounted)

	public:
		GFComponentBuilder();
		~GFComponentBuilder();

		// **************************************
		// *** Exposed ***
		// **************************************

		Ref<GFComponentBuilder> add_member(String, Variant::Type);
		int get_member_count();
		Ref<GFComponentBuilder> set_name(String);
		void build();

		// **************************************
		// *** Unexposed ***
		// **************************************

		void set_world(GFWorld*);

	protected:
		static void _bind_methods();

	private:
		ecs_component_desc_t component_desc;
		ecs_struct_desc_t struct_desc;
		/// @brief A list of the allocated names of members
		/// Also used to find the number of added of members
		Array member_names;
		/// The name of this compoennt
		String name;
		GFWorld* world;
		/// Is true if this builder has already been built
		bool built;

		static void ctor(void*, int32_t, const ecs_type_info_t*);
		static void dtor(void*, int32_t, const ecs_type_info_t*);
		static void copy(void*, const void*, int32_t, const ecs_type_info_t*);
		static void move(void*, void*, int32_t, const ecs_type_info_t*);

	};

	class HooksBindingContext {
	public:
		GFWorld* world;

		HooksBindingContext(GFWorld* world);
		~HooksBindingContext();
	};

}

#endif
