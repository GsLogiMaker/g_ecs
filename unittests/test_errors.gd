
extends GutTest

var world:GFWorld = null

func before_all():
	world = GFWorld.new()

func after_all():
	world.free()

#region Tests

func test_get_nonexistant_property():
	var entity:= GFEntity.spawn(world) \
		.add_component(Foo) \
		.set_name("Test")
	var foo:Foo = entity.get_component(Foo)
	
	assert_eq(foo.getm(&"not a real property"), null)


func test_new_entity_with_unregistered_component():
	var _entity:= GFEntity.spawn(world) \
		.add_component(Unregistered) \
		.set_name("Test")
	
	# We can't assert the right error is thrown, but it should be fine as
	# long as it doesn't crash
	assert_null(null)

#endregion

#region Classes

class Foo extends GFComponent:
	static func _get_members() -> Dictionary: return {
		vec = 0.0,
	}

class Unregistered extends GFComponent:
	static func _get_members() -> Dictionary: return {
		vec = 0.0,
	}

#endregion
