

#include "querylike_builder.h"
#include "godot_cpp/variant/callable.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "utils.h"
#include "world.h"

#include <stdlib.h>
#include <flecs.h>

using namespace godot;

GFQuerylikeBuilder::GFQuerylikeBuilder() {
}
GFQuerylikeBuilder::~GFQuerylikeBuilder() {
}

// **************************************
// *** Exposed ***
// **************************************

int GFQuerylikeBuilder::get_term_count() {
	return term_count;
}

GFWorld* GFQuerylikeBuilder::get_world() {
	return world;
}

bool GFQuerylikeBuilder::is_built() {
	return built;
}

Ref<GFQuerylikeBuilder> GFQuerylikeBuilder::with(Variant component) {
	ecs_entity_t comp_id = world->coerce_id(component);

	query_desc.terms[get_term_count()] = {
		.id = comp_id,
		.inout = ecs_inout_kind_t::EcsInOut,
		.oper = ecs_oper_kind_t::EcsAnd
	};
	term_count += 1;

	return Ref(this);
}
Ref<GFQuerylikeBuilder> GFQuerylikeBuilder::maybe_with(Variant component) {
	ecs_entity_t comp_id = world->coerce_id(component);

	query_desc.terms[get_term_count()] = {
		.id = comp_id,
		.inout = ecs_inout_kind_t::EcsInOut,
		.oper = ecs_oper_kind_t::EcsOptional
	};
	term_count += 1;

	return Ref(this);
}
Ref<GFQuerylikeBuilder> GFQuerylikeBuilder::or_with(Variant component) {
	if (get_term_count() == 0) {
		ERR(Ref(this),
			"Could not add term to query\n",
			"OR terms can't be the first term in a query"
		);
	}
	if (query_desc.terms[get_term_count()-1].oper != ecs_oper_kind_t::EcsAnd) {
		ERR(Ref(this),
			"Could not add term to query\n",
			"OR terms can only be added after AND terms; use \"with\" for previous term instead"
		);
	}

	ecs_entity_t comp_id = world->coerce_id(component);

	query_desc.terms[get_term_count()-1].oper = ecs_oper_kind_t::EcsOr;
	query_desc.terms[get_term_count()] = {
		.id = comp_id,
		.inout = ecs_inout_kind_t::EcsInOut,
		.oper = ecs_oper_kind_t::EcsAnd
	};
	term_count += 1;

	return Ref(this);
}
Ref<GFQuerylikeBuilder> GFQuerylikeBuilder::without(Variant component) {
	ecs_entity_t comp_id = world->coerce_id(component);

	query_desc.terms[get_term_count()] = {
		.id = comp_id,
		.inout = ecs_inout_kind_t::EcsInOut,
		.oper = ecs_oper_kind_t::EcsNot
	};
	term_count += 1;

	return Ref(this);
}

// **************************************
// *** Unexposed ***
// **************************************

void GFQuerylikeBuilder::set_world(GFWorld* world_) {
	world = world_;
}

// **********************************************
// *** PROTECTED ***
// **********************************************

QueryIterationContext* GFQuerylikeBuilder::setup_ctx(Callable callable) {
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

void GFQuerylikeBuilder::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("is_built"), &GFQuerylikeBuilder::is_built);
	godot::ClassDB::bind_method(D_METHOD("with", "component"), &GFQuerylikeBuilder::with);
	godot::ClassDB::bind_method(D_METHOD("maybe_with", "component"), &GFQuerylikeBuilder::maybe_with);
	godot::ClassDB::bind_method(D_METHOD("or_with", "component"), &GFQuerylikeBuilder::or_with);
	godot::ClassDB::bind_method(D_METHOD("without", "component"), &GFQuerylikeBuilder::without);

}

// **********************************************
// *** PRIVATE ***
// **********************************************


// **********************************************
// *** QueryIterationContext PUBLIC ***
// **********************************************

QueryIterationContext::QueryIterationContext(
	Ref<GFQuerylikeBuilder> query_b,
	Callable callable_
) {
	callable = callable_;
	world = query_b->get_world();

	for (int i=0; i != query_b->get_term_count(); i++) {
		auto terms = query_b->query_desc.terms;
		switch (terms[i].oper) {
			case ecs_oper_kind_t::EcsAnd:
			case ecs_oper_kind_t::EcsOptional:
				comp_ref_per_term.append(Variant(Ref(memnew(
					GFComponent(0, terms[i].id, get_world())
				))));
				comp_ref_args.append(Variant());
				break;
			case ecs_oper_kind_t::EcsOr:
				comp_ref_per_term.append(Variant(Ref(memnew(
					GFComponent(0, terms[i].id, get_world())
				))));
				// OR terms must always be followed by another term, so
				// there's guarentied to be another term after this one
				if (terms[i+1].oper == ecs_oper_kind_t::EcsNot) {
					// NOT terms don't add args for term, so add
					// it here instead
					comp_ref_args.append(Variant());
				}
				break;
			case ecs_oper_kind_t::EcsNot:
				comp_ref_per_term.append(Variant());
				// TODO: If previous term was OR, then an argument will never
				//		be added for the OR chain. This needs addressing.
				break;
		}
	}
}
QueryIterationContext::~QueryIterationContext() {}

Callable QueryIterationContext::get_callable() {
	return callable;
}
GFWorld* QueryIterationContext::get_world() {
	return world;
}

void QueryIterationContext::update_component_entities(ecs_iter_t* it, int entity_index) {
	for (int comp_i=0; comp_i != comp_ref_args.size(); comp_i++) {
		Object* obj = comp_ref_args[comp_i];
		if (obj == nullptr) {
			continue;
		}
		GFComponent* comp = obj->cast_to<GFComponent>(obj);
		comp->set_source_id(it->entities[entity_index]);
	}
}

void QueryIterationContext::update_component_terms(ecs_iter_t* it) {
	auto query = it->query;
	auto terms = it->query->terms;
	int i_arg = 0;
	for (int term_i=0; term_i != query->term_count; term_i++) {
		auto term = terms[term_i];
		switch (term.oper) {
			case ecs_oper_kind_t::EcsAnd:
				comp_ref_args[i_arg] = comp_ref_per_term[term_i];
				i_arg += 1;
				break;

			case ecs_oper_kind_t::EcsOptional:
				if (it->ptrs[term_i] == nullptr) {
					// Term is null due to being optional
					comp_ref_args[i_arg] = Variant();
				} else {
					comp_ref_args[i_arg] = comp_ref_per_term[term_i];
				}
				i_arg += 1;
				break;

			case ecs_oper_kind_t::EcsOr:
				if (term.id == it->ids[i_arg]) {
					// Term matches OR chain, set arg to term
					comp_ref_args[i_arg] = comp_ref_per_term[term_i];
					i_arg += 1;
					// Skip terms till OR chain is exited.
					// There is guarentied to be another none OR term at the
					// end of the chain, so we don't need to check for the end of
					// the terms list.
					while (terms[term_i].oper == ecs_oper_kind_t::EcsOr) {
						term_i++;
					}
					// Final term in OR chain is skipped by for loop.
				} else {
					// Term does not match arg, set arg to null until
					// matching term is found in OR chain.
					// Arg may remain null if the final term is
					// optional or negative.
					comp_ref_args[i_arg] = Variant();
				}

				break;

			case ecs_oper_kind_t::EcsNot:
				// NOT terms don't add arguments, do nothing
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
