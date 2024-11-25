
extends SceneTree

const TestRunner:= preload("./glecs_tests_runner.gd")

func _initialize():
	create_timer(30).timeout.connect(func():
		OS.crash("Ran for 30 seconds, timeout")
	)
	var node = TestRunner.new()
	node.run_tests()
