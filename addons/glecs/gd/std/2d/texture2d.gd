
## A component containing a [Texture2D].

extends GFComponent

const Self:= preload("./texture2d.gd")
const CanvasItemComponent:= preload("./canvas_item.gd")
const Render:= preload("../rendering/render.gd")

func _build(b: GFComponentBuilder) -> void:
	b.add_member("texture", TYPE_OBJECT)
func get_texture() -> Texture2D: return getm(&"texture")
func set_texture(v:Texture2D) -> void: return setm(&"texture", v)

func _register(w:GFWorld):
	# On Render-Texture set with CanvasItemComponent
	w.observer_builder("flecs/core/OnSet") \
		.with(w.pair(Render, Self)) \
		.with(CanvasItemComponent).access_filter() \
		.for_each(func(ctexture:Self, item:CanvasItemComponent):
			var texture:= ctexture.get_texture()
			RenderingServer.canvas_item_add_texture_rect(
				item.get_rid(),
				Rect2(-texture.get_size() / 2, texture.get_size()),
				texture,
				)
			)
