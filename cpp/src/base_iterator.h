
#ifndef BASE_ITERATOR_H
#define BASE_ITERATOR_H

#include "world.h"
#include "utils.h"

#include <flecs.h>
#include <godot_cpp/classes/ref_counted.hpp>

#define NEXT(ECS_NEXT, PER_TABLE, PER_ENTITY)	\
	if (is_done()) {	\
		ERR(false,	\
			"Can't get next in ", get_class_static(), "\n",	\
			"Iterator is already exhausted"                	\
		);	\
	}                             	\
	if (index == iterator.count) {	\
		index = 0;                     	\
		set_done(!ECS_NEXT(&iterator));	\
		if (is_done()) {               	\
			return false;	\
		}           	\
		PER_TABLE();	\
	}            	\
	PER_ENTITY();	\
	\
	index += 1;       	\
	return !is_done();	\
;

namespace godot {

	class QueryIterationContext;

	class GFBaseIterator : public RefCounted {
		GDCLASS(GFBaseIterator, RefCounted)

	public:
		GFBaseIterator():
			GFBaseIterator({0})
		{}
		GFBaseIterator(ecs_iter_t iterator):
			GFBaseIterator(iterator, nullptr)
		{}
		GFBaseIterator(ecs_iter_t iterator, GFWorld* world):
			iterator(iterator),
			world(GFWorld::world_or_singleton(world))
		{}
		~GFBaseIterator();

		// --------------------------------------
		// --- Exposed
		// --------------------------------------

		bool _iter_init(Variant arg);
		bool _iter_next(Variant arg);
		Variant _iter_get(Variant arg);

		GFWorld* get_world() const;

		// --------------------------------------
		// --- Unexposed
		// --------------------------------------

		QueryIterationContext* get_context();

		bool progress_iterator();

		bool next();

		bool is_done() const;
		bool is_started() const;

		void set_done(bool value);
		void set_started(bool value);

	protected:
		ecs_iter_t iterator;
		int index{0};

		static void _bind_methods();

	private:
		GFWorld* world;
		uint8_t iter_flags{0};
	};

}

#endif
