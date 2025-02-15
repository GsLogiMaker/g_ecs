
extends GFModule

func _register(world: GFWorld) -> void:
	world.register_script(GFCollisionObject2D)
	world.register_script(GFCollisionShapes)
	world.register_script(GFCollisionShape2D)
	world.register_script(GFRectangleShape)
	world.register_script(GFRigidBody)
	world.register_script(GFStaticBody)
	world.register_script(load("./systems.gd"))
