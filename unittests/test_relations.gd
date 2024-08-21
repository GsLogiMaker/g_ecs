
extends GutTest

var world:GFWorld

func before_all():
	world = GFWorld.new()

func after_all():
	world.free()

#region Tests

func test_bools():
	# TODO: Query for relations
	world.new_system().for_each(func(_delta): pass)
	var apple:= GFEntity.new(world) \
		.set_name("Apple")

	GFEntity.new(world).set_name("Eats")

	var man:= GFEntity.new(world) \
		.set_name("Man") \
		.add_relation("Eats", apple)

	var cow:= GFEntity.new(world) \
		.set_name("Cow")
	var grass:= GFEntity.new(world) \
		.set_name("Grass")
	cow.add_relation("Eats", grass)

	world.progress(0.0)

	assert_eq(true, true)

	man.free()
	apple.free()
#endregion

#region Components

class Bools extends GFComponent:
	func _build(b_: GFComponentBuilder) -> void:
		b_.add_member("a", TYPE_BOOL)
		b_.add_member("b", TYPE_BOOL)

#endregion
