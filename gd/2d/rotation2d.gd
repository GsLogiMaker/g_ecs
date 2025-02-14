
## A component that represents a rotaition in 2D space.

class_name GFRotation2D extends GFComponent

func get_angle() -> float: return getm(&"angle")
func set_angle(v:float) -> void: return setm(&"angle", v)
func _build(b: GFComponentBuilder) -> void:
	b.add_member("angle", TYPE_FLOAT)
