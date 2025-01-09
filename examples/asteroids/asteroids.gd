
extends Node2D

var texture:= load("res://icon.png")

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	await get_tree().process_frame
	GFGlobalWorld.start_rest_api()
	
	var body:= GFEntity.new() \
		.set_name("Body") \
		.add(GFTexture2D, texture) \
		.add(GFCanvasItem) \
		.add(GFPosition2D, Vector2(100, -00)) \
		.add_pair(GFCollisionObject2D, GFRigidBody)
	body.set(GFPosition2D, Vector2(100, 0))
	var shape:= GFEntity.new() \
		.set_name("Shape") \
		.add(GFRectangleShape2D) \
		.add_pair("flecs/core/ChildOf", body)


func _process(delta: float) -> void:
	GFGlobalWorld.progress(delta)
	
	if Input.is_action_just_pressed("ui_accept"):
		GFGlobalWorld.lookup("Body").set(GFPosition2D, Vector2(200, 000))
