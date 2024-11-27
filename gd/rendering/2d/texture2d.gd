
## A component containing a [Texture2D].

class_name GFTexture2D extends GFComponent

func get_texture() -> Texture2D: return getm(&"texture")
func set_texture(v:Texture2D) -> void: return setm(&"texture", v)
func _build(b: GFComponentBuilder) -> void:
	b.add_member("texture", TYPE_OBJECT)

static func update_texture_rect(sprite:GFTexture2D, item:GFCanvasItem) -> void:
	var texture:= sprite.get_texture()
	
	if texture == null:
		return
	
	RenderingServer.canvas_item_add_texture_rect(
		item.get_rid(),
		Rect2(-texture.get_size() / 2, texture.get_size()),
		texture,
	)
