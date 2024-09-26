
extends Control

const GlecsTestRunner:= preload("res://glecs_tests_runner.gd")

var thread:= Thread.new()
var mutex:= Mutex.new()

var world:= GFWorld.new()

var test_runner:GlecsTestRunner = null

func _enter_tree() -> void:
	test_runner = preload("res://glecs_tests_runner.tscn").instantiate()
	test_runner.exit_on_test_completion = false
	add_child(test_runner)

func _exit_tree() -> void:
	if thread.is_started():
		thread.wait_to_finish()
	world.free()

func _ready() -> void:
	world.start_rest_api()

	if "--unittests" in  OS.get_cmdline_args() or "--unittest" in  OS.get_cmdline_args():
		prints("--- Running Glecs unittests ---")
		test_runner.run_tests()
		return

	prints("Input ready")
	thread.start(func():
		while true:
			prints(is_queued_for_deletion())
			if is_queued_for_deletion():
				return
			var stdin:= OS.read_string_from_stdin().strip_edges()
			var args:= stdin.split(" ")
			if args[0] in ["exit", "quit", "close", "continue"]:
				break
			if args[0] in ["test"]:
				test_runner.run_tests.call_deferred()
				continue
			if args[0] in ["reset"]:
				get_tree().change_scene_to_file.call_deferred("res://main.tscn")
				continue
			do_test.call_deferred()
		)

func _physics_process(delta: float) -> void:
	world.progress(delta)

func _on_run_tests_pressed() -> void:
	get_tree().change_scene_to_file("res://addons/gut/gui/GutRunner.tscn")


func _on_play_asteroids_pressed() -> void:
	get_tree().change_scene_to_file("res://examples/asteroids/asteroids.tscn")


func _on_compose_release_pressed() -> void:
	pass

func do_test():
	prints("Test start")
	test_script_register()
	prints("Test done")

class MyModule extends GFModule:
	class SubModule extends GFModule:
		const import_MyComponent = MyComponent
		const import_std = preload("res://addons/glecs/gd/std/std.gd")

	class SubModuleNamed extends GFModule:
		func _register(_w:GFWorld) -> void:
			set_name("MyNamedModule woo!")

func test_script_register():
	world.register_script(MyModule)
	prints("Registered module", world.coerce_id(MyModule))
	GFEntity.spawn()

class MyComponent extends GFComponent:
	func _build(builder: GFComponentBuilder) -> void:
		builder.set_name("MyComponent")
		builder.add_member("my_member", TYPE_INT)

func test_observer() -> void:
	pass

func test_set_name() -> void:
	var e1:= GFEntity.spawn(world)
	e1.set_name("Hello")
	prints("set name to", e1.get_name())
	e1.set_name("glecs")
	prints("then to", e1.get_name())

func test_system_builder() -> void:
	var e1:= GFEntity.spawn(world) \
		.add_component("glecs/meta/Vector2") \
		.add_component("glecs/meta/Vector3")
	var e2:=GFEntity.spawn(world) \
		.add_component("glecs/meta/Vector2") \
		.add_component("glecs/meta/Vector3")
	var e3:=GFEntity.spawn(world) \
		.add_component("glecs/meta/Vector2") \
		.add_component("glecs/meta/Vector3") \
		.add_component("glecs/meta/Rect2")
	var e4:=GFEntity.spawn(world) \
		.add_component("glecs/meta/Vector2")
	var e5:=GFEntity.spawn(world) \
		.add_component("glecs/meta/Vector2") \
		.add_component("glecs/meta/Transform2D")
	e1.get_component("glecs/meta/Vector2").setm("x", 0.4)
	e1.get_component("glecs/meta/Vector3").setm("z", 0.1)
	e2.get_component("glecs/meta/Vector2").setm("x", 2.2)
	e2.get_component("glecs/meta/Vector3").setm("z", 3.3)
	e4.get_component("glecs/meta/Vector2").setm("x", 123.0)

	prints("iterator set")
	var q:GFQuery = world.query_builder() \
		.maybe_with("glecs/meta/Vector3") \
		.with("glecs/meta/Vector2") \
		.build()
	var i:= 1.0
	for terms:Array[GFComponent] in q.iterate():
		var vec3:= terms[0]
		if vec3:
			vec3.setm("z", vec3.getm('z')*i)
		var vec2:= terms[1]
		vec2.setm("x", vec2.getm('x')*i)
		i += 1.0
	prints("iterator get")
	for terms:Array[GFComponent] in q.iterate():
		var vec3:= terms[0]
		var vec2:= terms[1]
		if vec3:
			prints("hello world!", vec2.get_source_id(), vec2.get_id(), vec2.getm("x"), vec3.getm("z"))
		else:
			prints("hello world!", vec2.get_source_id(), vec2.get_id(), vec2.getm("x"), vec3)

	prints("system")
	world.system_builder() \
		.with("glecs/meta/Vector2") \
		.without("glecs/meta/Rect2") \
		.with("glecs/meta/Vector3") \
		.for_each(func(vec2:GFComponent, vec3:GFComponent):
			prints("hello world!", vec2.get_source_id(), vec2.get_id(), vec2.getm("x"), vec3.getm("z"))
			)
	world.progress(0.0)

	prints("iterator 2")
	prints("Rect2", world.coerce_id("glecs/meta/Rect2"))
	prints("Transform2D", world.coerce_id("glecs/meta/Transform2D"))
	var or_query:GFQuery = world.query_builder() \
		.with("glecs/meta/Rect2").or_with("glecs/meta/Transform2D") \
		.with("glecs/meta/Vector2") \
		.build()
	for terms:Array[GFComponent] in or_query.iterate():
		var rect_or_tra:= terms[0]
		var vec2:= terms[1]
		prints("hello world!", vec2.get_source_id(), rect_or_tra.get_id())

	prints("did", test_system_builder)

func query_test() -> void:
	prints("query_test")

	var query:GFQuery = world.query_builder() \
		.with("glecs/meta/Vector2") \
		.with("glecs/meta/Vector3") \
		.build()

	var i:= 0.0
	for terms:Array in query.iterate():
		var vec2_c:GFComponent = terms[0]
		var vec3_c:GFComponent = terms[1]
		vec2_c.setm("x", i)
		vec3_c.setm("x", i + 1.0)
		prints("Terms ", vec2_c.getm("x"), vec3_c.getm("x"))
		i += 1.0

	if randi_range(0,1) == 0 and false:
		GFEntity.spawn(world) \
			.add_component("glecs/meta/Vector2") \
			.add_component("glecs/meta/Vector3")
		prints("Spawn A")
	else:
		GFEntity.spawn(world) \
			.add_component("glecs/meta/String") \
			.add_component("glecs/meta/Vector2") \
			.add_component("glecs/meta/Vector3")
		prints("Spawn B")

	prints("END query_test")



func _on_button_pressed() -> void:
	do_test()

func test_comp_builder():
	pass
	var glecs:= GFEntity.from("glecs", world)
	var comp:= GFEntity.from("Comp", world)
	if not comp:
		world.component_builder() \
			.set_name("Comp") \
			.add_member("string_name", TYPE_STRING_NAME) \
			.add_member("node_path", TYPE_NODE_PATH) \
			.add_member("rid", TYPE_RID) \
			.add_member("object", TYPE_OBJECT) \
			.add_member("callable", TYPE_CALLABLE) \
			.add_member("signal", TYPE_SIGNAL) \
			.add_member("dictionary", TYPE_DICTIONARY) \
			.add_member("array", TYPE_ARRAY) \
			.build()
		prints("Defined component")
		return

	var glecs_comp:= glecs.get_component("Comp")
	if not glecs_comp:
		glecs.add_component("Comp")
		glecs.add_component("glecs/meta/Array");
		prints("Added component")
		return

	prints("--- pre ---")
	prints("string_name Pre", glecs_comp.getm("string_name"))
	prints("node_path Pre", glecs_comp.getm("node_path"))
	prints("rid Pre", glecs_comp.getm("rid"))
	prints("object Pre", glecs_comp.getm("object"))
	prints("callable Pre", glecs_comp.getm("callable"))
	prints("signal Pre", glecs_comp.getm("signal"))
	prints("dictionary Pre", glecs_comp.getm("dictionary"))
	prints("array Pre", glecs_comp.getm("array"))
	prints("--- set ---")
	glecs_comp.setm("string_name", &"Hello")
	glecs_comp.setm("node_path", ^"Hello/World")
	glecs_comp.setm("rid", RID())
	glecs_comp.setm("object", world)
	glecs_comp.setm("callable", func(): print("hello world!"))
	glecs_comp.setm("signal", tree_entered)
	glecs_comp.setm("dictionary", {"data":3})
	glecs_comp.setm("array", [1, "2", false])
	prints("--- post ---")
	prints("string_name Post", glecs_comp.getm("string_name"))
	prints("node_path Post", glecs_comp.getm("node_path"))
	prints("rid Post", glecs_comp.getm("rid"))
	prints("object Post", glecs_comp.getm("object"))
	prints("callable Post", glecs_comp.getm("callable"))
	prints("signal Post", glecs_comp.getm("signal"))
	prints("array Post", glecs_comp.getm("dictionary"))
	prints("dictionary Post", glecs_comp.getm("array"))

	#if not test_vector2:
		#prints("Define TestVariants")
		#world.component_builder() \
			#.set_name("TestVector2") \
			#.add_member("vector2", TYPE_VECTOR2) \
			#.add_member("vector2i", TYPE_VECTOR2I) \
			#.add_member("rect2", TYPE_RECT2) \
			#.add_member("rect2i", TYPE_RECT2I) \
			#.add_member("vector3", TYPE_VECTOR3) \
			#.add_member("vector3i", TYPE_VECTOR3I) \
			#.add_member("transform2d", TYPE_TRANSFORM2D) \
			#.add_member("vector4", TYPE_VECTOR4) \
			#.add_member("vector4i", TYPE_VECTOR4I) \
			#.add_member("plane", TYPE_PLANE) \
			#.add_member("quaternion", TYPE_QUATERNION) \
			#.add_member("aabb", TYPE_AABB) \
			#.add_member("basis", TYPE_BASIS) \
			#.add_member("transform3d", TYPE_TRANSFORM3D) \
			#.add_member("projection", TYPE_PROJECTION) \
			#.add_member("color", TYPE_COLOR) \
			#.build()
		#return
	#
	#var data:= glecs.get_component(test_vector2)
	#if not data:
		#prints("Add to glecs")
		#glecs.add_component(test_vector2)
		#return
	#
		#
	#var test:= GFEntity.from("glecs", world) \
		#.get_component("TestVector2")

	#prints("Get vector2", test.getm("vector2"))
	#prints("Get vector2i", test.getm("vector2i"))
	#prints("Get rect2", test.getm("rect2"))
	#prints("Get rect2i", test.getm("rect2i"))
	#prints("Get vector3", test.getm("vector3"))
	#prints("Get vector3i", test.getm("vector3i"))
	#prints("Get transform2d", test.getm("transform2d"))
	#prints("Get vector4", test.getm("vector4"))
	#prints("Get vector4i", test.getm("vector4i"))
	#prints("Get plane", test.getm("plane"))
	#prints("Get quaternion", test.getm("quaternion"))
	#prints("Get aabb", test.getm("aabb"))
	#prints("Get basis", test.getm("basis"))
	#prints("Get transform3d", test.getm("transform3d"))
	#prints("Get projection", test.getm("projection"))
	#prints("Get color", test.getm("color"))
	#
	#test.setm("vector2", Vector2.ONE)
	#test.setm("vector2i", Vector2i.ONE)
	#test.setm("rect2", Rect2(1, 1, 1, 1))
	#test.setm("rect2i", Rect2i(1, 1, 1, 1))
	#test.setm("vector3", Vector3.ONE)
	#test.setm("vector3i", Vector3i.ONE)
	#test.setm("transform2d", Transform2D.FLIP_Y)
	#test.setm("vector4", Vector4.ONE)
	#test.setm("vector4i", Vector4i.ONE)
	#test.setm("plane", Plane.PLANE_XZ)
	#test.setm("quaternion", Quaternion.IDENTITY)
	#test.setm("aabb", AABB(Vector3.ONE, Vector3.ONE))
	#test.setm("basis", Basis.FLIP_Y)
	#test.setm("transform3d", Transform3D.FLIP_Y)
	#test.setm("projection", Projection(Vector4.ONE, Vector4.ONE, Vector4.ONE, Vector4.ONE))
	#test.setm("color", Color.RED)

	# Get top level entity in Flecs world
	#var flecs:= GFEntity.from("flecs", world)
	#var Description:= GFEntity.from("flecs/doc/Description", world)
	#if not flecs.get_component(Description):
		#flecs.add_component(Description)
		#prints("Added doc")
		#return
	#else:
		#print(flecs.get_component(Description).getm("value"))
		#return

	# world.component_builder() \
	# 	.set_name("1Bool") \
	# 	.add_member("A", TYPE_BOOL) \
	# 	.build()
	# world.component_builder() \
	# 	.set_name("2Bool") \
	# 	.add_member("A", TYPE_BOOL) \
	# 	.add_member("B", TYPE_BOOL) \
	# 	.build()
	# world.component_builder() \
	# 	.set_name("3Bool") \
	# 	.add_member("A", TYPE_BOOL) \
	# 	.add_member("B", TYPE_BOOL) \
	# 	.add_member("C", TYPE_BOOL) \
	# 	.build()
	# world.component_builder() \
	# 	.set_name("4Bool") \
	# 	.add_member("A", TYPE_BOOL) \
	# 	.add_member("B", TYPE_BOOL) \
	# 	.add_member("C", TYPE_BOOL) \
	# 	.add_member("S", TYPE_BOOL) \
	# 	.build()

	#world.component_builder() \
	 	#.set_name("MyPosition") \
	 	#.add_member("x", TYPE_INT) \
	 	#.add_member("y", TYPE_INT) \
	 	#.build()
	#
	#world.component_builder() \
		#.set_name("Mix") \
		#.add_member("fi", TYPE_BOOL) \
		#.add_member("se", TYPE_INT) \
		#.add_member("th", TYPE_BOOL) \
		#.add_member("fo", TYPE_INT) \
		#.add_member("fith", TYPE_BOOL) \
		#.build()
	#
	#world.component_builder() \
		#.set_name("Optimal") \
		#.add_member("fi", TYPE_BOOL) \
		#.add_member("th", TYPE_BOOL) \
		#.add_member("fith", TYPE_BOOL) \
		#.add_member("se", TYPE_INT) \
		#.add_member("fo", TYPE_INT) \
		#.build()

	#prints(
		#world.coerce_id(flecs),
		#world.coerce_id(&"flecs"),
		#world.coerce_id("/root/flecs"),
		#world.coerce_id(^"flecs/core/Component"),
		#world.coerce_id(1),
		#world.coerce_id(doc),
		#world.coerce_id(Vector2i(485, 1)),
		#world.coerce_id(true),
		#world.coerce_id(.5),
		#world.coerce_id(null),
	#)
