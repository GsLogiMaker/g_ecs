
#include "component.h"
#include "component_builder.h"
#include "doc_data.gen.h"
#include "entity.h"
#include "gdextension_interface.h"
#include "godot_cpp/classes/engine.hpp"
#include "query.h"
#include "query_builder.h"
#include "query_iterator.h"
#include "querylike_builder.h"
#include "register_types.h"
#include "registerable_entity.h"
#include "system_builder.h"
#include "world.h"

using namespace godot;

void initialize_module(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
		GDExtensionsInterfaceEditorHelpLoadXmlFromUtf8CharsAndLen editor_help_load_xml_from_utf8_chars_and_len = (GDExtensionsInterfaceEditorHelpLoadXmlFromUtf8CharsAndLen)internal::gdextension_interface_get_proc_address("editor_help_load_xml_from_utf8_chars_and_len");
		editor_help_load_xml_from_utf8_chars_and_len(_doc_data, _doc_data_size);
	}
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	godot::ClassDB::register_class<GlWorld>();
	godot::ClassDB::register_abstract_class<GlEntity>();
	godot::ClassDB::register_abstract_class<GlRegisterableEntity>();
	godot::ClassDB::register_abstract_class<GlComponent>();
	godot::ClassDB::register_abstract_class<GlComponentBuilder>();
	godot::ClassDB::register_abstract_class<GlQuerylikeBuilder>();
	godot::ClassDB::register_abstract_class<GlQueryBuilder>();
	godot::ClassDB::register_abstract_class<GlQuery>();
	godot::ClassDB::register_abstract_class<GlQueryIterator>();
	godot::ClassDB::register_abstract_class<GlSystemBuilder>();

	Engine::get_singleton()->register_singleton("GlGlobalWorld", memnew(GlWorld));
}

void uninitialize_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	Engine::get_singleton()->unregister_singleton("GlGlobalWorld");
}

extern "C" {
// Initialization.
GDExtensionBool GDE_EXPORT library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
	godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

	init_obj.register_initializer(initialize_module);
	init_obj.register_terminator(uninitialize_module);
	init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

	return init_obj.init();
}
}
