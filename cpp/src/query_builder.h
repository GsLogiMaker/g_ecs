
#ifndef QUERY_BUILDER_H
#define QUERY_BUILDER_H

#include <querylike_builder.h>
#include <query.h>

#include <flecs.h>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot {

	// Predefine instead of include to avoid cyclic dependencies
	class GFWorld;

	class GFQueryBuilder : public GFQuerylikeBuilder {
		GDCLASS(GFQueryBuilder, GFQuerylikeBuilder)

	public:
		GFQueryBuilder(const GFWorld* world):
			GFQuerylikeBuilder(world)
		{}
		GFQueryBuilder():
			GFQueryBuilder(GFWorld::singleton())
		{}
		~GFQueryBuilder();

		// **************************************
		// *** Exposed ***
		// **************************************

		OVERRIDE_QUERYLIKE_SELF_METHODS(GFQueryBuilder);

		static Ref<GFQueryBuilder> new_in_world(const GFWorld*);
		Ref<GFQuery> build();

		// **************************************
		// *** Unexposed ***
		// **************************************

	protected:
		static void _bind_methods();

	private:

	};

}

#endif
