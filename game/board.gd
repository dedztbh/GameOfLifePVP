extends GridContainer

signal next_iteration

var cells = []
var life_driver = LifeDriver.new()

# Called when the node enters the scene tree for the first time.
func _ready():
	for i in range(columns):
		cells.append([])
		for j in range(columns):
			var new_cell = ColorRect.new()
			new_cell.custom_minimum_size = Vector2(10, 10)
			add_child(new_cell)
			cells[-1].append(new_cell)

	life_driver.update_cell.connect(_update_cell)
	life_driver.update_done.connect(_update_done)
	next_iteration.connect(life_driver.next_iteration)
	
	var bytearray = PackedByteArray()
	bytearray.resize(columns * columns)
	bytearray[0 * columns + 1] = 1
	bytearray[1 * columns + 2] = 1
	bytearray[2 * columns + 0] = 1
	bytearray[2 * columns + 1] = 1
	bytearray[2 * columns + 2] = 1
	_update_cell(0, 1, 1)
	_update_cell(1, 2, 1)
	_update_cell(2, 0, 1)
	_update_cell(2, 1, 1)
	_update_cell(2, 2, 1)
	
	life_driver.setup(columns, columns, bytearray, LifeDriver.BASIC)


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

func _update_cell(i: int, j: int, state: int):
	cells[i][j].color = Color.BLACK if state else Color.WHITE

func _update_done():
	pass


func _on_timer_timeout():
	emit_signal("next_iteration")
