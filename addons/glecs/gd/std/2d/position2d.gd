
## A component that represents a position in 2D space.

extends GFComponent

const Self:= preload("./position2d.gd")
const CanvasItemC:= preload("./canvas_item.gd")
const Rotation2DC:= preload("./rotation2d.gd")
const Scale2DC:= preload("./scale2d.gd")

func _build(b: GFComponentBuilder) -> void:
	b.add_member("vec", TYPE_VECTOR2)
func get_vec() -> Vector2: return getm(&"vec")
func set_vec(v:Vector2) -> void: return setm(&"vec", v)
func get_x() -> float: return getm(&"vec").x
func set_x(v:float) -> void: return setm(&"vec", Vector2(v, get_y()))
func get_y() -> float: return getm(&"vec").y
func set_y(v:float) -> void: return setm(&"vec", Vector2(get_x(), v))

static func _registered(w:GFWorld):
	# On Position2D set, update visual transform of CanvasItemC
	w.observer_builder("flecs/core/OnSet") \
		.with(CanvasItemC).access_filter() \
		.with(Self) \
		.maybe_with(Rotation2DC) \
		.maybe_with(Scale2DC) \
		.for_each(func(item:CanvasItemC, pos:Self, rot:Rotation2DC, scl:Scale2DC):
			item.update_transform_c(pos, rot, scl)
			)
