
extends Node2D


var texture:= load("res://icon.png")
var e:GFEntity

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	e = GFEntity.new()
	e.set_name("Test")
	e.add(GFCanvasItem)
	#e.add(GFTexture2D, texture)
	e.add(GFPosition2D, Vector2(0, 0))
	e.add(GFDrawRect2D)
	e.add(GFPosition2D)
	e.add(GFRotation2D)
	e.set_pair(GFSize2D, GFDrawRect2D, Vector2(100, 22))

	GFEntity.from(GFOnDraw, e.get_world()).emit(e)

	GFWorld.get_default_world().start_rest_api()


func _process(delta: float) -> void:
	var rot_c:GFRotation2D = e.get(GFRotation2D)
	rot_c.set_angle(rot_c.get_angle() + (delta * Input.get_axis("ui_left", "ui_right")))
	
	var v_axis:= Input.get_axis("ui_up", "ui_down")
	if v_axis:
		var size_c:GFSize2D= e.get(GFSize2D, GFDrawRect2D)
		size_c.set_y(size_c.get_y() - (100 * delta * v_axis))
