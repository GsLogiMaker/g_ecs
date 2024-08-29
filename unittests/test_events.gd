
extends GutTest

var world:GFWorld
var i:= 0

func before_all():
	world = GFWorld.new()

func after_all():
	world.free()

#region Tests

func test_on_add_event():
	i = 0

	world.observer_builder("flecs/core/OnAdd") \
		.with(Ints) \
		.for_each(func(_ints: Ints):
			self.i += 1
			)

	var e:= GFEntity.spawn(world) \
		.add_component(Ints) \
		.set_name("WithInts")
	var e2:= GFEntity.spawn(world) \
		.set_name("WithoutInts")
	var e3:= GFEntity.spawn(world) \
		.set_name("WithInts")
	var e4:= GFEntity.spawn(world) \
		.set_name("WithoutInts")

	e3.add_component(Ints)

	assert_eq(i, 2)

	e.free()
	e2.free()
	e3.free()
	e4.free()


func test_on_init_event():
	i = 0

	world.observer_builder("glecs/core/on_init") \
		.with(Ints) \
		.for_each(func(ints: Ints):
			self.i += ints.a + ints.b
			)

	var e:= GFEntity.spawn(world) \
		.add_component(Ints, [2, 31]) \
		.set_name("WithInts")
	var e2:= GFEntity.spawn(world) \
		.set_name("WithoutInts")
	var e3:= GFEntity.spawn(world) \
		.set_name("WithInts")
	var e4:= GFEntity.spawn(world) \
		.set_name("WithoutInts")

	e3.add_component(Ints, [99, 2])

	assert_eq(i, 2 + 31 + 99 + 2)

	e.free()
	e2.free()
	e3.free()
	e4.free()


func test_on_set_event():
	i = 0
	var w:= GFWorld.new()

	w.observer_builder("flecs/core/OnSet") \
		.with(Ints) \
		.for_each(func(ints:Ints):
			self.i += ints.a + ints.b
			)

	var e:= GFEntity.spawn(w) \
		.add_component(Ints, [2, 6])
	var ints_c:Ints = e.get_component(Ints)

	ints_c.a = 15
	ints_c.b = 45

	assert_eq(i, (2 + 6) + (15 + 6) + (15 + 45))


func test_on_add_event_with_objects():
	i = 0
	world.observer_builder("flecs/core/OnAdd") \
		.with(Textures) \
		.for_each(func(_ints: Textures):
			self.i += 1
			)

	var e:= GFEntity.spawn(world) \
		.add_component(Textures) \
		.set_name("WithInts")
	assert_eq(i, 1)
	assert_eq(e.get_component(Textures).a, null)
	assert_eq(e.get_component(Textures).b, null)

	e.free()

	# In this test, the loaded textures will be auto freed by Godot if Glecs
	# does not properly take ownership of them.
	i = 0
	var e2:= GFEntity.spawn(world) \
		.set_name("WithTextures")
	e2.add_component(Textures, [load("res://icon.png"), load("res://icon.svg")])
	assert_eq(i, 1)
	assert_eq(e2.get_component(Textures).a, load("res://icon.png"))
	assert_eq(e2.get_component(Textures).b, load("res://icon.svg"))

	e2.free()

#endregion

#region Components

class Ints extends GFComponent:
	func _build(b:GFComponentBuilder) -> void:
		b.add_member("a", TYPE_INT)
		b.add_member("b", TYPE_INT)
	var a:int:
		get: return getm(&"a")
		set(v): setm(&"a", v)
	var b:int = 25:
		get: return getm(&"b")
		set(v): setm(&"b", v)

class Textures extends GFComponent:
	func _build(b:GFComponentBuilder) -> void:
		b.add_member("a", TYPE_OBJECT)
		b.add_member("b", TYPE_OBJECT)
	var a:Texture2D:
		get: return getm(&"a")
		set(v): setm(&"a", v)
	var b:Texture2D:
		get: return getm(&"b")
		set(v): setm(&"b", v)

#endregion
