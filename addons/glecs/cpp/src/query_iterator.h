
#ifndef QUERY_ITERATOR_H
#define QUERY_ITERATOR_H

#include "query.h"
#include <flecs.h>
#include <godot_cpp/classes/ref_counted.hpp>

namespace godot {

	class GlQueryIterator : public RefCounted {
		GDCLASS(GlQueryIterator, RefCounted)

	public:
		GlQueryIterator();
		GlQueryIterator(Ref<GlQuery> query, ecs_iter_t iterator):
			query(query),
			iterator(iterator)
			{}
		~GlQueryIterator();

		// --------------------------------------
		// --- Exposed
		// --------------------------------------

		bool _iter_init(Variant arg);
		bool _iter_next(Variant arg);
		Variant _iter_get(Variant arg);

		GlWorld* get_world();

		// --------------------------------------
		// --- Unexposed
		// --------------------------------------

		bool is_done();
		bool is_started();

		void set_done(bool value);
		void set_started(bool value);

	protected:
		static void _bind_methods();

	private:
		Ref<GlQuery> query;
		ecs_iter_t iterator;
		int index{0};
		uint8_t iter_flags{0};
	};

}

#endif
