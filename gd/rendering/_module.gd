extends GFModule

const Systems:= preload("./_systems.gd")

func _register(world: GFWorld) -> void:
	set_name("rendering")
	world.register_script(GFCanvasItem)
	world.register_script(GFDrawRect2D)
	world.register_script(GFOnDraw)
	world.register_script(GFOnUpdateVisualTransform)
	world.register_script(GFTexture2D)
	world.register_script(Systems)
