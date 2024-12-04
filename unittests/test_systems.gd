
extends GutTest

var world:GFWorld

func before_each():
	world = GFWorld.new()
	# world.new_pipeline(&"test_pipeline")

func after_each():
	world.free()

#region Tests

func test_stuff():
	pass
	#world.system_builder(&"test_pipeline") \
	GFSystemBuilder.new_in_world(world) \
		.with(Bools) \
		.for_each(func(boo:Bools):
			boo.b = boo.a
			)

	var e:= GFEntity.new_in_world(world) \
		.add_component(Bools) \
		.set_name("Test")
	world.register_script(Bools)
	var bools = e.get_component(Bools)
	bools.a = true
	bools.b = false

	assert_eq(bools.a, true)
	assert_eq(bools.b, false)

	# world.run_pipeline(&"test_pipeline", 1.0)
	world.progress(1.0)

	assert_eq(bools.a, true)
	assert_eq(bools.b, true)


#endregion

#region Components

class Bools extends GFComponent:
	func _build(builder: GFComponentBuilder) -> void:
		builder.add_member("a", TYPE_BOOL)
		builder.add_member("b", TYPE_BOOL)
	var a:bool:
		get: return getm(&"a")
		set(v): setm(&"a", v)
	var b:bool:
		get: return getm(&"b")
		set(v): setm(&"b", v)

#endregion
