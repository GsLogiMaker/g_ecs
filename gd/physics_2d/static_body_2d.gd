
class_name GFStaticBody extends GFRegisterableEntity

func _register(world: GFWorld) -> void:
	add_tag("/root/flecs/core/Target")
	add_tag("/root/flecs/core/Exclusive")
