
#ifndef QUERYLike_BUILDER_H
#define QUERYLike_BUILDER_H

#include "component.h"
#include "godot_cpp/variant/callable.hpp"
#include <query.h>

#include <flecs.h>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot {

	// Predefine instead of include to avoid cyclic dependencies
	class GlWorld;
	class QueryIterationContext;

	class GlQuerylikeBuilder : public RefCounted {
		GDCLASS(GlQuerylikeBuilder, RefCounted)

		friend QueryIterationContext;

	public:
		GlQuerylikeBuilder();
		GlQuerylikeBuilder(GlWorld* world): world(world) {}
		~GlQuerylikeBuilder();

		// **************************************
		// *** Exposed ***
		// **************************************

		int get_term_count();
		GlWorld* get_world();
		bool is_built();
		Ref<GlQuerylikeBuilder> with(Variant component);

		// **************************************
		// *** Unexposed ***
		// **************************************

		void set_world(GlWorld*);

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
		GlWorld* world{nullptr};

	};

	class QueryIterationContext {
	public:
		Array comp_ref_per_term {Array()};
		Array comp_ref_args {Array()};

		QueryIterationContext(
			Ref<GlQuerylikeBuilder> query_b,
			Callable callable
		);
		~QueryIterationContext();

		Callable get_callable();
		GlWorld* get_world();

		void update_component_entities(ecs_iter_t* it, int entity_index);
		void update_component_terms(ecs_iter_t* it);
		static void iterator_callback(ecs_iter_t* it);

	private:
		Callable callable;
		GlWorld* world;
	};

}

#endif
