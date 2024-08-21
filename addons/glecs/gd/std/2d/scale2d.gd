
## A component that represents scale in 2D space.

extends GFComponent

const Self:= preload("./scale2d.gd")
const CanvasItemC:= preload("./canvas_item.gd")
const Position2DC:= preload("./position2d.gd")
const Rotation2DC:= preload("./rotation2d.gd")

func _build(b: GFComponentBuilder) -> void:
	b.add_member("scale", TYPE_VECTOR2)
func get_scale() -> Vector2: return getm(&"scale")
func set_scale(v:Vector2) -> void: return setm(&"scale", v)

func _register(w:GFWorld):
	# On Scale2DC set, update visual transform of CanvasItemC
	w.observer_builder("flecs/core/OnSet") \
		.with(CanvasItemC).access_filer() \
		.with(Self) \
		.maybe_with(Position2DC) \
		.maybe_with(Rotation2DC) \
		.for_each(func(item:CanvasItemC, scal:Self, pos:Position2DC, rot:Rotation2DC):
			item.update_transform_c(pos, rot, scal)
			)
