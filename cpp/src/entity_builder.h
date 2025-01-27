
#ifndef ENTITY_BUILDER_H
#define ENTITY_BUILDER_H

#include "godot_cpp/variant/callable.hpp"
#include "godot_cpp/variant/packed_int64_array.hpp"
#include "world.h"

#include <flecs.h>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot {

	class GFEntityBuilder : public RefCounted {
		GDCLASS(GFEntityBuilder, RefCounted)

	public:
		GFEntityBuilder(): GFEntityBuilder(GFWorld::singleton()) {}
		GFEntityBuilder(GFWorld* world):
			world(world),
			ids(PackedInt64Array()),
			desc({
				.sep="/",
				.root_sep="/root/",
			}),
			name(String("")),
			built_count(0)
		{}
		~GFEntityBuilder() {}

		// **************************************
		// *** Exposed ***
		// **************************************

		static Ref<GFEntityBuilder> new_in_world(GFWorld*);

		Ref<GFEntityBuilder> add_entity(Variant entity);
		Ref<GFEntityBuilder> add_pair(Variant first, Variant second);
		Ref<GFEntity> build();
		ecs_entity_t build_id();
		GFWorld* get_world();
		Ref<GFEntityBuilder> set_target_entity(Variant entity);
		Ref<GFEntityBuilder> set_name(String);
		Ref<GFEntityBuilder> set_parent(Variant entity);

		// **************************************
		// *** Unexposed ***
		// **************************************

		void set_world(GFWorld*);


	protected:
		ecs_entity_desc_t desc;
		/// Owned copy of the IDs to add to the entity.
		PackedInt64Array ids;
		/// Owned copy of the entity's name.
		String name;
		/// The world to query in.
		GFWorld* world;
		int built_count;

		static void _bind_methods();

	private:
	};

}

#endif
