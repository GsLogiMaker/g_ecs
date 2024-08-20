
## A component that represents a rotaition in 2D space.

extends GFComponent

const Self:= preload("./rotation2d.gd")
const CanvasItemC:= preload("./canvas_item.gd")
const Position2DC:= preload("./position2d.gd")
const Scale2DC:= preload("./scale2d.gd")

func _build(b: GFComponentBuilder) -> void:
	b.add_member("angle", TYPE_FLOAT)
func get_angle() -> float: return getm(&"angle")
func set_angle(v:float) -> void: return setm(&"angle", v)

func _register(w:GFWorld):
	# On Rotation2DC set, update visual transform of CanvasItemC
	w.observer_builder("flecs/core/OnSet") \
		.with(CanvasItemC).access_filter() \
		.with(Self) \
		.maybe_with(Position2DC) \
		.maybe_with(Scale2DC) \
		.for_each(func(item:CanvasItemC, rot:Self, pos:Position2DC, scl:Scale2DC):
			item.update_transform_c(pos, rot, scl)
			)
