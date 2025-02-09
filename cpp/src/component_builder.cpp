

#include "component_builder.h"
#include "entity_builder.h"
#include "godot_cpp/variant/utility_functions.hpp"
#include "world.h"
#include "utils.h"
#include "entity.h"

#include <stdlib.h>
#include <flecs.h>

using namespace godot;

GFComponentBuilder::~GFComponentBuilder() {
}

Ref<GFComponentBuilder> GFComponentBuilder::new_in_world(GFWorld* world) {
	return memnew(GFComponentBuilder(world));
}

Ref<GFComponentBuilder> GFComponentBuilder::add_member(
	const String member,
	Variant::Type type
) {
	const char* ERR_ADD_COMPONENT = "Failed to add member to component builder\n";

	if (get_member_count() == ECS_MEMBER_DESC_CACHE_SIZE) {
		ERR(Ref(this),
			ERR_ADD_COMPONENT,
			"Max member count reached"
		);
	}

	EntityResult ecs_type_result = GFWorld::variant_type_to_id(type);
	if (!ecs_type_result.is_ok()) {
		ERR(Ref(this),
			ERR_ADD_COMPONENT,
			ecs_type_result.unwrap_err()
		);
	}
	ecs_entity_t ecs_type = ecs_type_result.unwrap();

	struct_desc.members[get_member_count()] = {
		.type = ecs_type
	};
	member_names.append(member);

	return Ref(this);
}

int GFComponentBuilder::get_member_count() const {
	return member_names.size();
}


bool GFComponentBuilder::is_built() const {
	return built_count != 0;
}

Ref<GFEntity> GFComponentBuilder::build() {
	GFWorld* w = get_world();

	if (is_built()) {
		ERR(nullptr,
			"Failed to build component \"" + name + "\".\n",
			"	Component is already built."
		);
	}
	if (get_member_count() == 0) {
		ERR(nullptr,
			"Failed to build component \"" + name + "\".\n",
			"	No members were defined. Specify at least one member."
		);
	}

	// Create component entity
	Ref<GFEntity> entity = GFEntityBuilder::build();
	component_desc.entity = entity->get_id();
	struct_desc.entity = entity->get_id();

	// Set name to temporary pointers
	CharString name_utf8 = name.utf8();
	component_desc.type.name = name_utf8.ptr();
	CharString member_names_utf8[ECS_MEMBER_DESC_CACHE_SIZE] = {0};

	// Setup temporary member names
	for (int i=0; i != get_member_count(); i++) {
		member_names_utf8[i] = String(member_names[i]).utf8();
		struct_desc.members[i].name = member_names_utf8[i].ptr();
	}

	ecs_struct_init(w->raw(), &struct_desc);

	if (!entity->has_entity(ecs_id(EcsComponent), nullptr)) {
		ERR(nullptr,
			"Failed to build component with ID: ", entity->get_id(), "\n",
			"	Component entity was not created."
		);
	}

	ecs_type_hooks_t hooks = {
		.ctor = GFComponentBuilder::ctor,
		.dtor = GFComponentBuilder::dtor,
		.copy = GFComponentBuilder::copy,
		.move = GFComponentBuilder::move,
		.binding_ctx = new HooksBindingContext(w),
		.binding_ctx_free = [](void* ptr) {
			HooksBindingContext* ctx = static_cast<HooksBindingContext*>(ptr);
			delete ctx;
		}
	}; ecs_set_hooks_id(w->raw(), entity->get_id(), &hooks);

	return entity;
}

// **********************************************
// *** PROTECTED ***
// **********************************************

void GFComponentBuilder::_bind_methods() {
	REGISTER_ENTITY_BUILDER_SELF_METHODS(GFComponentBuilder);

	godot::ClassDB::bind_static_method(get_class_static(), D_METHOD("new_in_world", "world"), &GFComponentBuilder::new_in_world);
	godot::ClassDB::bind_method(D_METHOD("build"), &GFComponentBuilder::build);
	godot::ClassDB::bind_method(D_METHOD("add_member", "member", "type"), &GFComponentBuilder::add_member);
	godot::ClassDB::bind_method(D_METHOD("get_member_count"), &GFComponentBuilder::get_member_count);
}

// **********************************************
// *** PRIVATE ***
// **********************************************

void GFComponentBuilder::ctor(void* ptr, int32_t count, const ecs_type_info_t* type_info) {
	uint8_t* list = static_cast<uint8_t*>(ptr);
	HooksBindingContext* ctx = static_cast<HooksBindingContext*>(type_info->hooks.binding_ctx);
	GFWorld* w = ctx->get_world();

	for (int i=0; i != count; i++) {
		uint8_t* item = &list[i*type_info->size];
		w->init_component_ptr(static_cast<void*>(item), type_info->component, Variant());
	}
}
void GFComponentBuilder::dtor(void* ptr, int32_t count, const ecs_type_info_t* type_info) {
	uint8_t* list = static_cast<uint8_t*>(ptr);
	HooksBindingContext* ctx = static_cast<HooksBindingContext*>(type_info->hooks.binding_ctx);
	GFWorld* w = ctx->get_world();

	for (int i=0; i != count; i++) {
		uint8_t* item = &list[i*type_info->size];
		w->deinit_component_ptr(static_cast<void*>(item), type_info->component);
	}
}
void GFComponentBuilder::copy(
	void* dst_ptr,
	const void* src_ptr,
	int32_t count,
	const ecs_type_info_t* type_info
) {
	const uint8_t* src_list = static_cast<const uint8_t*>(src_ptr);
	uint8_t* dst_list = static_cast<uint8_t*>(dst_ptr);
	HooksBindingContext* ctx = static_cast<HooksBindingContext*>(type_info->hooks.binding_ctx);
	GFWorld* w = ctx->get_world();

	for (int i=0; i != count; i++) {
		const uint8_t* src = &src_list[i*type_info->size];
		uint8_t* dst = &dst_list[i*type_info->size];

		w->copy_component_ptr(
			static_cast<const void*>(src),
			static_cast<void*>(dst),
			type_info->component
		);
	}
}
void GFComponentBuilder::move(
	void* dst_ptr,
	void* src_ptr,
	int32_t count,
	const ecs_type_info_t* type_info
) {
	uint8_t* src_list = static_cast<uint8_t*>(src_ptr);
	uint8_t* dst_list = static_cast<uint8_t*>(dst_ptr);
	HooksBindingContext* ctx = static_cast<HooksBindingContext*>(
		type_info->hooks.binding_ctx
	);
	GFWorld* w = ctx->get_world();

	for (int i=0; i != count; i++) {
		uint8_t* src = &src_list[i*type_info->size];
		uint8_t* dst = &dst_list[i*type_info->size];

		w->copy_component_ptr(
			static_cast<void*>(src),
			static_cast<void*>(dst),
			type_info->component
		);
	}
}

GFWorld* HooksBindingContext::get_world() const { return Object::cast_to<GFWorld>(
	UtilityFunctions::instance_from_id(world_instance_id)
); }
