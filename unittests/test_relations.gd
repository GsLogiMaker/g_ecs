
extends GutTest

var world:GFWorld

func before_all():
	world = GFWorld.new()

func after_all():
	world.free()

#region Tests

#func test_bools():
	#var apple:= GFEntity.spawn(world) \
		#.set_name("Apple")
#
	#GFEntity.spawn(world) \
		#.set_name("Eats")
#
	#var man:= GFEntity.spawn(world) \
		#.set_name("Man") \
		#.add_pair("Eats", apple)
#
	#var cow:= GFEntity.spawn(world) \
		#.set_name("Cow")
	#var grass:= GFEntity.spawn(world) \
		#.set_name("Grass")
	#cow.add_pair("Eats", grass)
#
	#world.progress(0.0)
#
	## TODO: Test querying for relations
	#world.query_builder().build()
#
	#man.delete()
	#apple.delete()

#endregion

#region Components

class Bools extends GFComponent:
	func _build(b_: GFComponentBuilder) -> void:
		b_.add_member("a", TYPE_BOOL)
		b_.add_member("b", TYPE_BOOL)

#endregion
