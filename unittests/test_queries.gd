
extends GutTest

var world:GFWorld

func before_all():
	world = GFWorld.new()
	#world.new_pipeline(&"test")

func after_all():
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

	var empty:= GFEntity.spawn(w) \
		.set_name("Empty") \
		.add_component(Bools)
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

	data.ints = 0
	data.bools = 0
	w.system_builder() \
		.with(Ints) \
		.maybe_with(Bools) \
		.for_each(callable)
	w.progress(0.0)
	assert_eq(data.ints, 2)
	assert_eq(data.bools, 1)

	data.ints = 0
	data.bools = 0
	w.system_builder() \
		.maybe_with(Ints) \
		.with(Bools) \
		.for_each(callable)
	w.progress(0.0)
	assert_eq(data.ints, 1 + (2)) # Extra 2 from previous system running
	assert_eq(data.bools, 3 + (1)) # Extra 1 from previous system running

func test_or_operation_terms():
	var w:= world

	var data:= {ints=0, bools=0}
	w.system_builder() \
		.with(Bools).or_with(Ints) \
		.for_each(func(bools_or_ints:GFComponent):
			if bools_or_ints is Ints:
				data.ints += 1
			if bools_or_ints is Bools:
				data.bools += 1
			)

	GFEntity.spawn(w).add_component(Ints)
	GFEntity.spawn(w).add_component(Ints)
	GFEntity.spawn(w).add_component(Ints)
	GFEntity.spawn(w).add_component(Bools)
	GFEntity.spawn(w).add_component(Ints).add_component(Bools)

	w.progress(0.0)

	assert_eq(data.ints, 3)
	assert_eq(data.bools, 2)

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
