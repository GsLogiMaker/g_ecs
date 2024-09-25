
extends Node

const GutRunnerGD:= preload("res://addons/gut/gui/GutRunner.gd")
const GutConfig:= preload('res://addons/gut/gut_config.gd')
const GutEditorGlobals:= preload('res://addons/gut/gui/editor_globals.gd')
const ResultExporter:= preload('res://addons/gut/result_exporter.gd')

## When true, exits the program with OK if all tests succeded, otherwise exits
## the program with 1.
@export var exit_on_test_completion:= false

var gut_runner:GutRunnerGD = null
var gut_config:GutConfig= GutConfig.new()
var gut_exporter:ResultExporter = ResultExporter.new()

func _enter_tree() -> void:
	gut_runner = preload("res://addons/gut/gui/GutRunner.tscn").instantiate()
	add_child(gut_runner)

	gut_config.load_options(GutEditorGlobals.editor_run_gut_config_path)
	gut_runner.ran_from_editor = false
	gut_runner.set_gut_config(gut_config)

func run_tests():
	gut_config.options.tests = []
	gut_config.options.unit_test_name = ""
	gut_runner.run_tests()
	
	# Wait for tests up to 10 seconds
	var data:= {tests_done = false}
	gut_runner._gut.end_run.connect(func():
		data.tests_done = true
		,
		CONNECT_ONE_SHOT
	)
	var elapsed:= 0.0
	while elapsed < 10.0:
		if data.tests_done:
			break
		await get_tree().process_frame
		elapsed += get_process_delta_time()
	
	# Quit fail
	if exit_on_test_completion:
		var props:Dictionary = gut_exporter \
			.get_results_dictionary(gut_runner._gut) \
			.test_scripts \
			.props
		if (
			props.size() == 0
			or null in [
				props.get("pending"),
				props.get("failures"),
				props.get("errors"),
				props.get("tests"),
				props.get("passing"),
			]
			or props.pending + props.failures + props.errors != 0
			or props.tests != props.passing
			or props.passing == 0
		):
			# Test suite failed. Exit with error code.
			prints("Closing with error...")
			#get_tree().quit(1)
			OS.crash("Glecs unittest(s) failed")
			return
		
		# Quit success
		var since_try_kill:= 0.0
		while true:
			prints("Closing with passing...")
			get_tree().quit(OK)
			await get_tree().process_frame
			since_try_kill +=- get_process_delta_time()
			if since_try_kill >= 5.0:
				# Program won't quit. Force
				# kill program. (Exit code will be an error)
				OS.kill(OS.get_process_id())
			if since_try_kill >= 10.0:
				# Give up
				break
		return
