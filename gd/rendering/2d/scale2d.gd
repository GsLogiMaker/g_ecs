
## A component that represents scale in 2D space.

class_name GFScale2D extends GFComponent

func get_scale() -> Vector2: return getm(&"scale")
func set_scale(v:Vector2) -> void: return setm(&"scale", v)
func _build(b: GFComponentBuilder) -> void:
	b.add_member("scale", TYPE_VECTOR2)
