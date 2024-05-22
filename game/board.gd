extends GridContainer

signal speed_changed

@export var rows = 64

var init_matrix : Variant = null
var cells = []
var life_driver = LifeDriver.new()
var ruleset = Dictionary()
const COLORS = [Color.WHITE, Color.BLACK, Color.RED]
var interval_us : int = 100000

# Called when the node enters the scene tree for the first time.
func _ready():
	for i in range(rows):
		for j in range(columns):
			var new_cell = ColorRect.new()
			new_cell.custom_minimum_size = Vector2(10, 10)
			add_child(new_cell)
			cells.append(new_cell)
			if init_matrix != null:
				update_cell(i, j, init_matrix[i * columns + j])
	
	life_driver.setup(rows, columns, init_matrix, LifeDriver.TWO_PLAYER, ruleset)
	life_driver.auto_run_set_interval(interval_us)
	life_driver.start_auto_run()
	emit_signal("speed_changed", 1000000.0 / interval_us)

func _process(_delta):
	life_driver.consume_updates(update_done)
	

func update_done(color_updates : Dictionary):
	for ij : Vector2i in color_updates:
		update_cell(ij.x, ij.y, color_updates[ij])


func update_cell(i: int, j: int, state: int):
	cells[i * columns + j].color = COLORS[state]


func change_speed(ratio):
	interval_us = clampi(interval_us / ratio, 1, 1000000)
	life_driver.auto_run_set_interval(interval_us)
	emit_signal("speed_changed", 1000000.0 / interval_us)
