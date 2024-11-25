
extends RefCounted

const DictionaryC:= preload("./dictionary.gd")

class Positional2D extends GFComponent:
	var transform:= Transform2D.IDENTITY

class Motional2D extends GFComponent:
	var velocity:= Vector2.ZERO

class List extends GFComponent:
	var elements:= {}

class MyEntity extends GFEntity: pass

static func move_position(positional:Positional2D, motional:Motional2D) -> void:
	positional.transform.origin += motional.velocity
