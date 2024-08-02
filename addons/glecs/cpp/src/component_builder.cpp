

#include "component_builder.h"
#include "world.h"
#include "utils.h"

#include <flecs.h>

using namespace godot;

GlComponentBuilder::GlComponentBuilder() {
	component_desc = {0};
	struct_desc = {0};
	member_names = Array();
	world = {0};
	built = {0};
}
GlComponentBuilder::~GlComponentBuilder() {
}

Ref<GlComponentBuilder> GlComponentBuilder::add_member(
	String member,
	Variant::Type type
) {
	const char* ERR_ADD_COMPONENT = "Failed to add member to component builder\n";

	if (get_member_count() == ECS_MEMBER_DESC_CACHE_SIZE) {
		ERR(Ref(this),
			ERR_ADD_COMPONENT,
			"Max member count reached"
		);
	}

	EntityResult ecs_type_result = Utils::variant_type_to_id(type);
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

int GlComponentBuilder::get_member_count() {
	return member_names.size();
}

Ref<GlComponentBuilder> GlComponentBuilder::set_name(
	String name_
) {
	name = name_;
	return Ref(this);
}

void GlComponentBuilder::build() {
	const char* FAILED_TO_BUILD = "Failed to build component\n";
	if (built) {
		ERR(/**/,
			FAILED_TO_BUILD,
			"Component builder was already built"
		);
	}
	built = true;

	// Set names to temporary pointers
	CharString name_utf8 = name.utf8();
	CharString member_names_utf8[ECS_MEMBER_DESC_CACHE_SIZE] = {0};
	component_desc.type.name = name_utf8.ptr();
	for (int i=0; i != get_member_count(); i++) {
		member_names_utf8[i] = String(member_names[i]).utf8();
		struct_desc.members[i].name = member_names_utf8[i].ptr();
	}

	ecs_world_t* raw = world->raw();

	// Create component entity
	ecs_entity_t component_id = ecs_new_from_path(raw, 0, component_desc.type.name);
	component_desc.entity = component_id;
	struct_desc.entity = component_id;

	ecs_entity_t struct_id = ecs_struct_init(raw, &struct_desc);

	// Set size of component
	const EcsStruct* struct_data = ecs_get(raw, struct_id, EcsStruct);
	const ecs_member_t* final_member = ecs_vec_last_t(
		&struct_data->members,
		ecs_member_t
	);
	component_desc.type.size = final_member->offset + final_member->size;
	component_desc.type.alignment = 8;

	ecs_component_init(raw, &component_desc);
}

void GlComponentBuilder:: set_world(GlWorld* world_) {
	world = world_;
}

// **********************************************
// *** PROTECTED ***
// **********************************************

void GlComponentBuilder::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("add_member", "member", "type"), &GlComponentBuilder::add_member);
	godot::ClassDB::bind_method(D_METHOD("set_name", "name"), &GlComponentBuilder::set_name);
	godot::ClassDB::bind_method(D_METHOD("build"), &GlComponentBuilder::build);

}

// **********************************************
// *** PRIVATE ***
// **********************************************
