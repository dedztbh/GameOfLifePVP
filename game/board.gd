extends GridContainer

signal next_iteration
signal speed_changed

@export var rows = 64

var init_matrix : Variant = null
var cells = []
var life_driver = LifeDriver.new()

# Called when the node enters the scene tree for the first time.
func _ready():
	for i in range(rows):
		cells.append([])
		for j in range(columns):
			var new_cell = ColorRect.new()
			new_cell.custom_minimum_size = Vector2(10, 10)
			add_child(new_cell)
			cells[-1].append(new_cell)
			if init_matrix != null:
				_update_cell(i, j, init_matrix[i * columns + j])

	life_driver.update_cell.connect(_update_cell)
	life_driver.update_done.connect(_update_done)
	next_iteration.connect(life_driver.next_iteration)
	
	life_driver.setup(rows, columns, init_matrix, LifeDriver.BASIC)
	
	emit_signal("speed_changed", 1 / $Timer.wait_time)


func _update_cell(i: int, j: int, state: int):
	cells[i][j].color = Color.BLACK if state else Color.WHITE

func _update_done():
	pass


func _on_timer_timeout():
	emit_signal("next_iteration")


func change_speed(ratio):
	$Timer.wait_time /= ratio
	$Timer.start()
	emit_signal("speed_changed", 1 / $Timer.wait_time)
