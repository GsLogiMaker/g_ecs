
extends GutTest

const TEXTURE:= preload("res://icon.png")

var world:GFWorld = null
var _old_world:GFWorld = null

func before_each():
	world = GFWorld.new()
	_old_world = GFWorld.get_local_thread_singleton()
	GFWorld.set_local_thread_singleton(world)

func after_each():
	GFWorld.set_local_thread_singleton(_old_world)
	world.free()

#region Tests

#endregion
