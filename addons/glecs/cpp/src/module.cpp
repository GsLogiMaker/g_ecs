
#include "module.h"
#include "godot_cpp/classes/ref_counted.hpp"
#include "registerable_entity.h"
#include "utils.h"
#include <stdint.h>
#include <flecs.h>
#include "godot_cpp/variant/char_string.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/array.hpp"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

GFModule::GFModule() {
}
GFModule::~GFModule() {
}

Ref<GFModule> GFModule::spawn(String name, GFWorld* world_) {
	CharString name_utf8 = name.utf8();
	GFWorld* world = world_;
	if (world == nullptr) {
		// world = GlWorld::singleton();
	}
	ecs_component_desc_t comp_desc = {0};
	ecs_entity_t module_id = ecs_module_init(
		world->raw(),
		name_utf8,
		&comp_desc
	);
	return from_id(module_id, world);
}

Ref<GFModule> GFModule::from(Variant module, GFWorld* world) {
	ecs_entity_t module_id = world->coerce_id(module);
	return from_id(module_id, world);
}

Ref<GFModule> GFModule::from_id(ecs_entity_t module_id, GFWorld* world_) {
	GFWorld* world = world_;
	if (world == nullptr) {
		// world = GlWorld::singleton();
	}
	if (!ecs_has_id(world->raw(), module_id, EcsModule)) {
		ERR(nullptr,
			"Could not instantiate module from ID\n",
			"ID is not a module"
		);
	}
	Ref<GFModule> ett = memnew(GFModule(
		module_id,
		world
	));
	if (!ett->is_alive()) {
		ERR(nullptr,
			"Could not instantiate module from ID\n",
			"World/ID is not valid/alive"
		);
	}
	return ett;
}

void GFModule::_register_internal(GFWorld* world) {
	// Setup module with Flecs
	ecs_component_desc_t desc = {
		.entity = get_id(),
	};
	ecs_module_init(
		world->raw(),
		"TODOSomeName",
		&desc
	);

	// Register module's items
	Ref<Script> script = get_script();
	Dictionary constants = script->get_script_constant_map();
	Array keys = constants.keys();
	for (int i=0; i != keys.size(); i++) {
		Variant const_val = constants[keys[i]];

		Ref<Script> const_script = const_val;
		if (const_script == nullptr) {
			// Constant is not a script, skip it
			continue;
		}

		if (!ClassDB::is_parent_class(
			const_script->get_instance_base_type(),
			GFRegisterableEntity::get_class_static()
		)) {
			// Constant is not a registerable entity script, skip it
			continue;
		}

		// Register entity
		ecs_entity_t old_scope = ecs_get_scope(get_world()->raw());
		ecs_set_scope(get_world()->raw(), get_id());
		ecs_entity_t e_id = world->register_script_id(const_script);
		ecs_set_scope(get_world()->raw(), old_scope);

		if (ecs_get_name(get_world()->raw(), e_id) == nullptr) {
			// Entity has no name, give it the name of the constant's key
			String name = keys[i];
			ecs_set_name(get_world()->raw(), e_id, name.utf8());
		}
	}

	GFRegisterableEntity::_register_internal(world);
}

Ref<GFRegisterableEntity> GFModule::new_internal() {
	return Ref(memnew(GFModule));
}

void GFModule::_bind_methods() {
	godot::ClassDB::bind_static_method(get_class_static(), D_METHOD("spawn", "name", "world"), &GFModule::spawn, nullptr);
	godot::ClassDB::bind_static_method(get_class_static(), D_METHOD("from", "module", "world"), &GFModule::from, nullptr);
	godot::ClassDB::bind_static_method(get_class_static(), D_METHOD("from_id", "module_id", "world"), &GFModule::from_id, nullptr);
	godot::ClassDB::bind_method(D_METHOD("_register_internal", "world"), &GFModule::_register_internal);
	godot::ClassDB::bind_static_method(get_class_static(), D_METHOD("_new_internal"), GFModule::new_internal);
}
