
extends GFModule

static func get_main_space_2d() -> RID:
	return Engine.get_main_loop() \
		.current_scene \
		.get_viewport() \
		.world_2d \
		.space

func _register(w: GFWorld) -> void:
	var gf_position_2d:= GFEntity.from(GFPosition2D, w)

	#region GFStaticBody2D
	# On add GFStaticBody2D
	GFObserverBuilder.new_in_world(w) \
		.set_events("/root/flecs/core/OnAdd") \
		.with(GFStaticBody2D) \
		.for_each(func(static_body:GFStaticBody2D):
			var rid:= PhysicsServer2D.body_create()
			static_body.set_rid(rid)
			PhysicsServer2D.body_set_mode(rid, PhysicsServer2D.BODY_MODE_STATIC)
			PhysicsServer2D.body_set_space(
				rid,
				get_main_space_2d(),
				)
			PhysicsServer2D.body_set_state_sync_callback(
				rid,
				rigid_body_sync.bind(
					static_body.get_source_entity(),
					gf_position_2d.get_id(),
					),
				)
			)

	# On free GFRigidBody2D
	GFObserverBuilder.new_in_world(w) \
		.set_events("/root/flecs/core/OnRemove") \
		.with(GFStaticBody2D) \
		.for_each(func(static_body:GFStaticBody2D):
			PhysicsServer2D.free_rid(static_body.get_rid())
			)
	#endregion

	#region Rigid Body 2D
	# On add GFRigidBody2D
	GFObserverBuilder.new_in_world(w) \
		.set_events("/root/flecs/core/OnAdd") \
		.with(GFRigidBody2D) \
		.for_each(func(rigid_body:GFRigidBody2D):
			var source:= rigid_body.get_source_entity()
			
			source.add_component(GFCollisionShapes)
			
			var rid:= PhysicsServer2D.body_create()
			rigid_body.set_rid(rid)
			PhysicsServer2D.body_set_space(
				rid,
				get_main_space_2d(),
				)
			PhysicsServer2D.body_set_state_sync_callback(
				rid,
				rigid_body_sync.bind(
					source,
					gf_position_2d.get_id(),
					),
				)
			)

	# On free GFRigidBody2D
	GFObserverBuilder.new_in_world(w) \
		.set_events("/root/flecs/core/OnRemove") \
		.with(GFRigidBody2D) \
		.for_each(func(rigid_body:GFRigidBody2D):
			# TODO: Remove GFCollisionShapes
			PhysicsServer2D.free_rid(rigid_body.get_rid())
			)
	#endregion


	#region GFRectangleShape
	GFObserverBuilder.new_in_world(w) \
		.set_events("/root/flecs/core/OnAdd") \
		.with(GFRigidBody2D).up() \
		.with(GFCollisionShapes).up() \
		.with(GFRectangleShape2D) \
		.for_each(func(
			body:GFRigidBody2D,
			shapes:GFCollisionShapes,
			shape:GFRectangleShape2D
			):
			shape.set_rid(
				PhysicsServer2D.rectangle_shape_create()
				)
			prints("PREPUSH", shapes.get_shapes())
			shapes.get_shapes().append(shape.get_rid())
			prints("POSTPUSH", shapes.get_shapes())
			PhysicsServer2D.body_add_shape(
				body.get_rid(),
				shape.get_rid(),
				)
			)
	
	GFObserverBuilder.new_in_world(w) \
		.set_events("/root/flecs/core/OnRemove") \
		.with(GFRigidBody2D).up() \
		.with(GFCollisionShapes).up() \
		.with(GFRectangleShape2D) \
		.for_each(func(
			body:GFRigidBody2D,
			shapes:GFCollisionShapes,
			shape:GFRectangleShape2D,
			):
			prints("PREREMOVE", shapes.get_shapes())
			shapes.get_shapes().erase(shape.get_rid())
			prints("POSTREMOVE", shapes.get_shapes())
			PhysicsServer2D.body_remove_shape(
				body.get_rid(),
				0,
				)
			)
	#endregion

static func rigid_body_sync(
	state:PhysicsDirectBodyState2D,
	entity:GFEntity,
	gf_position_2d:int,
) -> void:
	if gf_position_2d != 0:
		entity.set_component(
			gf_position_2d,
			state.transform.origin,
		)
