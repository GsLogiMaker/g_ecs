
#ifndef QUERYLike_BUILDER_H
#define QUERYLike_BUILDER_H

#include <query.h>

#include <flecs.h>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot {

	// Predefine instead of include to avoid cyclic dependencies
	class GlWorld;

	class GlQuerylikeBuilder : public RefCounted {
		GDCLASS(GlQuerylikeBuilder, RefCounted)

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

		static void _bind_methods();

	private:
		/// The number of terms added to the query so far.
		int term_count{0};
		/// The world to query in.
		GlWorld* world{nullptr};

	};

}

#endif
