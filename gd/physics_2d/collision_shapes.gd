
class_name GFCollisionShapes extends GFComponent

func get_shapes() -> Array: return getm("shapes")
func set_shapes(v:Array[RID]) -> void: setm("shapes", v)

func _build(b:GFComponentBuilder) -> void: b \
	.add_member("shapes", TYPE_ARRAY)
