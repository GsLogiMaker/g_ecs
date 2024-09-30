
#ifndef QUERY_ITERATOR_H
#define QUERY_ITERATOR_H

#include "query.h"
#include <flecs.h>
#include <godot_cpp/classes/ref_counted.hpp>

namespace godot {

	class GFQueryIterator : public RefCounted {
		GDCLASS(GFQueryIterator, RefCounted)

	public:
		GFQueryIterator();
		GFQueryIterator(Ref<GFQuery> query, ecs_iter_t iterator):
			query(query),
			iterator(iterator)
			{}
		~GFQueryIterator();

		// --------------------------------------
		// --- Exposed
		// --------------------------------------

		bool _iter_init(Variant arg);
		bool _iter_next(Variant arg);
		Variant _iter_get(Variant arg);

		GFWorld* get_world();

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
		Ref<GFQuery> query;
		ecs_iter_t iterator;
		int index{0};
		uint8_t iter_flags{0};
	};

}

#endif
