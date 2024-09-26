
extends GutTest

var world:GFWorld = null

func before_all():
	world = GFWorld.new()

func after_all():
	world.free()

#region Tests

func test_component_get_and_set():
	var e:GFEntity = GFEntity.spawn(world) \
		.add_component(Foo) \
		.set_name("Test")

	var foo:Foo = e.get_component(Foo)
	assert_almost_eq(foo.value, 0.0, 0.01)

	foo.value = 2.3
	assert_almost_eq(foo.value, 2.3, 0.01)

	e.delete()

func test_component_string_get_and_set():
	var e:= GFEntity.spawn(world) \
		.add_component(Stringy) \
		.set_name("Test")

	var foo:Stringy = e.get_component(Stringy)
	foo.a = "po"
	foo.b = "em"
	assert_eq(foo.a, "po")

	foo.a += foo.b

	assert_eq(foo.a, "poem")
	assert_eq(foo.b, "em")

func test_new_entity_with_unregistered_component():
	var e:GFEntity = GFEntity.spawn(world) \
		.add_component(Unregistered) \
		.set_name("Test")
	assert_eq(e.get_component(Unregistered).value, 0)

func test_creating_entity_by_new():
	# Test that an entity is invalidated by being deleted
	var e:= GFEntity.spawn(world)
	assert_eq(e.is_alive(), true)
	e.delete()
	assert_eq(e.is_alive(), false)

	# Test that an entity is invalidated by its world being deleted
	var w:= GFWorld.new()
	var e2:= GFEntity.spawn(w)
	assert_eq(e2.is_alive(), true)
	w.free()
	assert_eq(e2.is_alive(), false)

func test_entity_from():
	var id:= 0
	if true:
		var tmp_entity = GFEntity.spawn(world)
		tmp_entity.set_name(&"Cool Name")
		id = tmp_entity.get_id()
	assert_ne(id, 0)

	var e:= GFEntity.from(id, world)
	assert_ne(e, null)
	assert_eq(e.get_name(), &"Cool Name")

func test_entity_created_in_singleton():
	var e:= GFEntity.spawn()
	assert_eq(e.is_alive(), true)

	var e2:= GFEntity.spawn(GFGlobalWorld)
	assert_eq(e2.is_alive(), true)

	assert_eq(e.get_world(), e2.get_world())

	e.delete()
	e2.delete()

#endregion

#region Classes

class Foo extends GFComponent:
	func _build(b_: GFComponentBuilder) -> void:
		b_.add_member("value", TYPE_FLOAT)
	var value:float:
		get: return getm(&"value")
		set(v): setm(&"value", v)

class Stringy extends GFComponent:
	func _build(b_: GFComponentBuilder) -> void:
		b_.add_member("a", TYPE_STRING)
		b_.add_member("b", TYPE_STRING)
	var a:String:
		get: return getm(&"a")
		set(v): setm(&"a", v)
	var b:String:
		get: return getm(&"b")
		set(v): setm(&"b", v)

class Unadded extends GFComponent:
	func _build(b_: GFComponentBuilder) -> void:
		b_.add_member("value", TYPE_INT)
	var value:int:
		get: return getm(&"value")
		set(v): setm(&"value", v)

class Unregistered extends GFComponent:
	func _build(b_: GFComponentBuilder) -> void:
		b_.add_member("value", TYPE_INT)
	var value:int:
		get: return getm(&"value")
		set(v): setm(&"value", v)

#endregion
