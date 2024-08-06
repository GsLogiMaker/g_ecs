
#ifndef WORLD_H
#define WORLD_H

#include <flecs.h>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/ref_counted.hpp>

namespace godot {

	// Predefine instead of include to avoid cyclic dependencies
	class GlComponentBuilder;

	class GlWorld : public Object {
		GDCLASS(GlWorld, Object)

	public:
		GlWorld();
		~GlWorld();

		// **************************************
		// *** Exposed ***
		// **************************************

		Ref<GlComponentBuilder> component_builder();

		ecs_entity_t coerce_id(Variant);
		void progress(double delta);
		void start_rest_api();

		// **************************************
		// *** Unexposed ***
		// **************************************

		// *** Glecs entities ***
		static ecs_entity_t glecs;
		static ecs_entity_t glecs_meta;
		static ecs_entity_t glecs_meta_real;
		static ecs_entity_t glecs_meta_vector2;
		static ecs_entity_t glecs_meta_vector2i;
		static ecs_entity_t glecs_meta_rect2;
		static ecs_entity_t glecs_meta_rect2i;
		static ecs_entity_t glecs_meta_vector3;
		static ecs_entity_t glecs_meta_vector3i;
		static ecs_entity_t glecs_meta_transform2d;
		static ecs_entity_t glecs_meta_vector4;
		static ecs_entity_t glecs_meta_vector4i;
		static ecs_entity_t glecs_meta_plane;
		static ecs_entity_t glecs_meta_quaternion;
		static ecs_entity_t glecs_meta_aabb;
		static ecs_entity_t glecs_meta_basis;
		static ecs_entity_t glecs_meta_transform3d;
		static ecs_entity_t glecs_meta_projection;
		static ecs_entity_t glecs_meta_color;
		static ecs_entity_t glecs_meta_string_name;
		static ecs_entity_t glecs_meta_node_path;
		static ecs_entity_t glecs_meta_rid;
		static ecs_entity_t glecs_meta_object;
		static ecs_entity_t glecs_meta_callable;
		static ecs_entity_t glecs_meta_signal;
		static ecs_entity_t glecs_meta_dictionary;
		static ecs_entity_t glecs_meta_array;
		static ecs_entity_t glecs_meta_packed_int32_array;
		static ecs_entity_t glecs_meta_packed_int64_array;
		static ecs_entity_t glecs_meta_packed_float32_array;
		static ecs_entity_t glecs_meta_packed_float64_array;
		static ecs_entity_t glecs_meta_packed_string_array;
		static ecs_entity_t glecs_meta_packed_vector2_array;
		static ecs_entity_t glecs_meta_packed_vector3_array;
		static ecs_entity_t glecs_meta_packed_color_array;

		static GlWorld* singleton();
		ecs_world_t* raw();

	protected:
		static void _bind_methods();

	private:
		ecs_world_t* _raw;

		template<typename T>
		static void gd_type_ctor(
			void* ptr,
			int32_t count,
			const ecs_type_info_t* type_info
		) {
			T* list = (T*)ptr;
			for (int i=0; i != count; i++) {
				list[i] = T();
			}
		}

		template<typename T>
		static void gd_type_dtor(
			void* ptr,
			int32_t count,
			const ecs_type_info_t* type_info
		) {
			T* list = (T*)ptr;
			for (int i=0; i != count; i++) {
				list[i].~T();
			}
		}

		template<typename T>
		static void gd_type_copy(
			void* dst_ptr,
			const void* src_ptr,
			int32_t count,
			const ecs_type_info_t* type_info
		) {
			T* dst_list = (T*)dst_ptr;
			const T* src_list = (const T*)src_ptr;
			for (int i=0; i != count; i++) {
				dst_list[i] = T(src_list[i]);
			}
		}

		template<typename T>
		static void gd_type_move(
			void* dst_ptr,
			void* src_ptr,
			int32_t count,
			const ecs_type_info_t* type_info
		) {
			T* dst_list = (T*)dst_ptr;
			T* src_list = (T*)src_ptr;
			for (int i=0; i != count; i++) {
				dst_list[i] = T(src_list[i]);
			}
		}

		template<typename T>
		void define_gd_component(
			const char* name,
			ecs_entity_t* static_id
		) {
			ecs_component_desc_t desc = {
				.entity = ecs_new_from_path(_raw, glecs_meta, name),
				.type = {
					.size = sizeof(T),
					.alignment = 8
				}
			};
			*static_id = ecs_component_init(_raw, &desc);
		}

		template<typename T>
		void define_gd_component_with_hooks(
			const char* name,
			ecs_entity_t* static_id
		) {
			define_gd_component<T>(name, static_id);
			ecs_type_hooks_t hooks = {
				.ctor = GlWorld::gd_type_ctor<T>,
				.dtor = GlWorld::gd_type_dtor<T>,
				.copy = GlWorld::gd_type_copy<T>,
				.move = GlWorld::gd_type_move<T>
			}; ecs_set_hooks_id(_raw, *static_id, &hooks);
		}

	};
}

#endif
