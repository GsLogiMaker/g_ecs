
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
			iterator(iterator),
			done(false)
			{}
		~GlQueryIterator();

		// --------------------------------------
		// --- Exposed
		// --------------------------------------

		bool _iter_init(Variant arg);
		bool _iter_next(Variant arg);
		Variant _iter_get(Variant arg);

		// --------------------------------------
		// --- Unexposed
		// --------------------------------------

	protected:
		static void _bind_methods();

	private:
		Ref<GlQuery> query;
		ecs_iter_t iterator;
		int index;
		bool done;
	};

}

#endif
