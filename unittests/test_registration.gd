
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
		).is_valid(),
		true,
	)

func test_register_module():
	world.register(load("res://components/a_module.gd"))

	assert_eq(
		world.get_child_entity(
			"Glecs/Scripts/components/a_module.gd"
		).is_valid(),
		true,
	)
	assert_eq(
		world.get_child_entity(
			"Glecs/Scripts/components/a_module.gd/SubEntity"
		).is_valid(),
		true,
	)
	assert_eq(
		world.get_child_entity(
			"Glecs/Scripts/components/a_module.gd/b_module/MyBComponent"
		).is_valid(),
		true,
	)

func test_register_component_script():
	world.register(load("res://components/a_component.gd"))

	assert_eq(
		world.get_child_entity(
			"Glecs/Scripts/components/a_component.gd"
		).is_valid(),
		true,
	)

func test_register_entity_script():
	world.register(load("res://components/a_entity.gd"))

	assert_eq(
		world.get_child_entity(
			"Glecs/Scripts/components/a_entity.gd"
		).is_valid(),
		true,
	)

func test_register_sub_class():
	world.register(Foo, "Foo")
	world.register(Bar, "Bar")

	assert_eq(
		world.get_child_entity(
			"Glecs/Scripts/Foo"
		).is_valid(),
		true,
	)
	assert_eq(
		world.get_child_entity(
			"Glecs/Scripts/Bar"
		).is_valid(),
		true,
	)

#func test_name_conflic():
	#world.register(Foo, "Foo")
	#world.register(Bar, "Foo")
#
	#assert_eq(
		#world.get_child_entity(
			#"Glecs/Scripts/Foo"
		#).is_valid(),
		#true,
	#)

#endregion

#region Components

class Foo extends GFComponent:
	static func _get_members() -> Dictionary: return {
		a = false,
		b = 0,
		c = 0.0,
	}

class Bar extends GFComponent:
	static func _get_members() -> Dictionary: return {
		a = Vector2.ZERO,
		b = 0.0,
	}
	
#endregion
