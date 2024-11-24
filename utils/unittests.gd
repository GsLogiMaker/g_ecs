
extends SceneTree

const TestRunner:= preload("./glecs_tests_runner.gd")

func _initialize():
	prints("A")
	create_timer(30).timeout.connect(func():
		prints("B")
		OS.crash("Ran for 30 seconds, timeout")
	)
	prints("C")

	var node = TestRunner.new()
	prints(node)
	prints("D")
	node.run_tests()
	prints("E")
