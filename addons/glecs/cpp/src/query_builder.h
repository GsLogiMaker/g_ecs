
#ifndef QUERY_BUILDER_H
#define QUERY_BUILDER_H

#include <query.h>

#include <flecs.h>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot {

	// Predefine instead of include to avoid cyclic dependencies
	class GlWorld;

	class GlQueryBuilder : public RefCounted {
		GDCLASS(GlQueryBuilder, RefCounted)

	public:
		GlQueryBuilder();
		GlQueryBuilder(GlWorld* world): world(world) {}
		~GlQueryBuilder();

		// **************************************
		// *** Exposed ***
		// **************************************

		Ref<GlQuery> build();
		int get_term_count();
		GlWorld* get_world();
		Ref<GlQueryBuilder> with(Variant component);

		// **************************************
		// *** Unexposed ***
		// **************************************

		void set_world(GlWorld*);

	protected:
		static void _bind_methods();

	private:
		/// The Flecs description of the building query.
		ecs_query_desc_t query_desc{0};
		/// The number of terms added to the query so far.
		int term_count{0};
		/// The world to query in.
		GlWorld* world{nullptr};
		/// Is true if this builder has already been built
		bool built{false};

	};

}

#endif
