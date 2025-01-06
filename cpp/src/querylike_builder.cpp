

#include "querylike_builder.h"
#include "component.h"
#include "godot_cpp/variant/callable.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "utils.h"
#include "world.h"

#include <stdlib.h>
#include <flecs.h>

using namespace godot;

#define CHECK_HAS_A_TERM(return_value, err) \
	if (get_term_count() == 0) { \
		ERR(return_value, \
			err, \
			"	Query has no terms" \
		); \
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

Ref<GFQuerylikeBuilder> GFQuerylikeBuilder::access_default() {
	CHECK_HAS_A_TERM(Ref(this),
		"Failed to set term's access mode to `default`\n"
	);
	query_desc.terms[get_term_count()-1].inout = ecs_inout_kind_t::EcsInOutDefault;
	return Ref(this);
}
Ref<GFQuerylikeBuilder> GFQuerylikeBuilder::access_filter() {
	CHECK_HAS_A_TERM(Ref(this),
		"Failed to set term's access mode to `filter`\n"
	);
	query_desc.terms[get_term_count()-1].inout = ecs_inout_kind_t::EcsInOutFilter;
	return Ref(this);
}
Ref<GFQuerylikeBuilder> GFQuerylikeBuilder::access_in() {
	CHECK_HAS_A_TERM(Ref(this),
		"Failed to set term's access mode to `in`\n"
	);
	query_desc.terms[get_term_count()-1].inout = ecs_inout_kind_t::EcsIn;
	return Ref(this);
}
Ref<GFQuerylikeBuilder> GFQuerylikeBuilder::access_inout() {
	CHECK_HAS_A_TERM(Ref(this),
		"Failed to set term's access mode to `inout`\n"
	);
	query_desc.terms[get_term_count()-1].inout = ecs_inout_kind_t::EcsInOut;
	return Ref(this);
}
Ref<GFQuerylikeBuilder> GFQuerylikeBuilder::access_none() {
	CHECK_HAS_A_TERM(Ref(this),
		"Failed to set term's access mode to `none`\n"
	);
	query_desc.terms[get_term_count()-1].inout = ecs_inout_kind_t::EcsInOutNone;
	return Ref(this);
}
Ref<GFQuerylikeBuilder> GFQuerylikeBuilder::access_out() {
	CHECK_HAS_A_TERM(Ref(this),
		"Failed to set term's access mode to `out`\n"
	);
	query_desc.terms[get_term_count()-1].inout = ecs_inout_kind_t::EcsOut;
	return Ref(this);
}

Ref<GFQuerylikeBuilder> GFQuerylikeBuilder::with(Variant component) {
	ecs_entity_t comp_id = world->coerce_id(component);

	CHECK_ENTITY_ALIVE(comp_id, world,
		Ref(this),
		"Failed to add term to query\n"
	);

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

	CHECK_ENTITY_ALIVE(comp_id, world,
		Ref(this),
		"Failed to add `optional` term to query\n"
	);

	query_desc.terms[get_term_count()] = {
		.id = comp_id,
		.inout = ecs_inout_kind_t::EcsInOut,
		.oper = ecs_oper_kind_t::EcsOptional
	};
	term_count += 1;

	return Ref(this);
}
Ref<GFQuerylikeBuilder> GFQuerylikeBuilder::or_with(Variant component) {
	CHECK_HAS_A_TERM(Ref(this),
		"Failed to add `or` term to query\n"
	);
	if (query_desc.terms[get_term_count()-1].oper != ecs_oper_kind_t::EcsAnd) {
		ERR(Ref(this),
			"Could not add term to query\n",
			"OR terms can only be added after AND terms; use \"with\" for previous term instead"
		);
	}

	ecs_entity_t comp_id = world->coerce_id(component);

	CHECK_ENTITY_ALIVE(comp_id, world,
		Ref(this),
		"Failed to add `or` term to query\n"
	);

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

	CHECK_ENTITY_ALIVE(comp_id, world,
		Ref(this),
		"Failed to add `negative` term to query\n"
	);

	query_desc.terms[get_term_count()] = {
		.id = comp_id,
		.inout = ecs_inout_kind_t::EcsInOut,
		.oper = ecs_oper_kind_t::EcsNot
	};
	term_count += 1;

	return Ref(this);
}

Ref<GFQuerylikeBuilder> GFQuerylikeBuilder::up(Variant entity) {
	ecs_entity_t entity_id = 0;
	if (!entity) {
		// Passed variant is null, use the default ChildOf tag
		entity_id = ecs_lookup(world->raw(), "flecs.core.ChildOf");
	} else {
		// Passed variant is not null, coerce it to an entity ID
		entity_id = world->coerce_id(entity);
	}

	CHECK_ENTITY_ALIVE(entity_id, world,
		Ref(this),
		"Failed to add `up` traversal to query\n"
	);

	query_desc.terms[get_term_count()-1].src.id = EcsUp;
	query_desc.terms[get_term_count()-1].trav = entity_id;

	return Ref(this);
}
Ref<GFQuerylikeBuilder> GFQuerylikeBuilder::descend(Variant entity) {
	ecs_entity_t entity_id = world->coerce_id(entity);

	CHECK_ENTITY_ALIVE(entity_id, world,
		Ref(this),
		"Failed to add `descend` traversal to query\n"
	);

	query_desc.terms[get_term_count()-1].src.id = EcsDesc;
	query_desc.terms[get_term_count()-1].trav = entity_id;

	return Ref(this);
}
Ref<GFQuerylikeBuilder> GFQuerylikeBuilder::cascade(Variant entity) {
	ecs_entity_t entity_id = world->coerce_id(entity);

	CHECK_ENTITY_ALIVE(entity_id, world,
		Ref(this),
		"Failed to add `cascade` traversal to query\n"
	);

	query_desc.terms[get_term_count()-1].src.id = EcsCascade;
	query_desc.terms[get_term_count()-1].trav = entity_id;

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
		delete static_cast<QueryIterationContext*>(ptr);
	};

	if (callable.get_argument_count() != ctx->comp_ref_args.size()) {
		ERR(ctx,
			"Failed to setup query context\n",
			"Query expected a callable with ",
			ctx->comp_ref_args.size(),
			" arguments, but got callable with ",
			callable.get_argument_count(),
			" arguments."
		);
	}

	return ctx;
}

void GFQuerylikeBuilder::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("is_built"), &GFQuerylikeBuilder::is_built);

	godot::ClassDB::bind_method(D_METHOD("access_default"), &GFQuerylikeBuilder::access_default);
	godot::ClassDB::bind_method(D_METHOD("access_filter"), &GFQuerylikeBuilder::access_filter);
	godot::ClassDB::bind_method(D_METHOD("access_in"), &GFQuerylikeBuilder::access_in);
	godot::ClassDB::bind_method(D_METHOD("access_inout"), &GFQuerylikeBuilder::access_inout);
	godot::ClassDB::bind_method(D_METHOD("access_none"), &GFQuerylikeBuilder::access_none);
	godot::ClassDB::bind_method(D_METHOD("access_out"), &GFQuerylikeBuilder::access_out);

	godot::ClassDB::bind_method(D_METHOD("with", "component"), &GFQuerylikeBuilder::with);
	godot::ClassDB::bind_method(D_METHOD("maybe_with", "component"), &GFQuerylikeBuilder::maybe_with);
	godot::ClassDB::bind_method(D_METHOD("or_with", "component"), &GFQuerylikeBuilder::or_with);
	godot::ClassDB::bind_method(D_METHOD("without", "component"), &GFQuerylikeBuilder::without);

	godot::ClassDB::bind_method(D_METHOD("up", "traversal"), &GFQuerylikeBuilder::up, 0);
	godot::ClassDB::bind_method(D_METHOD("descend", "traversal"), &GFQuerylikeBuilder::descend, 0);
	godot::ClassDB::bind_method(D_METHOD("cascade", "traversal"), &GFQuerylikeBuilder::cascade, 0);

}

// **********************************************
// *** PRIVATE ***
// **********************************************


// **********************************************
// *** QueryIterationContext PUBLIC ***
// **********************************************

ecs_entity_t get_compmonent_of_term(ecs_term_t* term) {
	if (term->id == 0) {
		ERR(0,
			"Could not get component of term\n",
			"Term ID is 0"
		);
	}
	return term->id;
}

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
				comp_ref_per_term.append(
					GFComponent::from_id_no_source(
						get_compmonent_of_term(&terms[i]),
						get_world()
					)
				);
				comp_ref_args.append(Variant());
				comp_ref_term_ids.append(i);
				break;
			case ecs_oper_kind_t::EcsOr:
				comp_ref_per_term.append(
					GFComponent::from_id_no_source(
						get_compmonent_of_term(&terms[i]),
						get_world()
					)
				);
				// OR terms must always be followed by another term, so
				// there's guaranteed to be another term after this one
				if (terms[i+1].oper == ecs_oper_kind_t::EcsNot) {
					// NOT terms don't add args for term, so add
					// it here instead
					comp_ref_args.append(Variant());
					comp_ref_term_ids.append(i);
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
		int term_i = comp_ref_term_ids[comp_i];

		ecs_entity_t entity = ecs_field_src(it, term_i);
		if (entity == 0) {
			entity = it->entities[entity_index];
		}

		Ref<GFComponent> comp = comp_ref_args[comp_i];
		if (comp == nullptr) {
			continue;
		}

		comp->set_source_id(entity);
	}
}

void QueryIterationContext::update_component_terms(ecs_iter_t* it) {
	auto query = it->query;
	auto terms = it->query->terms;

	int i_arg = 0;
	for (int term_i=0; term_i != query->term_count; term_i++) {
		ecs_entity_t entity_id = ecs_field_src(it, term_i);
		if (entity_id == 0) {
			entity_id = it->entities[0];
		}

		const ecs_term_t* term = &terms[term_i];
		Ref<GFComponent> comp_ref = comp_ref_per_term[term_i];
		switch (term->oper) {
			case ecs_oper_kind_t::EcsAnd:
				comp_ref_args[i_arg] = comp_ref;
				comp_ref_term_ids[i_arg] = term_i;
				i_arg += 1;
				break;

			case ecs_oper_kind_t::EcsOptional:
				if (!ecs_has_id(world->raw(), entity_id, term->id)) {
					// Term is null due to being optional
					comp_ref_args[i_arg] = Variant();
					comp_ref_term_ids[i_arg] = term_i;
				} else {
					comp_ref_args[i_arg] = comp_ref;
					comp_ref_term_ids[i_arg] = term_i;
				}
				i_arg += 1;
				break;

			case ecs_oper_kind_t::EcsOr:
				if (term->id == it->ids[i_arg]) {
					// Term matches OR chain, set arg to term
					comp_ref_args[i_arg] = comp_ref;
					comp_ref_term_ids[i_arg] = term_i;
					i_arg += 1;
					// Skip terms till OR chain is exited.
					// There is guaranteed to be another none OR term at the
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
					comp_ref_term_ids[i_arg] = term_i;
				}

				break;

			case ecs_oper_kind_t::EcsNot:
				// NOT terms don't add arguments, do nothing
				break;
		}

	}
}

void QueryIterationContext::iterator_callback(ecs_iter_t* it) {
	QueryIterationContext* ctx = static_cast<QueryIterationContext*>(it->query->binding_ctx);
	ctx->update_component_terms(it);
	for (int i=0; i != it->count; i++) {
		ctx->update_component_entities(it, i);
		ctx->get_callable().callv(ctx->comp_ref_args);
	}
}
