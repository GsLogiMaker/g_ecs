
#include "registerable_entity.h"
#include "godot_cpp/classes/wrapped.hpp"

#include <flecs.h>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

GFRegisterableEntity::GFRegisterableEntity() {
}
GFRegisterableEntity::~GFRegisterableEntity() {
}

// --------------------------------------------------------
// --- Exposed ---
// --------------------------------------------------------


// --------------------------------------------------------
// --- Unexposed ---
// --------------------------------------------------------

Ref<GFRegisterableEntity> GFRegisterableEntity::from_id(ecs_entity_t id, GFWorld* world_) {
	return from_id_template<GFRegisterableEntity>(id, world_);
}

Ref<GFRegisterableEntity> GFRegisterableEntity::from_script(Ref<Script> script, GFWorld* world) {
	Ref<GFRegisterableEntity> e = ClassDB::instantiate(
		script->get_instance_base_type()
	);
	e->set_id(world->get_registered_id(script));
	e->set_world(world);
	e->set_script(script);

	if (!e->is_alive()) {
		ERR(nullptr,
			"Could not instantiate", script->get_instance_base_type(), " from ID\n",
			"World/ID is not valid/alive"
		);
	}

	return e;
}

void GFRegisterableEntity::register_in_world(
	GFWorld* world
) {
	call_internal_register();
	call_user_register();
}

void GFRegisterableEntity::call_internal_register() {
	this->call("_register_internal");
}

void GFRegisterableEntity::call_user_register() {
	this->call("_register_user");
}

// --------------------------------------------------------
// --- Protected ---
// --------------------------------------------------------

void GFRegisterableEntity::_register_internal() {}
void GFRegisterableEntity::_register_user() {
	GDVIRTUAL_CALL(_register, get_world());
}

Ref<GFRegisterableEntity> GFRegisterableEntity::new_internal() {
	return Ref(memnew(GFRegisterableEntity));
}

void GFRegisterableEntity::_bind_methods() {
	GDVIRTUAL_BIND(_register, "world");
	godot::ClassDB::bind_method(D_METHOD("_register_internal"), &GFRegisterableEntity::_register_internal);
	godot::ClassDB::bind_method(D_METHOD("_register_user"), &GFRegisterableEntity::_register_user);
	godot::ClassDB::bind_static_method(get_class_static(), D_METHOD("_new_internal"), &GFRegisterableEntity::new_internal);
}
