
## A component that represents scale in 2D space.

extends GFComponent

const std:= preload("../std.gd")
const CanvasItemC:= std.CanvasItemC
const Position2DC:= std.Position2DC
const Rotation2DC:= std.Rotation2DC
const Scale2DC:= std.Scale2DC

func _build(b: GFComponentBuilder) -> void:
	b.add_member("scale", TYPE_VECTOR2)
func get_scale() -> Vector2: return getm(&"scale")
func set_scale(v:Vector2) -> void: return setm(&"scale", v)

func _register(w:GFWorld):
	# On Scale2DC set, update visual transform of CanvasItemC
	w.observer_builder("flecs/core/OnSet") \
		.with(CanvasItemC).access_filter() \
		.with(Scale2DC) \
		.maybe_with(Position2DC) \
		.maybe_with(Rotation2DC) \
		.for_each(func(item:CanvasItemC, scal:Scale2DC, pos:Position2DC, rot:Rotation2DC):
			item.update_transform_c(pos, rot, scal)
			)
