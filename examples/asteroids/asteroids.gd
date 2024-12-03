
extends Node2D


var texture:= load("res://icon.png")

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	GFEntity.new() \
		.set_name("Test") \
		.add_component(GFTexture2D, texture) \
		.add_component(GFCanvasItem) \
		.add_component(GFPosition2D)

	GFGlobalWorld.start_rest_api()


func _process(delta: float) -> void:
	GFGlobalWorld.progress(delta)
	GFGlobalWorld.lookup("Test") \
		.get_component(GFPosition2D) \
		.set_vec(get_global_mouse_position())
