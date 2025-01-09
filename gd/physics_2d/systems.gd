
extends GFModule

static func get_main_space_2d() -> RID:
	return Engine.get_main_loop() \
		.current_scene \
		.get_viewport() \
		.world_2d \
		.space

func _register(w: GFWorld) -> void:
	#region GFCollisionObject2D
	# On add collision object
	GFObserverBuilder.new_in_world(w) \
		.set_events("/root/flecs/core/OnAdd") \
		.with(w.pair(GFCollisionObject2D, "/root/flecs/core/*")) \
		.for_each(func(static_body:GFCollisionObject2D):
			var rid:= PhysicsServer2D.body_create()
			PhysicsServer2D.body_set_space(
				rid,
				get_main_space_2d(),
				)
			PhysicsServer2D.body_set_state_sync_callback(
				rid,
				rigid_body_sync.bind(
					static_body.get_source_entity(),
					w.coerce_id(GFPosition2D),
					w.coerce_id(GFRotation2D),
					),
				)
			GFCanvasItem.get_main_canvas()
			static_body._set_rid(rid)
			static_body.get_source_entity() \
				.add_component(GFCollisionShapes)
			)


	# On free collision object
	GFObserverBuilder.new_in_world(w) \
		.set_events("/root/flecs/core/OnRemove") \
		.with(w.pair(GFCollisionObject2D, "/root/flecs/core/*")) \
		.for_each(func(static_body:GFCollisionObject2D):
			PhysicsServer2D.free_rid(static_body.get_rid())
			)
	
	
	# On set rigid body
	GFObserverBuilder.new_in_world(w) \
		.set_events("/root/flecs/core/OnAdd") \
		.with(w.pair(GFCollisionObject2D, GFRigidBody)) \
		.with(GFCollisionShapes).access_in() \
		.for_each(func(body:GFCollisionObject2D, _shapes):
			PhysicsServer2D.body_set_mode(
				body.get_rid(),
				PhysicsServer2D.BODY_MODE_RIGID,
				)
			)
	
	
	# On set static body
	GFObserverBuilder.new_in_world(w) \
		.set_events("/root/flecs/core/OnAdd") \
		.with(w.pair(GFCollisionObject2D, GFStaticBody)) \
		.with(GFCollisionShapes).access_in() \
		.for_each(func(body:GFCollisionObject2D, _shapes):
			PhysicsServer2D.body_set_mode(
				body.get_rid(),
				PhysicsServer2D.BODY_MODE_STATIC,
				)
			)
	
	
	# On set transform in collision shape
	GFObserverBuilder.new_in_world(w) \
		.set_events("/root/flecs/core/OnSet") \
		.with(w.pair(GFCollisionObject2D, "/root/flecs/core/*")).access_filter() \
		.maybe_with(GFPosition2D).access_in() \
		.maybe_with(GFRotation2D).access_in() \
		.maybe_with(GFScale2D).access_in() \
		.for_each(func(
			body:GFCollisionObject2D,
			pos_c:GFPosition2D,
			rot_c:GFRotation2D,
			scl_c:GFScale2D,
			):
			var pos:= pos_c.get_vec() if pos_c else Vector2.ZERO
			var rot:= rot_c.get_angle() if rot_c else 0.0
			var scl:= scl_c.get_scale() if false else Vector2.ONE
			PhysicsServer2D.body_set_state(
				body.get_rid(),
				PhysicsServer2D.BODY_STATE_TRANSFORM,
				Transform2D(rot, scl, 0.0, pos),
				)
			)
	#endregion


	#region GFRectangleShape
	# Add rectangle shape to collision object
	GFObserverBuilder.new_in_world(w) \
		.set_events("/root/flecs/core/OnAdd") \
		.with(w.pair(GFCollisionObject2D, "/root/flecs/core/*")).up() \
		.with(GFCollisionShapes).up() \
		.with(GFRectangleShape2D) \
		.for_each(func(
			body:GFCollisionObject2D,
			shapes:GFCollisionShapes,
			shape:GFRectangleShape2D
			):
			shape._set_rid(
				PhysicsServer2D.rectangle_shape_create()
				)
			shapes.get_shapes().append(shape.get_rid())
			PhysicsServer2D.body_add_shape(
				body.get_rid(),
				shape.get_rid(),
				)
			)
	
	
	# Set rectangle shape size
	GFObserverBuilder.new_in_world(w) \
		.set_events("/root/flecs/core/OnSet") \
		.with(GFRectangleShape2D) \
		.for_each(func(shape:GFRectangleShape2D):
			PhysicsServer2D.shape_set_data(
				shape.get_rid(),
				shape.get_size(),
				)
			)
	
	
	# On set transform in shape
	GFObserverBuilder.new_in_world(w) \
		.set_events("/root/flecs/core/OnSet") \
		.with(w.pair(GFCollisionObject2D, "/root/flecs/core/*")).access_filter().up() \
		.with(GFCollisionShapes).access_filter().up() \
		.with(GFRectangleShape2D).access_filter() \
		.maybe_with(GFPosition2D).access_in() \
		.maybe_with(GFRotation2D).access_in() \
		.maybe_with(GFScale2D).access_in() \
		.for_each(func(
			body:GFCollisionObject2D,
			shapes:GFCollisionShapes,
			shape:GFRectangleShape2D,
			pos_c:GFPosition2D,
			rot_c:GFRotation2D,
			scl_c:GFScale2D,
			):
			var pos:= pos_c.get_vec() if pos_c else Vector2.ZERO
			var rot:= rot_c.get_angle() if rot_c else 0.0
			var scl:= scl_c.get_scale() if false else Vector2.ONE
			var shape_idx:= shapes.get_shapes().find(shape.get_rid())
			PhysicsServer2D.body_set_shape_transform(
				body.get_rid(),
				shape_idx,
				Transform2D(rot, scl, 0.0, pos),
				)
			)
	
	
	# Remove rectangle shape from collision object
	GFObserverBuilder.new_in_world(w) \
		.set_events("/root/flecs/core/OnRemove") \
		.with(w.pair(GFCollisionObject2D, "/root/flecs/core/*")).up() \
		.with(GFCollisionShapes).up() \
		.with(GFRectangleShape2D) \
		.for_each(func(
			body:GFCollisionObject2D,
			shapes:GFCollisionShapes,
			shape:GFRectangleShape2D,
			):
			var index:= shapes.get_shapes().find(shape.get_rid())
			PhysicsServer2D.body_remove_shape(
				body.get_rid(),
				index,
				)
			shapes.get_shapes().pop_at(index)
			)
	#endregion

static func rigid_body_sync(
	state:PhysicsDirectBodyState2D,
	entity:GFEntity,
	gf_position_2d:int,
	gf_rotation_2d:int,
) -> void:
	if gf_position_2d != 0:
		entity.set_component(
			gf_position_2d,
			state.transform.origin,
		)
	if gf_rotation_2d != 0:
		entity.set_component(
			gf_rotation_2d,
			state.transform.get_rotation(),
		)
