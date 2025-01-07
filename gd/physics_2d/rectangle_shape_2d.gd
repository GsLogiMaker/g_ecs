
class_name GFRectangleShape2D extends GFComponent

func get_rid() -> RID: return getm("rid")
func set_rid(v:RID) -> void: setm("rid", v)
func get_size() -> Vector2: return getm("size")
func set_size(v:Vector2) -> void: setm("size", v)

func _build(b:GFComponentBuilder) -> void: b \
	.add_member("rid", TYPE_RID) \
	.add_member("size", TYPE_VECTOR2) \
