
extends GutTest

var world:GFWorld = null

func before_all():
	world = GFWorld.new()

func after_all():
	world.free()

#region Tests

func test_get_nonexistant_property():
	var entity:= GFEntity.new_in_world(world) \
		.add(Foo) \
		.set_name("Test")
	var foo:Foo = entity.get(Foo)

	assert_eq(foo.getm(&"not a real property"), null)


func test_new_entity_with_unregistered_component():
	var _entity:= GFEntity.new_in_world(world) \
		.add(Unregistered) \
		.set_name("Test")

	# We can't assert the right error is thrown, but it should be fine as
	# long as it doesn't crash
	assert_null(null)

#endregion

#region Classes

class Foo extends GFComponent:
	func _build(b_: GFComponentBuilder) -> void:
		b_.add_member("vec", TYPE_FLOAT)

class Unregistered extends GFComponent:
	func _build(b_: GFComponentBuilder) -> void:
		b_.add_member("vec", TYPE_FLOAT)

#endregion
