
extends GutTest

var world: GFWorld = null

func before_all():
	world = GFWorld.new()

func after_all():
	world.free()

func test_add_entity():
	var _entity:= GFEntity.spawn(world) \
		.set_name("Test")

	# Can't assert, but should be fine as long as it doesn't crash
	assert_null(null)


func test_pairs_are_alive():
	var w:= GFWorld.new()
	var first:= GFEntity.spawn(w)
	var second:= GFEntity.spawn(w)
	assert_eq(first.is_alive(), true)
	assert_eq(second.is_alive(), true)

	var pair:= w.pair(first, second)
	var p:= GFEntity.from(w.pair(first, second), w)
	assert_eq(p.is_alive(), true)


func test_world_deletion():
	var w:= GFWorld.new()

	var e:= GFEntity.spawn(w) \
		.add_component(Foo) \
		.set_name("Test")
	var foo:Foo = e.get_component(Foo)

	var e2:= GFEntity.spawn(w) \
		.add_component(Foo) \
		.set_name("Test")
	var foo2:Foo = e2.get_component(Foo)

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
	world.system_builder() \
		.with(Foo) \
		.for_each(func(_delta:float, foo:Foo):
			foo.setm(&"vec", 2.67)
			)

	var entity:= GFEntity.spawn(world) \
		.add_component(Foo) \
		.set_name("Test")

	world.progress(0.0)

	assert_almost_eq(entity.get_component(Foo).getm(&"vec"), 2.67, 0.01)

class Foo extends GFComponent:
	func _build(b: GFComponentBuilder) -> void:
		b.add_member("vec", TYPE_FLOAT)
