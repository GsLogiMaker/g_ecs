
@tool
extends GutTest

var world:GFWorld

func before_all():
	world = GFWorld.new()

func after_all():
	world.free()

#region Tests

func test_auto_register_script():
	GFEntity.spawn(world) \
		.add_component(load("res://components/a_component.gd"))

	assert_ne(
		world.lookup(
			"Glecs/Scripts/components/a_component.gd"
		),
		null,
	)

func test_register_script_module():
	world.register_script(load("res://components/a_module.gd"))

	assert_ne(
		world.lookup(
			"Glecs/Scripts/components/a_module.gd"
		),
		null,
	)
	assert_ne(
		world.lookup(
			"Glecs/Scripts/components/a_module.gd/SubEntity"
		),
		null,
	)
	assert_ne(
		world.lookup(
			"Glecs/Scripts/components/a_module.gd/b_module/MyBComponent"
		),
		null,
	)

func test_register_script_component_script():
	world.register_script(load("res://components/a_component.gd"))

	assert_ne(
		world.lookup(
			"Glecs/Scripts/components/a_component.gd"
		),
		null,
	)

func test_register_script_entity_script():
	world.register_script(load("res://components/a_entity.gd"))

	assert_ne(
		world.lookup(
			"Glecs/Scripts/components/a_entity.gd"
		),
		null,
	)

func test_register_script_sub_class():
	world.register_script(Foo)
	world.register_script(Bar)

	assert_ne(
		world.lookup(
			"Glecs/Scripts/Foo"
		),
		null,
	)
	assert_ne(
		world.lookup(
			"Glecs/Scripts/Bar"
		),
		null,
	)

func test_name_conflic():
	GFEntity.spawn(world) \
		.set_name("Foo")
	world.register_script(Foo)

	assert_ne(
		world.lookup(
			"Foo"
		).is_alive(),
		null,
	)

#endregion

#region Components

class Foo extends GFComponent:
	func _build(b_: GFComponentBuilder) -> void:
		b_.add_member("a", TYPE_BOOL)
		b_.add_member("b", TYPE_INT)
		b_.add_member("c", TYPE_FLOAT)

class Bar extends GFComponent:
	func _build(b_: GFComponentBuilder) -> void:
		b_.add_member("a", TYPE_VECTOR2)
		b_.add_member("b", TYPE_FLOAT)

#endregion
