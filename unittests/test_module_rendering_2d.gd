
extends GutTest

const TEXTURE:= preload("res://icon.png")

var world:GFWorld = null

func before_each():
	world = GFWorld.new()

func after_each():
	world.free()

#region Tests

#endregion
