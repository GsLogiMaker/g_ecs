
## A component that represents a position in 2D space.

class_name GFPosition2D extends GFComponent

func get_vec() -> Vector2: return getm(&"vec")
func set_vec(v:Vector2) -> void: return setm(&"vec", v)
func get_x() -> float: return getm(&"vec").x
func set_x(v:float) -> void: return setm(&"vec", Vector2(v, get_y()))
func get_y() -> float: return getm(&"vec").y
func set_y(v:float) -> void: return setm(&"vec", Vector2(get_x(), v))
func _build(b: GFComponentBuilder) -> void:
	b.add_member("vec", TYPE_VECTOR2)
