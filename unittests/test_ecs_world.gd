
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

func test_add_entity():
	var _entity:= GFEntity.new() \
		.set_name("Test")

	# Can't assert, but should be fine as long as it doesn't crash
	assert_null(null)


func test_pairs_are_alive():
	var first:= GFEntity.new()
	var second:= GFEntity.new()
	assert_eq(first.is_alive(), true)
	assert_eq(second.is_alive(), true)

	var pair:= world.pair(first, second)
	assert_eq(pair.is_alive(), true, "Expected pair to be alive. First: " + str(first) + ", Second: " + str(second) + ", Pair: " + str(pair))
	var p:= GFEntity.from(pair, world)
	assert_eq(p.is_alive(), true, "Expected pair to be alive. First: " + str(first) + ", Second: " + str(second) + ", Pair: " + str(p))


func test_world_deletion():
	var w:= GFWorld.new()

	var e:= GFEntity.new_in_world(w) \
		.add(Foo) \
		.set_name("Test")
	assert_eq(e.get_world(), w, "Expected entity to have been created in the local world")
	var foo:Foo = e.get(Foo)

	var e2:= GFEntity.new_in_world(w) \
		.add(Foo) \
		.set_name("Test")
	assert_eq(e2.get_world(), w, "Expected entity to have been created in the local world")
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
	GFSystemBuilder.new() \
		.with(Foo) \
		.for_each(func(foo:Foo):
			foo.setm(&"vec", 2.67)
			)

	var entity:= GFEntity.new() \
		.add(Foo) \
		.set_name("Test")

	world.progress(0.0)

	assert_almost_eq(entity.get(Foo).getm(&"vec"), 2.67, 0.01)

func test_entity_created_in_local_thread_world():
	var e:= GFEntity.new()
	assert_eq(e.get_world(), world)
	assert_eq(world, GFWorld.get_local_thread_singleton())


class Foo extends GFComponent:
	func _build(b: GFComponentBuilder) -> void:
		b.add_member("vec", TYPE_FLOAT)
