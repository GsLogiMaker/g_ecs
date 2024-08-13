
#ifndef QUERY_BUILDER_H
#define QUERY_BUILDER_H

#include <querylike_builder.h>
#include <query.h>

#include <flecs.h>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot {

	// Predefine instead of include to avoid cyclic dependencies
	class GlWorld;

	class GlQueryBuilder : public GlQuerylikeBuilder {
		GDCLASS(GlQueryBuilder, GlQuerylikeBuilder)

	public:
		GlQueryBuilder();
		GlQueryBuilder(GlWorld* world): GlQuerylikeBuilder(world) {}
		~GlQueryBuilder();

		// **************************************
		// *** Exposed ***
		// **************************************

		Ref<GlQuery> build();

		// **************************************
		// *** Unexposed ***
		// **************************************

	protected:
		static void _bind_methods();

	private:

	};

}

#endif
