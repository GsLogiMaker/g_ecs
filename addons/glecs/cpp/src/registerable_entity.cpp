
#include "registerable_entity.h"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/utility_functions.hpp"

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

void GFRegisterableEntity::register_in_world(
	GFWorld* world,
	ecs_entity_t with_id
) {
	set_id(with_id);
	_register_internal(world);
}

void GFRegisterableEntity::_register_internal(
	GFWorld* world
) {
	GDVIRTUAL_CALL(_register, world);
}

void GFRegisterableEntity::test_func() {}

// --------------------------------------------------------
// --- Protected ---
// --------------------------------------------------------

void GFRegisterableEntity::_bind_methods() {
	GDVIRTUAL_BIND(_register, "world");
}
