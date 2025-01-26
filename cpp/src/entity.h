
#ifndef GL_ENTITY_H
#define GL_ENTITY_H

#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "utils.h"
#include "world.h"

#include <cassert>
#include <cstdint>
#include <flecs.h>
#include <godot_cpp/classes/ref_counted.hpp>

#define OVERRIDE_ENTITY_SELF_METHODS(Self)	\
	Ref<Self> add_child(Variant v0) { return GFEntity::add_child(v0); } \
	Ref<Self> add_componentv(Variant v0, Variant v1) { return GFEntity::add_componentv(v0, v1); } \
	Ref<Self> set_componentv(Variant v0, Variant v1) { return GFEntity::set_componentv(v0, v1); } \
	Ref<Self> set_pairv(Variant v0, Variant v1, Variant v2) { return GFEntity::set_pairv(v0, v1, v2); } \
	Ref<Self> add_pairv(Variant v0, Variant v1, Variant v2) { return GFEntity::add_pairv(v0, v1, v2); } \
	Ref<Self> add_tag(Variant v0) { return GFEntity::add_tag(v0); } \
	Ref<Self> emit(Variant v0, Array v1, Array v2) { return GFEntity::emit(v0, v1, v2); } \
	Ref<Self> set_name(String v0) { return GFEntity::set_name(v0); } \
	Ref<Self> set_parent(Variant v0) { return GFEntity::set_parent(v0); }	\
	Ref<Self> add_component(const Variant** v0, GDExtensionInt v1, GDExtensionCallError& v2) { return GFEntity::add_component(v0, v1, v2); } \
	Ref<Self> set_component(const Variant** v0, GDExtensionInt v1, GDExtensionCallError& v2) { return GFEntity::set_component(v0, v1, v2); } \
	Ref<Self> add_pair(const Variant** v0, GDExtensionInt v1, GDExtensionCallError& v2) { return GFEntity::add_pair(v0, v1, v2); } \
	Ref<Self> set_pair(const Variant** v0, GDExtensionInt v1, GDExtensionCallError& v2) { return GFEntity::set_pair(v0, v1, v2); } \
;

#define REGISTER_ENTITY_SELF_METHODS(Self) {	\
	godot::ClassDB::bind_method(D_METHOD("add_child", "entity"), &Self::add_child);	\
	godot::ClassDB::bind_method(D_METHOD("addv", "component", "members"), &Self::add_componentv);	\
	godot::ClassDB::bind_method(D_METHOD("setv", "component", "members"), &Self::set_componentv);	\
	godot::ClassDB::bind_method(D_METHOD("add_pairv", "first", "second", "members"), &Self::add_pairv);	\
	godot::ClassDB::bind_method(D_METHOD("set_pairv", "first", "second", "members"), &Self::set_pairv);	\
	godot::ClassDB::bind_method(D_METHOD("add_tag", "tag"), &Self::add_tag);	\
	godot::ClassDB::bind_method(D_METHOD("emit", "entity", "components", "event_members"), &Self::emit);	\
	godot::ClassDB::bind_method(D_METHOD("set_name", "name"), &Self::set_name);	\
	godot::ClassDB::bind_method(D_METHOD("set_parent", "entity"), &Self::set_parent);	\
	{	\
		MethodInfo mi;	\
		mi.arguments.push_back(PropertyInfo(Variant::NIL, "component"));	\
		mi.name = "add";	\
		mi.flags = METHOD_FLAGS_DEFAULT;	\
		godot::ClassDB::bind_vararg_method(METHOD_FLAGS_DEFAULT, StringName(mi.name), &Self::add_component, mi);	\
	}	\
	{	\
		MethodInfo mi;	\
		mi.arguments.push_back(PropertyInfo(Variant::NIL, "component"));	\
		mi.name = "set";	\
		godot::ClassDB::bind_vararg_method(METHOD_FLAGS_DEFAULT, StringName(mi.name), &Self::set_component, mi);	\
	}	\
	{	\
		MethodInfo mi;	\
		mi.arguments.push_back(PropertyInfo(Variant::NIL, "first"));	\
		mi.arguments.push_back(PropertyInfo(Variant::NIL, "second"));	\
		mi.name = "add_pair";	\
		godot::ClassDB::bind_vararg_method(METHOD_FLAGS_DEFAULT, StringName(mi.name), &Self::add_pair, mi);	\
	}	\
	{	\
		MethodInfo mi;	\
		mi.arguments.push_back(PropertyInfo(Variant::NIL, "first"));	\
		mi.arguments.push_back(PropertyInfo(Variant::NIL, "second"));	\
		mi.name = "set_pair";	\
		godot::ClassDB::bind_vararg_method(METHOD_FLAGS_DEFAULT, StringName(mi.name), &Self::set_pair, mi);	\
	}	\
}

namespace godot {

	// Predefine instead of include to avoid cyclic dependencies
	class GFComponent;
	class GFPair;

	class GFEntity : public RefCounted {
		GDCLASS(GFEntity, RefCounted)

	public:
		// New entity in global world
		GFEntity();
		// New entity in specific world
		GFEntity(const GFWorld* world) ;
		// Reference an entity
		GFEntity(const ecs_entity_t id_, const GFWorld* world_):
			id(id_)
		{
			world_instance_id = world_->get_instance_id();
		}
		// Copy an entity reference
		GFEntity(GFEntity& ett): GFEntity(ett.get_id(), ett.get_world()) {}
		~GFEntity();

		// --------------------------------------
		// --- Exposed ---
		// --------------------------------------

		static Ref<GFEntity> new_in_world(const GFWorld*);
		static Ref<GFEntity> from(const Variant, GFWorld*);
		static Ref<GFEntity> from_id(const ecs_entity_t, const GFWorld*);

		Ref<GFEntity> add_child(Variant entity);
		Ref<GFEntity> add_component(const Variant**, GDExtensionInt, GDExtensionCallError&);
		Ref<GFEntity> add_componentv(Variant, Array);
		Ref<GFEntity> set_component(const Variant**, GDExtensionInt, GDExtensionCallError&);
		Ref<GFEntity> set_componentv(Variant, Array);

		Ref<GFEntity> add_pair(const Variant**, GDExtensionInt, GDExtensionCallError&);
		Ref<GFEntity> add_pairv(Variant, Variant, Array);
		Ref<GFEntity> set_pair(const Variant**, GDExtensionInt, GDExtensionCallError&);
		Ref<GFEntity> set_pairv(Variant, Variant, Array);

		Ref<GFEntity> add_tag(Variant);
		Ref<GFEntity> emit(Variant, Array, Array);
		Ref<GFEntity> set_name(String);
		Ref<GFEntity> set_parent(Variant entity);

		void delete_() const;

		Ref<GFEntity> get_child(String) const;
		Ref<GFComponent> get_component(Variant) const;
		ecs_entity_t get_id() const;
		String get_name() const;
		Ref<GFComponent> get_pair(Variant, Variant) const;
		Ref<GFEntity> get_parent() const;
		String get_path() const;
		GFWorld* get_world() const;

		bool has_entity(Variant) const;
		bool has_pair(Variant, Variant) const;
		bool has_child(String) const;

		bool is_alive() const;
		bool is_pair() const;

		Ref<GFPair> pair(Variant second) const;
		ecs_entity_t pair_id(ecs_entity_t second_id) const;

		String _to_string() const;

		// --------------------------------------
		// --- Unexposed ---
		// --------------------------------------

		/// Assigns the correct script to this entity according to its ID.
		void update_script() {
			Ref<Script> script = get_world()->get_registered_script(get_id());
			if (script == nullptr) {
				return;
			}
			if (!ClassDB::is_parent_class(
				get_class(),
				script->get_instance_base_type()
			)) {
				return;
			}
			set_script(script);
		}

		template<typename T>
		static Ref<T> setup_template(Ref<T> e) {
			e->update_script();

			if (!e->is_alive()) {
				ERR(nullptr,
					"Could not instantiate ", T::get_class_static(), " from ID\n",
					"ID ", e->to_string(), " is not valid in world ", e->get_world()
				);
			}

			return e;
		}


		void set_id(ecs_entity_t);
		void set_world(const GFWorld*);

	protected:
		static void _bind_methods();

	private:
		ecs_entity_t id {0};
		uint64_t world_instance_id {0};
	};

}

#endif
