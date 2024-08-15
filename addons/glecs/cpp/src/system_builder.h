
#ifndef SYSTEM_BUILDER_H
#define SYSTEM_BUILDER_H

#include "component.h"
#include "godot_cpp/variant/callable.hpp"
#include "godot_cpp/variant/variant.hpp"
#include <querylike_builder.h>

#include <flecs.h>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot {

	// Predefine instead of include to avoid cyclic dependencies
	class GlWorld;

	class GlSystemBuilder : public GlQuerylikeBuilder {
		GDCLASS(GlSystemBuilder, GlQuerylikeBuilder)

	public:
		GlSystemBuilder();
		GlSystemBuilder(GlWorld* world): GlQuerylikeBuilder(world) {}
		~GlSystemBuilder();

		// **************************************
		// *** Exposed ***
		// **************************************

		void for_each(Callable callable);

		// **************************************
		// *** Unexposed ***
		// **************************************

	protected:
		static void callback(ecs_iter_t *it);

		static void _bind_methods();

	private:
		Callable callable;
	};

	class SystemContext {
	public:
		Array comp_ref_per_term {Array()};
		Array comp_ref_args {Array()};

		SystemContext(GlWorld* world, Callable callable):
			world(world),
			callable(callable)
		{
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
		}
		~SystemContext();

		Callable get_callable();
		GlWorld* get_world();

	private:
		Callable callable;
		GlWorld* world;
	};

}

#endif
