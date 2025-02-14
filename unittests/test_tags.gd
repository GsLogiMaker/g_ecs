
extends GutTest

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

func _test_tag_creation():
	var enemy:= GFTag.new()
	var robot:=GFEntity.new() \
		.add(enemy)
	
	assert_true(robot.has(enemy), "Expected `robot` to have the `enemy` tag")

#endregion

#region Classes

class Eats extends GFTag: pass

class Grass extends GFTag: pass

#endregion
