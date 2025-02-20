
extends GFModule

static func get_main_tree() -> SceneTree:
	return Engine.get_main_loop()

static func get_main_space_2d() -> RID:
	return Engine.get_main_loop() \
		.current_scene \
		.get_viewport() \
		.world_2d \
		.space


static func update_body_transform(
	body:GFCollisionBody2D,
	pos_c:GFPosition2D,
	rot_c:GFRotation2D,
	scl_c:GFScale2D,
	_shapes=null,
) -> void:
	var pos:= pos_c.get_vec() if pos_c else Vector2.ZERO
	var rot:= rot_c.get_angle() if rot_c else 0.0
	var scl:= scl_c.get_scale() if false else Vector2.ONE
	PhysicsServer2D.body_set_state(
		body.get_rid(),
		PhysicsServer2D.BODY_STATE_TRANSFORM,
		Transform2D(rot, scl, 0.0, pos),
	)


func _register(w: GFWorld) -> void:
	var OnAdd:= w.lookup("/root/flecs/core/OnAdd")
	var OnSet:= w.lookup("/root/flecs/core/OnSet")
	var OnRemove:= w.lookup("/root/flecs/core/OnRemove")

	#region GFCollisionBody2D

	GFObserverBuilder.new() \
		.set_name("construct_collision_object2d") \
		.set_events(OnAdd) \
		.with(GFCollisionBody2D, "/root/flecs/core/*") \
		.for_each(func(body:GFCollisionBody2D):
			var rid:= PhysicsServer2D.body_create()
			PhysicsServer2D.body_set_space(
				rid,
				get_main_space_2d(),
				)
			PhysicsServer2D.body_set_state_sync_callback(
				rid,
				rigid_body_sync.bind(
					body.get_source_entity()
					),
				)
			body._set_rid(rid)
			body.get_source_entity().add(GFCollisionShapes)
			)

	GFObserverBuilder.new() \
		.set_name("deconstruct_collision_object2d") \
		.set_events("/root/flecs/core/OnRemove") \
		.with(GFCollisionBody2D, "/root/flecs/core/*") \
		.for_each(func(static_body:GFCollisionBody2D):
			PhysicsServer2D.free_rid(static_body.get_rid())
			)

	GFObserverBuilder.new() \
		.set_name("set_collision_object2d_to_rigid") \
		.set_events(OnAdd) \
		.with(GFCollisionBody2D, GFRigidBody) \
		.with(GFCollisionShapes).io_in() \
		.for_each(func(body:GFCollisionBody2D, _shapes):
			PhysicsServer2D.body_set_mode(
				body.get_rid(),
				PhysicsServer2D.BODY_MODE_RIGID,
				)
			)

	GFObserverBuilder.new() \
		.set_name("set_collision_object2d_to_static") \
		.set_events(OnAdd) \
		.with(GFCollisionBody2D, GFStaticBody) \
		.with(GFCollisionShapes).io_in() \
		.for_each(func(body:GFCollisionBody2D, _shapes):
			PhysicsServer2D.body_set_mode(
				body.get_rid(),
				PhysicsServer2D.BODY_MODE_STATIC,
				)
			)

	GFObserverBuilder.new() \
		.set_name("update_collision_object2d_transform") \
		.set_events(OnAdd, OnSet) \
		.with(GFCollisionBody2D, "/root/flecs/core/*") \
		.with_maybe(GFPosition2D) \
		.with_maybe(GFRotation2D) \
		.with_maybe(GFScale2D) \
		.with(GFCollisionShapes).io_filter() \
		.for_each(update_body_transform)

	#endregion

	#region GFRectangleShape

	GFObserverBuilder.new() \
		.set_name("construct_collision_shape2d") \
		.set_events(OnAdd) \
		.with(GFCollisionBody2D, "/root/flecs/core/*").up() \
		.with(GFCollisionShapes).up() \
		.with(GFRectangleShape2D) \
		.for_each(func(
			body:GFCollisionBody2D,
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

	GFObserverBuilder.new() \
		.set_name("deconstruct_collision_shape2d") \
		.set_events(OnRemove) \
		.with(GFCollisionBody2D, "/root/flecs/core/*").up() \
		.with(GFCollisionShapes).up() \
		.with(GFRectangleShape2D) \
		.for_each(func(
			body:GFCollisionBody2D,
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

	GFObserverBuilder.new() \
		.set_name("set_rectangle_shape2d_size") \
		.set_events(OnSet) \
		.with(GFRectangleShape2D) \
		.for_each(func(shape:GFRectangleShape2D):
			PhysicsServer2D.shape_set_data(
				shape.get_rid(),
				shape.get_size(),
				)
			)

	GFObserverBuilder.new() \
		.set_name("set_rectangle_shape2d_transform") \
		.set_events(OnSet) \
		.with(GFCollisionBody2D, "/root/flecs/core/*").io_filter().up() \
		.with(GFCollisionShapes).io_filter().up() \
		.with(GFRectangleShape2D).io_filter() \
		.with_maybe(GFPosition2D).io_in() \
		.with_maybe(GFRotation2D).io_in() \
		.with_maybe(GFScale2D).io_in() \
		.for_each(func(
			body:GFCollisionBody2D,
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

	#endregion


static func rigid_body_sync(
	state:PhysicsDirectBodyState2D,
	entity:GFEntity,
) -> void:
	var position_c:GFPosition2D = entity.get(GFPosition2D) \
		if entity.has(GFPosition2D) \
		else null
	var rotation_c:GFRotation2D = entity.get(GFRotation2D) \
		if entity.has(GFRotation2D) \
		else null
	if position_c:
		position_c.setm_no_notify("vec", state.transform.get_origin())
	if rotation_c:
		rotation_c.setm_no_notify("angle", state.transform.get_rotation())

	var c:GFComponent = position_c \
		if position_c \
		else rotation_c \
		if rotation_c \
		else null

	if c:
		GFEntity.from(
			GFOnTransformUpdate,
			c.get_world(),
		).emit(c.get_source_id())
