
#ifndef ENTITY_BUILDER_H
#define ENTITY_BUILDER_H

#include "godot_cpp/variant/callable.hpp"
#include "godot_cpp/variant/packed_int64_array.hpp"
#include "world.h"

#include <flecs.h>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>

#define OVERRIDE_ENTITY_BUILDER_SELF_METHODS(Self)	\
	Ref<Self> add_entity(Variant v0)          	{ return GFEntityBuilder::add_entity(v0); }       	\
	Ref<Self> add_pair(Variant v0, Variant v1)	{ return GFEntityBuilder::add_pair(v0, v1); }     	\
	Ref<Self> set_target_entity(Variant v0)   	{ return GFEntityBuilder::set_target_entity(v0); }	\
	Ref<Self> set_name(Variant v0)            	{ return GFEntityBuilder::set_name(v0); }         	\
	Ref<Self> set_parent(Variant v0)          	{ return GFEntityBuilder::set_parent(v0); }       	\
;

#define REGISTER_ENTITY_BUILDER_SELF_METHODS(Self)	\
	godot::ClassDB::bind_method(D_METHOD("add_entity", "entity"),       	&Self::add_entity);       	\
	godot::ClassDB::bind_method(D_METHOD("add_pair", "first", "second"),	&Self::add_pair);         	\
	godot::ClassDB::bind_method(D_METHOD("set_target_entity", "entity"),	&Self::set_target_entity);	\
	godot::ClassDB::bind_method(D_METHOD("set_name", "name"),           	&Self::set_name);         	\
	godot::ClassDB::bind_method(D_METHOD("set_parent", "entity"),       	&Self::set_parent, 0);    	\
;

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
		Ref<GFEntityBuilder> set_target_entity(Variant entity);
		Ref<GFEntityBuilder> set_name(String);
		Ref<GFEntityBuilder> set_parent(Variant entity);

		Ref<GFEntity> build();
		ecs_entity_t build_id();
		GFWorld* get_world();

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
