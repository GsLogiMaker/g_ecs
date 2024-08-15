

#include "system_builder.h"
#include "component.h"
#include "godot_cpp/classes/object.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "world.h"
#include "utils.h"

#include <stdlib.h>
#include <flecs.h>

using namespace godot;

GlSystemBuilder::GlSystemBuilder() {
}

GlSystemBuilder::~GlSystemBuilder() {
}

void GlSystemBuilder::for_each(Callable callable) {
	const char* FAILED_TO_BUILD = "Failed to build system\n";
	if (is_built()) {
		ERR(/**/,
			FAILED_TO_BUILD,
			"System builder was already built"
		);
	}
	built = true;

	SystemContext* ctx = new SystemContext(get_world(), callable);

	for (int i=0; i != get_term_count(); i++) {
		switch (query_desc.terms[i].oper) {
			case ecs_oper_kind_t::EcsAnd:
			case ecs_oper_kind_t::EcsOr:
			case ecs_oper_kind_t::EcsOptional:
				ctx->comp_ref_per_term.append(Variant(Ref(memnew(
					GlComponent(0, query_desc.terms[i].id, get_world())
				))));
				ctx->comp_ref_args.append(Variant());
				break;
			case ecs_oper_kind_t::EcsNot:
				ctx->comp_ref_per_term.append(Variant(nullptr));
				break;
		}
	}

	ecs_entity_t sys_id = ecs_new(get_world()->raw());
	ecs_add_id(get_world()->raw(), sys_id, ecs_dependson(EcsOnUpdate));
	ecs_system_desc_t desc = {
		.entity = sys_id,
		.query = query_desc,
		.callback = GlSystemBuilder::callback,
		.callback_ctx = ctx,
		.callback_ctx_free = [](void* ptr) {delete (SystemContext*) ptr;}
	};
	ecs_system_init(get_world()->raw(), &desc);

}

// **********************************************
// *** PROTECTED ***
// **********************************************

void update_component_terms(ecs_iter_t* it, SystemContext* ctx) {
	int i_arg = 0;
	for (int term_i=0; term_i != it->field_count; term_i++) {
		switch (it->query->terms[term_i].oper) {
			case ecs_oper_kind_t::EcsAnd:
				ctx->comp_ref_args[i_arg] = ctx->comp_ref_per_term[term_i];
				i_arg += 1;
				break;
			case ecs_oper_kind_t::EcsOptional:
				if (it->ids[term_i]) {
					ctx->comp_ref_args[i_arg] = ctx->comp_ref_per_term[term_i];
					i_arg += 1;
				}
				break;
			case ecs_oper_kind_t::EcsOr:
				if (it->ids[i_arg] == it->query->terms[term_i].id) {
					ctx->comp_ref_args[i_arg] = ctx->comp_ref_per_term[term_i];
					i_arg += 1;
				}
			case ecs_oper_kind_t::EcsNot:
				break;
		}

	}
}

void GlSystemBuilder::callback(ecs_iter_t* it) {
	SystemContext* ctx = (SystemContext*) it->callback_ctx;
	update_component_terms(it, ctx);
	for (int i=0; i != it->count; i++) {
		for (int comp_i=0; comp_i != ctx->comp_ref_args.size(); comp_i++) {
			Object* obj = ctx->comp_ref_args[comp_i];
			GlComponent* comp = obj->cast_to<GlComponent>(obj);
			comp->set_source_id(it->entities[i]);
		}
		ctx->get_callable().callv(ctx->comp_ref_args);
	}
}

void GlSystemBuilder::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("for_each", "callback"), &GlSystemBuilder::for_each);
}

// **********************************************
// *** PRIVATE ***
// **********************************************


SystemContext::~SystemContext() {
}

Callable SystemContext::get_callable() {
	return callable;
}
GlWorld* SystemContext::get_world() {
	return world;
}
