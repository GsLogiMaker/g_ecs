
extends Node2D

var world:= GFWorld()

var texture:= load("res://icon.png")

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	var e:= GFEntity.spawn().set_name("Test")
	e.add_component(RenderableSprite2D, [texture])

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	pass

class RenderableSprite2D extends GFComponent:
	static func _get_members() -> Dictionary: return {
		texture = null,
		rid = RID(),
	}

	func _register(world:GFWorld):
		# On add
		world.observer_builder("flecs/core/OnAdd") \
			.with(RenderableSprite2D) \
			.for_each(func(sprite:RenderableSprite2D):
				await Engine.get_main_loop().process_frame
				var texture:= sprite.get_texture()
				if sprite.get_texture() == null:
					return
				var rid:= RenderingServer.canvas_item_create()
				sprite.set_rid(rid)
				RenderingServer.canvas_item_set_parent(rid, Engine.get_main_loop().current_scene.get_canvas_item())
				RenderingServer.canvas_item_add_texture_rect(
					rid,
					Rect2(-texture.get_size() / 2, texture.get_size()),
					texture,
				)
				)

		# On set
		world.observer_builder("flecs/core/OnSet") \
			.with(RenderableSprite2D) \
			.for_each(func(sprite:RenderableSprite2D):
				var texture:= sprite.get_texture()
				var rid:= sprite.get_rid()
				RenderingServer.canvas_item_clear(rid)
				RenderingServer.canvas_item_add_texture_rect(
					rid,
					Rect2(-texture.get_size() / 2, texture.get_size()),
					sprite.get_texture(),
				)
				)

	func get_rid() -> RID:
		return getm(&"rid")
	func set_rid(v:RID) -> void:
		setm(&"rid", v)

	func get_texture() -> Texture2D:
		return getm(&"texture")
	func set_texture(v:Texture2D) -> void:
		setm(&"texture", v)
