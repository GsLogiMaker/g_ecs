
#ifndef QUERYLike_BUILDER_H
#define QUERYLike_BUILDER_H

#include "component.h"
#include "godot_cpp/variant/callable.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include <query.h>

#include <flecs.h>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot {

	// Predefine instead of include to avoid cyclic dependencies
	class GFWorld;
	class QueryIterationContext;

	class GFQuerylikeBuilder : public RefCounted {
		GDCLASS(GFQuerylikeBuilder, RefCounted)

		friend QueryIterationContext;

	public:
		GFQuerylikeBuilder():
			GFQuerylikeBuilder(nullptr)
		{}
		GFQuerylikeBuilder(GFWorld* world):
			query_desc( {0} ),
			built(false),
			term_count(0),
			world(world)
		{}
		~GFQuerylikeBuilder();

		// **************************************
		// *** Exposed ***
		// **************************************

		int get_term_count();
		GFWorld* get_world();
		bool is_built();
		Ref<GFQuerylikeBuilder> access_default();
		Ref<GFQuerylikeBuilder> access_filter();
		Ref<GFQuerylikeBuilder> access_in();
		Ref<GFQuerylikeBuilder> access_inout();
		Ref<GFQuerylikeBuilder> access_none();
		Ref<GFQuerylikeBuilder> access_out();
		Ref<GFQuerylikeBuilder> with(Variant component);
		Ref<GFQuerylikeBuilder> maybe_with(Variant component);
		Ref<GFQuerylikeBuilder> or_with(Variant component);
		Ref<GFQuerylikeBuilder> without(Variant component);

		// **************************************
		// *** Unexposed ***
		// **************************************

		void set_world(GFWorld*);

	protected:
		/// The Flecs description of the building query.
		ecs_query_desc_t query_desc{0};
		/// Is true if this builder has already been built
		bool built{false};

		QueryIterationContext* setup_ctx(Callable callable);
		static void _bind_methods();

	private:
		/// The number of terms added to the query so far.
		int term_count{0};
		/// The world to query in.
		GFWorld* world{nullptr};

	};

	class QueryIterationContext {
	public:
		TypedArray<GFComponent> comp_ref_per_term {TypedArray<GFComponent>()};
		TypedArray<GFComponent> comp_ref_args {TypedArray<GFComponent>()};

		QueryIterationContext(
			Ref<GFQuerylikeBuilder> query_b,
			Callable callable
		);
		~QueryIterationContext();

		Callable get_callable();
		GFWorld* get_world();

		void update_component_entities(ecs_iter_t* it, int entity_index);
		void update_component_terms(ecs_iter_t* it);
		static void iterator_callback(ecs_iter_t* it);

	private:
		Callable callable;
		GFWorld* world;
	};

}

#endif
