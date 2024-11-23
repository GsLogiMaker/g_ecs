
extends SceneTree

const TestRunner:= preload("./glecs_tests_runner.gd")

func _initialize():
	prints("A")
	create_timer(30).timeout.connect(func():
		prints("B")
		OS.crash("Timeout: Ran for over 30 seconds")
	)
	prints("C")
	
	current_scene.set_script(TestRunner)
	prints("D")
	current_scene.run_tests()
	prints("E")
