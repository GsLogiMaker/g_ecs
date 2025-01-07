
class_name GFRigidBody2D extends GFComponent

func get_rid() -> RID: return getm("rid")
func set_rid(rid:RID) -> void: setm("rid", rid)

func _build(b:GFComponentBuilder) -> void: b \
	.add_member("rid", TYPE_RID)
