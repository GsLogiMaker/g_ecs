
extends GutTest

var world:GFWorld

func before_all():
	world = GFWorld.new()
	world.new_pipeline(&"test")

func after_all():
	world.free()

#region Tests

func test_optional_terms():
	var w:= world
	w.new_pipeline("1")
	w.new_pipeline("2")
	w.new_pipeline("3")

	var empty:= GFEntity.spawn(w) \
		.set_name("Empty")
	var just_ints:= GFEntity.spawn(w) \
		.set_name("JustInts") \
		.add_component(Ints)
	var just_bools:= GFEntity.spawn(w) \
		.set_name("JustBools") \
		.add_component(Bools)
	var all:= GFEntity.spawn(w) \
		.set_name("All") \
		.add_component(Ints) \
		.add_component(Bools)

	var data:Dictionary = {i=0, ints=0, bools=0}
	var callable:= func(ints, bools):
		data.i += 1
		if ints:
			data.ints += 1
		if bools:
			data.bools += 1

	data.i = 0
	data.ints = 0
	data.bools = 0
	w.new_system(&"1") \
		.maybe_with(Ints) \
		.maybe_with(Bools) \
		.for_each(callable)
	w.run_pipeline(&"1", 0.0)
	assert_eq(data.ints, 2)
	assert_eq(data.bools, 2)

	data.i = 0
	data.ints = 0
	data.bools = 0
	w.new_system(&"2") \
		.with(Ints) \
		.maybe_with(Bools) \
		.for_each(callable)
	w.run_pipeline(&"2", 0.0)
	assert_eq(data.i, 2)
	assert_eq(data.ints, 2)
	assert_eq(data.bools, 1)

	data.i = 0
	data.ints = 0
	data.bools = 0
	w.new_system(&"3") \
		.maybe_with(Ints) \
		.with(Bools) \
		.for_each(callable)
	w.run_pipeline(&"3", 0.0)
	assert_eq(data.i, 2)
	assert_eq(data.ints, 1)
	assert_eq(data.bools, 2)

func test_or_operation_terms():
	var w:= world

	var data:= {ints=0, bools=0}
	w.new_system("test") \
		.or_with(Bools).with(Ints) \
		.for_each(func(bools:Bools, ints:Ints):
			prints(bools, ints)
			#if term is Ints:
				#data.ints += 1
			#if term is Bools:
				#data.bools += 1
			)

	GFEntity.spawn(w).add_component(Ints)
	GFEntity.spawn(w).add_component(Bools)
	GFEntity.spawn(w).add_component(Ints).add_component(Bools)

	w.run_pipeline("test", 0.0)

	prints(data)

#endregion

#region Components

class Bools extends GFComponent:
	func _build(b_: GFComponentBuilder) -> void:
		b_.add_member("a", TYPE_BOOL)
		b_.add_member("b", TYPE_BOOL)
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
	var a:int:
		get: return getm(&"a")
		set(v): setm(&"a", v)
	var b:int:
		get: return getm(&"b")
		set(v): setm(&"b", v)

#endregion
