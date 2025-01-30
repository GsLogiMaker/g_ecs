
#ifndef ENTITY_ITERATOR_H
#define ENTITY_ITERATOR_H

#include "base_iterator.h"
#include <flecs.h>
#include <godot_cpp/classes/ref_counted.hpp>

namespace godot {

	class GFEntityIterator : public GFBaseIterator {
		GDCLASS(GFEntityIterator, GFBaseIterator)

	public:
		GFEntityIterator():
			GFBaseIterator()
		{}
		GFEntityIterator(ecs_iter_t iterator):
			GFBaseIterator(iterator)
		{}
		GFEntityIterator(ecs_iter_t iterator, GFWorld* world):
			GFBaseIterator(iterator, world)
		{}
		~GFEntityIterator() {};

		// --------------------------------------
		// --- Exposed
		// --------------------------------------

		bool _iter_next(Variant arg);

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
