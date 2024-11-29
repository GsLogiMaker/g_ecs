
## Registers default Glecs modules with new worlds.

@tool
class_name _GlecsModules extends Node

const MODULES:= [
	preload("./rendering/rendering.gd"),
]

static var _register_self_singleton = (func():
	Engine.register_singleton("_glecs_modules", _GlecsModules)
	).call()

static func register_modules(w:GFWorld) -> void:
	for module in MODULES:
		w.register_script(module)
