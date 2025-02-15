
extends Node2D

var texture:= load("res://icon.png")
var e:GFEntity

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	await get_tree().process_frame
	GFWorld.get_singleton() \
		.start_rest_api()

	var body:= GFEntity.new()
	body.set_name("Body")
	body.add(GFTexture2D, texture)
	body.add(GFCanvasItem)
	body.add(GFPosition2D, Vector2(100, -00))
	body.add_pair(GFCollisionBody2D, GFRigidBody)
	body.set(GFPosition2D, Vector2(100, 0))
	var shape:= GFEntity.new() \
		.set_name("Shape") \
		.add(GFRectangleShape2D) \
		.add_pair("flecs/core/ChildOf", body)


func _process(delta: float) -> void:
	GFWorld.get_singleton().progress(delta)

	if Input.is_action_just_pressed("ui_accept"):
		GFWorld.get_singleton() \
			.lookup("Body") \
			.set(GFPosition2D, Vector2(200, 000))
