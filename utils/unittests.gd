
extends SceneTree

const TestRunner:= preload("./glecs_tests_runner.gd")

func _initialize():
	create_timer(30).timeout.connect(func():
		OS.crash("Timeout: Ran for over 30 seconds")
	)
	
	current_scene.set_script(TestRunner)
	await process_frame
	await process_frame
	current_scene.run_tests()
