
@tool
extends GutTest

var world:GFWorld

func before_all():
	world = GFWorld.new()

func after_all():
	world.free()

#region Tests

func test_auto_register():
	GFEntity.spawn(world) \
		.add_component(load("res://components/a_component.gd"))

	assert_eq(
		world.get_child_entity(
			"Glecs/Scripts/components/a_component.gd"
		).is_alive(),
		true,
	)

func test_register_module():
	world.register(load("res://components/a_module.gd"))

	assert_eq(
		world.get_child_entity(
			"Glecs/Scripts/components/a_module.gd"
		).is_alive(),
		true,
	)
	assert_eq(
		world.get_child_entity(
			"Glecs/Scripts/components/a_module.gd/SubEntity"
		).is_alive(),
		true,
	)
	assert_eq(
		world.get_child_entity(
			"Glecs/Scripts/components/a_module.gd/b_module/MyBComponent"
		).is_alive(),
		true,
	)

func test_register_component_script():
	world.register(load("res://components/a_component.gd"))

	assert_eq(
		world.get_child_entity(
			"Glecs/Scripts/components/a_component.gd"
		).is_alive(),
		true,
	)

func test_register_entity_script():
	world.register(load("res://components/a_entity.gd"))

	assert_eq(
		world.get_child_entity(
			"Glecs/Scripts/components/a_entity.gd"
		).is_alive(),
		true,
	)

func test_register_sub_class():
	world.register(Foo, "Foo")
	world.register(Bar, "Bar")

	assert_eq(
		world.get_child_entity(
			"Glecs/Scripts/Foo"
		).is_alive(),
		true,
	)
	assert_eq(
		world.get_child_entity(
			"Glecs/Scripts/Bar"
		).is_alive(),
		true,
	)

#func test_name_conflic():
	#world.register(Foo, "Foo")
	#world.register(Bar, "Foo")
#
	#assert_eq(
		#world.get_child_entity(
			#"Glecs/Scripts/Foo"
		#).is_alive(),
		#true,
	#)

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
