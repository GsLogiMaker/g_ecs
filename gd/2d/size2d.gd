
## A component that represents a size in 2D space.

class_name GFSize2D extends GFComponent

func get_size() -> Vector2: return getm(&"size")
func set_size(v:Vector2) -> void: return setm(&"size", v)
func get_x() -> float: return getm(&"size").x
func set_x(v:float) -> void: return setm(&"size", Vector2(v, get_y()))
func get_y() -> float: return getm(&"size").y
func set_y(v:float) -> void: return setm(&"size", Vector2(get_x(), v))
func _build(b: GFComponentBuilder) -> void:
	b.add_member("size", TYPE_VECTOR2)
