
extends GFModule

const Systems:= preload("./_systems.gd")

func _register(world: GFWorld) -> void:
	world.register_script(GFCollisionBody2D)
	world.register_script(GFCollisionShapes)
	world.register_script(GFRectangleShape2D)
	world.register_script(GFRigidBody)
	world.register_script(GFStaticBody)
	world.register_script(Systems)
