
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
	await gut_runner._gut.end_run
	
	if exit_on_test_completion:
		var props:Dictionary = gut_exporter \
			.get_results_dictionary(gut_runner._gut) \
			.test_scripts \
			.props
		if props.pending + props.failures + props.errors != 0:
			get_tree().quit(1)
			return
		get_tree().quit(OK)
		return
