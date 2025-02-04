
#ifndef QUERY_ITERATOR_H
#define QUERY_ITERATOR_H

#include "base_iterator.h"
#include "godot_cpp/variant/variant.hpp"
#include "query.h"
#include "world.h"
#include <flecs.h>
#include <godot_cpp/classes/ref_counted.hpp>

namespace godot {

	class GFQueryIterator : public GFBaseIterator {
		GDCLASS(GFQueryIterator, GFBaseIterator)

	public:
		GFQueryIterator():
			GFBaseIterator({0})
		{}
		GFQueryIterator(ecs_iter_t iterator):
			GFBaseIterator(iterator)
		{}
		GFQueryIterator(ecs_iter_t iterator, GFWorld* world):
			GFBaseIterator(iterator, world)
		{}
		~GFQueryIterator() {
			if (!is_done()) {
				ecs_iter_fini(&iterator);
			}
		};

		// --------------------------------------
		// --- Exposed
		// --------------------------------------

		bool _iter_init(Variant arg);
		bool _iter_next(Variant arg);
		Variant _iter_get(Variant arg);

		// --------------------------------------
		// --- Unexposed
		// --------------------------------------

		bool next();

	protected:
		static void _bind_methods();

	private:
	};

}

#endif
