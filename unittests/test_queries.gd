
extends GutTest

var world:GFWorld

func before_each():
	world = GFWorld.new()
	#world.new_pipeline(&"test")

func after_each():
	world.free()

#region Tests

func test_optional_terms():
	var w:= world
	var data:Dictionary = {ints=0, bools=0}
	var callable:= func(ints, bools):
		if ints:
			data.ints += 1
		if bools:
			data.bools += 1

	var empty:= GFEntity.new_in_world(w) \
		.set_name("Empty") \
		.add(Bools)
	var just_ints:= GFEntity.new_in_world(w) \
		.set_name("JustInts") \
		.add(Ints)
	var just_bools:= GFEntity.new_in_world(w) \
		.set_name("JustBools") \
		.add(Bools)
	var all:= GFEntity.new_in_world(w) \
		.set_name("All") \
		.add(Ints) \
		.add(Bools)

	data.ints = 0
	data.bools = 0
	GFSystemBuilder.new_in_world(world) \
		.with(Ints) \
		.maybe_with(Bools) \
		.for_each(callable)
	w.progress(0.0)
	assert_eq(data.ints, 2, "Expected `Ints` to be queried 2 times")
	assert_eq(data.bools, 1, "Expected `Bools` to be queried 1 times")

	data.ints = 0
	data.bools = 0
	GFSystemBuilder.new_in_world(world) \
		.maybe_with(Ints) \
		.with(Bools) \
		.for_each(callable)
	w.progress(0.0)
	assert_eq(data.ints, 1 + (2), "Expected `Ints` to be queried 3 times total") # Extra 2 from previous system running
	assert_eq(data.bools, 3 + (1), "Expected `Bools` to be queried 4 times total") # Extra 1 from previous system running

func test_or_operation_terms():
	var w:= world

	var data:= {ints=0, bools=0}
	GFSystemBuilder.new_in_world(world) \
		.with(Bools).or_with(Ints) \
		.for_each(func(bools_or_ints:GFComponent):
			if bools_or_ints is Ints:
				data.ints += 1
			if bools_or_ints is Bools:
				data.bools += 1
			)

	GFEntity.new_in_world(w).add(Ints)
	GFEntity.new_in_world(w).add(Ints)
	GFEntity.new_in_world(w).add(Ints)
	GFEntity.new_in_world(w).add(Bools)
	GFEntity.new_in_world(w).add(Ints).add(Bools)

	w.progress(0.0)

	assert_eq(data.ints, 3)
	assert_eq(data.bools, 2)


func test_up_traversal():
	var par:= GFEntity.new_in_world(world) \
		.set_name("Parent") \
		.add(Bools)
	var child:= GFEntity.new_in_world(world) \
		.set_name("Child") \
		.add(Bools) \
		.add_pair("flecs/core/ChildOf", par)

	var parent_descriptions:= GFQueryBuilder.new_in_world(world) \
		.with(Bools).up() \
		.with(Bools) \
		.build()

	var i:= 0
	for desc in parent_descriptions.iterate():
		assert_true(desc[0].get_source_id() == par.get_id(), "Expecteded 1st item to be the parent")
		assert_true(desc[1].get_source_id() == child.get_id(), "Expecteded 2nd item to be the child")
		i += 1

	assert_eq(i, 1, "Expected query to match 1 item")


func _test_desc_traversal():
	# TODO: Implement test for desc traversal
	assert_true(false, "Unimplemnted")


func _test_cascade_traversal():
	# TODO: Implement test for cascade traversal
	assert_true(false, "Unimplemnted")


func test_with_pair():
	var w:= world
	var data:= {ints_bools=0}

	GFEntityBuilder.new_in_world(world) \
		.add_pair(Ints, Bools) \
		.build()

	var query:GFQuery = GFQueryBuilder.new_in_world(world) \
		.with(Ints, Bools) \
		.build()

	var count:= 0
	for _x in query.iterate():
		count += 1

	assert_eq(count, 1, "Expected query to find 1 entity")


#endregion

#region Components

class Bools extends GFComponent:
	func _build(b_: GFComponentBuilder) -> void:
		b_.add_member("a", TYPE_BOOL)
		b_.add_member("b", TYPE_BOOL)
		b_.set_name("Bools")
	var a:bool:
		get: return getm(&"a")
		set(v): setm(&"a", v)
	var b:bool:
		get: return getm(&"b")
		set(v): setm(&"b", v)

class Ints extends GFComponent:
	func _build(b_: GFComponentBuilder) -> void:
		b_.add_member("a", TYPE_INT)
		b_.add_member("b", TYPE_INT)
		b_.set_name("Ints")
	var a:int:
		get: return getm(&"a")
		set(v): setm(&"a", v)
	var b:int:
		get: return getm(&"b")
		set(v): setm(&"b", v)

#endregion
