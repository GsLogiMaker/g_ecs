
extends GutTest

var world: GFWorld = null

func before_all():
	world = GFWorld.new()

func after_all():
	world.free()

func test_add_entity():
	var _entity:= GFEntity.new_in_world(world) \
		.set_name("Test")

	# Can't assert, but should be fine as long as it doesn't crash
	assert_null(null)


func test_pairs_are_alive():
	var w:= GFWorld.new()
	var first:= GFEntity.new_in_world(w)
	var second:= GFEntity.new_in_world(w)
	assert_eq(first.is_alive(), true)
	assert_eq(second.is_alive(), true)

	var pair:= w.pair(first, second)
	assert_eq(pair.is_alive(), true, "Expected pair to be alive. First: " + str(first) + ", Second: " + str(second) + ", Pair: " + str(pair))
	var p:= GFEntity.from(pair, w)
	assert_eq(p.is_alive(), true, "Expected pair to be alive. First: " + str(first) + ", Second: " + str(second) + ", Pair: " + str(p))


func test_world_deletion():
	var w:= GFWorld.new()

	var e:= GFEntity.new_in_world(w) \
		.add(Foo) \
		.set_name("Test")
	var foo:Foo = e.get(Foo)

	var e2:= GFEntity.new_in_world(w) \
		.add(Foo) \
		.set_name("Test")
	var foo2:Foo = e2.get(Foo)

	foo.setm(&"vec", 24.3)
	foo2.setm(&"vec", 125.1)

	assert_eq(e.is_alive(), true)
	assert_eq(foo.is_alive(), true)
	assert_eq(e2.is_alive(), true)
	assert_eq(foo2.is_alive(), true)

	e2.delete()
	assert_eq(e.is_alive(), true)
	assert_eq(foo.is_alive(), true)
	assert_eq(e2.is_alive(), false)
	assert_eq(foo2.is_alive(), false)

	foo.delete()
	assert_eq(e.is_alive(), true)
	assert_eq(foo.is_alive(), false)
	assert_eq(e2.is_alive(), false)
	assert_eq(foo2.is_alive(), false)

	w.free()
	assert_eq(is_instance_valid(w), false)
	assert_eq(e.is_alive(), false)
	assert_eq(foo.is_alive(), false)

func test_simple_system():
	GFSystemBuilder.new_in_world(world) \
		.with(Foo) \
		.for_each(func(foo:Foo):
			foo.setm(&"vec", 2.67)
			)

	var entity:= GFEntity.new_in_world(world) \
		.add(Foo) \
		.set_name("Test")

	world.progress(0.0)

	assert_almost_eq(entity.get(Foo).getm(&"vec"), 2.67, 0.01)

class Foo extends GFComponent:
	func _build(b: GFComponentBuilder) -> void:
		b.add_member("vec", TYPE_FLOAT)
