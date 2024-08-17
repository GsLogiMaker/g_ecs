
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
	assert_eq(first.is_valid(), true)
	assert_eq(second.is_valid(), true)

	var pair:= w.pair(first, second)
	var p:= GFEntity.from(w.pair(first, second), w)
	assert_eq(p.is_valid(), true)

	
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
	
	assert_eq(e.is_valid(), true)
	assert_eq(foo.is_valid(), true)
	assert_eq(e2.is_valid(), true)
	assert_eq(foo2.is_valid(), true)
	
	e2.free()
	assert_eq(e.is_valid(), true)
	assert_eq(foo.is_valid(), true)
	assert_eq(e2.is_valid(), false)
	assert_eq(foo2.is_valid(), false)
	
	foo.free()
	assert_eq(e.is_valid(), true)
	assert_eq(foo.is_valid(), false)
	assert_eq(e2.is_valid(), false)
	assert_eq(foo2.is_valid(), false)
	
	w.free()
	assert_eq(is_instance_valid(w), false)
	assert_eq(e.is_valid(), false)
	assert_eq(foo.is_valid(), false)
	
func test_simple_system():
	world.new_system() \
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
	static func _get_members() -> Dictionary: return {
		vec = 0.0,
	}
