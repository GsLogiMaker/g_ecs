

#include "querylike_builder.h"
#include "godot_cpp/variant/callable.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
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

QueryIterationContext* GlQuerylikeBuilder::setup_ctx(Callable callable) {
	QueryIterationContext* ctx =  new QueryIterationContext(
		Ref(this),
		callable
	);
	query_desc.binding_ctx = ctx;
	query_desc.binding_ctx_free = [](void* ptr) {
		delete (QueryIterationContext*) ptr;
	};

	return ctx;
}

void GlQuerylikeBuilder::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("is_built"), &GlQuerylikeBuilder::is_built);
	godot::ClassDB::bind_method(D_METHOD("with", "component"), &GlQuerylikeBuilder::with);

}

// **********************************************
// *** PRIVATE ***
// **********************************************


// **********************************************
// *** QueryIterationContext PUBLIC ***
// **********************************************

QueryIterationContext::QueryIterationContext(
	Ref<GlQuerylikeBuilder> query_b,
	Callable callable_
) {
	callable = callable_;
	world = query_b->get_world();

	comp_ref_per_term = Array();
	comp_ref_args = Array();
	comp_ref_per_term.set_typed(
		Variant::OBJECT,
		GlComponent::get_class_static(),
		Variant(nullptr)
	);
	comp_ref_args.set_typed(
		Variant::OBJECT,
		GlComponent::get_class_static(),
		Variant(nullptr)
	);

	for (int i=0; i != query_b->get_term_count(); i++) {
		switch (query_b->query_desc.terms[i].oper) {
			case ecs_oper_kind_t::EcsAnd:
			case ecs_oper_kind_t::EcsOr:
			case ecs_oper_kind_t::EcsOptional:
				comp_ref_per_term.append(Variant(Ref(memnew(
					GlComponent(0, query_b->query_desc.terms[i].id, get_world())
				))));
				comp_ref_args.append(Variant());
				break;
			case ecs_oper_kind_t::EcsNot:
				comp_ref_per_term.append(Variant());
				break;
		}
	}
}
QueryIterationContext::~QueryIterationContext() {}

Callable QueryIterationContext::get_callable() {
	return callable;
}
GlWorld* QueryIterationContext::get_world() {
	return world;
}

void QueryIterationContext::update_component_entities(ecs_iter_t* it, int entity_index) {
	for (int comp_i=0; comp_i != comp_ref_args.size(); comp_i++) {
		Object* obj = comp_ref_args[comp_i];
		GlComponent* comp = obj->cast_to<GlComponent>(obj);
		comp->set_source_id(it->entities[entity_index]);
	}
}

void QueryIterationContext::update_component_terms(ecs_iter_t* it) {
	int i_arg = 0;
	for (int term_i=0; term_i != it->field_count; term_i++) {
		switch (it->query->terms[term_i].oper) {
			case ecs_oper_kind_t::EcsAnd:
				comp_ref_args[i_arg] = comp_ref_per_term[term_i];
				i_arg += 1;
				break;
			case ecs_oper_kind_t::EcsOptional:
				if (it->ids[term_i]) {
					comp_ref_args[i_arg] = comp_ref_per_term[term_i];
					i_arg += 1;
				}
				break;
			case ecs_oper_kind_t::EcsOr:
				if (it->ids[i_arg] == it->query->terms[term_i].id) {
					comp_ref_args[i_arg] = comp_ref_per_term[term_i];
					i_arg += 1;
				}
			case ecs_oper_kind_t::EcsNot:
				break;
		}

	}
}

void QueryIterationContext::iterator_callback(ecs_iter_t* it) {
	QueryIterationContext* ctx = (QueryIterationContext*) it->query->binding_ctx;
	ctx->update_component_terms(it);
	for (int i=0; i != it->count; i++) {
		ctx->update_component_entities(it, i);
		ctx->get_callable().callv(ctx->comp_ref_args);
	}
}
