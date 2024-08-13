

#include "querylike_builder.h"
#include "world.h"

#include <stdlib.h>
#include <flecs.h>

using namespace godot;

GlQuerylikeBuilder::GlQuerylikeBuilder() {
}

GlQuerylikeBuilder::~GlQuerylikeBuilder() {
}

// **************************************
// *** Exposed ***
// **************************************

int GlQuerylikeBuilder::get_term_count() {
	return term_count;
}

GlWorld* GlQuerylikeBuilder::get_world() {
	return world;
}

bool GlQuerylikeBuilder::is_built() {
	return built;
}

Ref<GlQuerylikeBuilder> GlQuerylikeBuilder::with(Variant component) {
	ecs_entity_t comp_id = world->coerce_id(component);

	query_desc.terms[get_term_count()].id = comp_id;
	query_desc.terms[get_term_count()].inout = ecs_inout_kind_t::EcsInOut;
	term_count += 1;

	return Ref(this);
}

// **************************************
// *** Unexposed ***
// **************************************

void GlQuerylikeBuilder::set_world(GlWorld* world_) {
	world = world_;
}

// **********************************************
// *** PROTECTED ***
// **********************************************

void GlQuerylikeBuilder::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("is_built"), &GlQuerylikeBuilder::is_built);
	godot::ClassDB::bind_method(D_METHOD("with", "component"), &GlQuerylikeBuilder::with);

}

// **********************************************
// *** PRIVATE ***
// **********************************************
