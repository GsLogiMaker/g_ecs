
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
		GFEntityBuilder() {
			world = GFWorld::singleton();
		}
		GFEntityBuilder(GFWorld* world): world(world) {}
		~GFEntityBuilder() {}

		// **************************************
		// *** Exposed ***
		// **************************************

		static Ref<GFEntityBuilder> new_in_world(GFWorld*);

		Ref<GFEntityBuilder> add_entity(Variant entity);
		Ref<GFEntityBuilder> add_pair(Variant first, Variant second);
		Ref<GFEntity> build();
		Ref<GFEntityBuilder> set_target_entity(Variant entity);
		Ref<GFEntityBuilder> set_name(String);
		Ref<GFEntityBuilder> set_parent(Variant entity);

		// **************************************
		// *** Unexposed ***
		// **************************************


	protected:
		PackedInt64Array ids = PackedInt64Array();
		ecs_entity_desc_t desc {
			.sep="/",
			.root_sep="/root/",
		};
		String name;

		static void _bind_methods();

	private:
		/// The world to query in.
		GFWorld* world{nullptr};

	};

}

#endif
