
extends GFComponent

const DEFINE:= {
	dict = TYPE_DICTIONARY,
}

func get_dict() -> Dictionary:
	return getm(&"dict")

func set_dict(v:Dictionary) -> void:
	return setm(&"dict", v)

class OtherClass extends GFComponent: pass
