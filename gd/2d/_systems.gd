
extends GFModule

func _register(world: GFWorld) -> void:
	var OnSet:= world.lookup("/root/flecs/core/OnSet")
	
	GFObserverBuilder.new() \
		.set_name("emit_transform_update_from_position") \
		.set_events(OnSet) \
		.with(GFPosition2D) \
		.for_each(transform_updated)
	
	GFObserverBuilder.new() \
		.set_name("emit_transform_update_from_rotation") \
		.set_events(OnSet) \
		.with(GFRotation2D) \
		.for_each(transform_updated)
		
	GFObserverBuilder.new() \
		.set_name("emit_transform_update_from_scale") \
		.set_events(OnSet) \
		.with(GFScale2D) \
		.for_each(transform_updated)

static func transform_updated(c:GFComponent) -> void:
	GFEntity.from(GFOnTransformUpdate, c.get_world()) \
		.emit(c.get_source_id())
