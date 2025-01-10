
extends GutTest

var world:GFWorld = null

func before_each():
	world = GFWorld.new()

func after_each():
	world.free()

#region Tests

func _test_tag_creation():
	var enemy:= GFTag.new_in_world(world)
	var robot:=GFEntity.new_in_world(world) \
		.add(enemy)
	
	assert_true(robot.has_entity(enemy), "Expected `robot` to have the `enemy` tag")

#endregion

#region Classes

class Eats extends GFTag: pass

class Grass extends GFTag: pass

#endregion
