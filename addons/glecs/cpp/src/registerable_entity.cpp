
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
	GFWorld* world
) {
	call("_register_internal", world);
	GDVIRTUAL_CALL(_register, world);
}

// --------------------------------------------------------
// --- Protected ---
// --------------------------------------------------------

void GFRegisterableEntity::_register_internal(GFWorld*) {}

Ref<GFRegisterableEntity> GFRegisterableEntity::new_internal() {
	return Ref(memnew(GFRegisterableEntity));
}

void GFRegisterableEntity::_bind_methods() {
	GDVIRTUAL_BIND(_register, "world");
	godot::ClassDB::bind_method(D_METHOD("_register_internal", "world"), &GFRegisterableEntity::_register_internal);
	godot::ClassDB::bind_static_method(get_class_static(), D_METHOD("_new_internal"), &GFRegisterableEntity::new_internal);
}
