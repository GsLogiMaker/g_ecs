
#include "registerable_entity.h"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "world.h"

#include <flecs.h>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

GFRegisterableEntity::~GFRegisterableEntity() {
}

// --------------------------------------------------------
// --- Exposed ---
// --------------------------------------------------------

Ref<GFRegisterableEntity> GFRegisterableEntity::new_in_world(
	GFWorld* world
) {
	ERR(nullptr,
		"Couldn't instantiate entity\n",
		get_class_static(), " cannot be instantiated. Use \"from\" instead"
	);
}

Ref<GFRegisterableEntity> GFRegisterableEntity::from_id(
	ecs_entity_t id,
	GFWorld* world_
) {
	return setup_template<GFRegisterableEntity>(memnew(
		GFRegisterableEntity(id, world_))
	);
}

Ref<GFRegisterableEntity> GFRegisterableEntity::from_script(
	const Ref<Script> script,
	GFWorld* world
) {
	UtilityFunctions::prints("FROM SCJI", script->get_path());
	Ref<GFRegisterableEntity> e = ClassDB::instantiate(
		script->get_instance_base_type()
	);
	e->set_id(world->get_registered_id(script));
	e->set_world(world);
	e->set_script(script);

	if (!e->is_alive()) {
		ERR(nullptr,
			"Could not instantiate",
			script->get_instance_base_type(),
			" from ID\n",
			"World/ID is not valid/alive"
		);
	}

	return e;
}

// --------------------------------------------------------
// --- Unexposed ---
// --------------------------------------------------------

void GFRegisterableEntity::register_in_world() {
	call_internal_register();
	call_user_register();
}

void GFRegisterableEntity::call_internal_register() {
	this->call("_register_internal");
}

void GFRegisterableEntity::call_user_register() {
	ecs_entity_t old_scope = ecs_get_scope(get_world()->raw());
	ecs_set_scope(get_world()->raw(), get_id());
	this->call("_register_user");
	ecs_set_scope(get_world()->raw(), old_scope);
}

// --------------------------------------------------------
// --- Protected ---
// --------------------------------------------------------

void GFRegisterableEntity::_register_internal() {}
void GFRegisterableEntity::_register_user() {
	GDVIRTUAL_CALL(_register, get_world());
}

void GFRegisterableEntity::_bind_methods() {
	REGISTER_ENTITY_SELF_METHODS(GFRegisterableEntity);

	GDVIRTUAL_BIND(_register, "world");
	godot::ClassDB::bind_static_method(GFRegisterableEntity::get_class_static(), D_METHOD("new_in_world", "world"), &GFRegisterableEntity::new_in_world);
	godot::ClassDB::bind_method(D_METHOD("_register_internal"), &GFRegisterableEntity::_register_internal);
	godot::ClassDB::bind_method(D_METHOD("_register_user"), &GFRegisterableEntity::_register_user);
}
