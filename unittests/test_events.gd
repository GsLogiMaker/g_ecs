
extends GutTest

var world:GFWorld = null

func before_each():
	world = GFWorld.new()

func after_each():
	world.free()

#region Tests

func test_on_add_event():
	var data:= {i=0}

	GFObserverBuilder.new_in_world(world) \
		.set_events("flecs/core/OnAdd") \
		.with(Ints) \
		.for_each(func(_ints: Ints):
			data.i += 1
			)

	var e:= GFEntity.new_in_world(world) \
		.add_component(Ints) \
		.set_name("WithInts")
	var e2:= GFEntity.new_in_world(world) \
		.set_name("WithoutInts")
	var e3:= GFEntity.new_in_world(world) \
		.set_name("WithInts")
	var e4:= GFEntity.new_in_world(world) \
		.set_name("WithoutInts")

	e3.add_component(Ints)

	assert_eq(data.i, 2)

	e.delete()
	e2.delete()
	e3.delete()
	e4.delete()

# test events on_set_event
func test_on_set_event():
	var data:= {i=0}
	GFObserverBuilder.new_in_world(world) \
		.set_events("flecs/core/OnSet") \
		.with(Ints) \
		.for_each(func(ints: Ints):
			data.i += ints.a + ints.b
			)

	var e:= GFEntity.new_in_world(world) \
		.add_component(Ints, 2, 31) \
		.set_name("WithInts")
	var e2:= GFEntity.new_in_world(world) \
		.set_name("WithoutInts")
	var e3:= GFEntity.new_in_world(world) \
		.set_name("WithInts")
	var e4:= GFEntity.new_in_world(world) \
		.set_name("WithoutInts")

	e3.add_component(Ints, 99, 2)

	assert_eq(data.i, 2 + 31 + 99 + 2)

	e.delete()
	e2.delete()
	e3.delete()
	e4.delete()


func test_on_add_event_with_objects():
	var data:= {i=0}
	GFObserverBuilder.new_in_world(world) \
		.set_events("flecs/core/OnAdd") \
		.with(Textures) \
		.for_each(func(_ints: Textures):
			data.i += 1
			)

	var e:= GFEntity.new_in_world(world) \
		.add_component(Textures) \
		.set_name("WithInts")
	assert_eq(data.i, 1)
	assert_eq(e.get_component(Textures).a, null)

	e.delete()

	# In this test, the loaded textures will be auto freed by Godot if Glecs
	# does not properly take ownership of them.
	data.i = 0
	var e2:= GFEntity.new_in_world(world) \
		.set_name("WithTextures")
	e2.add_component(Textures, load("res://icon.png"))
	assert_eq(data.i, 1)
	assert_eq(e2.get_component(Textures).a, load("res://icon.png"))

	e2.delete()

func test_user_signal() -> void:
	var Update = GFEntity.new_in_world(world) \
		.set_name("Update")
	
	var data:= {i = 0}
	GFObserverBuilder.new_in_world(world) \
		.set_events(Update) \
		.with(Ints) \
		.for_each(func(ints:Ints):
			data.i += 1
			)
	
	var e:= GFEntity.new_in_world(world) \
		.add_component(Ints)
	# Observer shouldn't match against this entity
	var e2:= GFEntity.new_in_world(world) \
		.add_component(Ints)
	
	Update.emit(e, [Ints])
	
	assert_eq(data.i, 1, "Expected observer to match an `Update` event once")

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
	var a:Texture2D:
		get: return getm(&"a")
		set(v): setm(&"a", v)

#endregion
