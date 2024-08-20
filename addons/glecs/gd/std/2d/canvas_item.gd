
## A component that gives entities functionality of a CanvasItem.

extends GFComponent

const Self:= preload("./canvas_item.gd")
const Position2DC:= preload("./position2d.gd")
const Rotation2DC:= preload("./rotation2d.gd")
const Scale2DC:= preload("./scale2d.gd")

func _build(b: GFComponentBuilder) -> void:
	b.add_member("rid", TYPE_RID)

func get_rid() -> RID: return getm(&"rid")
func set_rid(v:RID) -> void: setm(&"rid", v)

## Sets the parent canvas item of this canvas item by its [RID].
func set_parent_canvas_item(rid:RID) -> void:
	RenderingServer.canvas_item_set_parent(
		get_rid(),
		rid
	)

## Updates the transform of this canvas item via the three 2D spatial
## components.
func update_transform_c(pos:Position2DC, rot:Rotation2DC, scale:Scale2DC) -> void:
	var loc:Vector2 = pos.get_vec() if pos else Vector2()
	var angle:float = rot.get_angle() if rot else 0.0
	var size:Vector2 = scale.get_scale() if scale else Vector2.ONE
	RenderingServer.canvas_item_set_transform(
		get_rid(),
		Transform2D(angle, size, 0, loc)
		)

static func _registered(w:GFWorld):
	# On add
	w.observer_builder("flecs/core/OnAdd") \
		.with(Self) \
		.for_each(func(item:Self):
			var rid:= RenderingServer.canvas_item_create()
			item.set_rid(rid)
			)

	# On set
	w.observer_builder("flecs/core/OnSet") \
		.with(Self) \
		.for_each(func(item:Self):
			item.set_parent_canvas_item(
				Engine.get_main_loop().current_scene.get_canvas_item()
				)
			)

	# On remove
	w.observer_builder("flecs/core/OnRemove") \
		.with(Self) \
		.for_each(func(item:Self):
			RenderingServer.free_rid(item.get_rid())
			)
