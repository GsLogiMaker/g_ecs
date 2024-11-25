
#ifndef GDEXAMPLE_REGISTER_TYPES_H
#define GDEXAMPLE_REGISTER_TYPES_H

#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0601 // Support Windows 7+

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void initialize_module(ModuleInitializationLevel p_level);
void uninitialize_module(ModuleInitializationLevel p_level);

#endif // GDEXAMPLE_REGISTER_TYPES_H
