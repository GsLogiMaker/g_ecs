
extends Node2D

var texture:= load("res://icon.png")
var e:GFEntity

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	await get_tree().process_frame
	GFWorld.get_singleton() \
		.start_rest_api()

	e = GFEntity.new()
	e.set_name("Body")
	e.add(GFTexture2D, texture)
	e.add(GFCanvasItem)
	e.add_pair(GFCollisionBody2D, GFRigidBody)
	e.set(GFPosition2D, Vector2(100, 0))
	var shape:= GFEntity.new() \
		.set_name("Shape") \
		.add(GFRectangleShape2D) \
		.add_pair("flecs/core/ChildOf", e)


func _process(delta: float) -> void:
	GFWorld.get_singleton().progress(delta)

	if Input.is_action_just_pressed("ui_accept"):
		e.set(GFPosition2D, Vector2(200, 000))
