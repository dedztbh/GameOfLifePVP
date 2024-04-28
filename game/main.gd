extends Node

const board_scene : PackedScene = preload("res://board.tscn")

var board

# Called when the node enters the scene tree for the first time.
func _ready():
	board = board_scene.instantiate()
	board.init_matrix = PackedByteArray()
	var bytearray = board.init_matrix
	bytearray.resize(100 * 100)
	bytearray[0 * 100 + 1] = 1
	bytearray[1 * 100 + 2] = 1
	bytearray[2 * 100 + 0] = 1
	bytearray[2 * 100 + 1] = 1
	bytearray[2 * 100 + 2] = 1
	board.rows = 100
	board.columns = 100
	board.speed_changed.connect(board_speed_changed)
	board.ruleset["wrap_around"] = $WrapAroundButton.button_pressed
	add_child(board)


func _on_load_board_dialog_file_selected(path):
	if board != null:
		board.queue_free()
		
	var file = FileAccess.open(path, FileAccess.READ)
	if not file:
		print("Failed to open the file.")
		return

	board = board_scene.instantiate()
	board.init_matrix = PackedByteArray()
	var result = board.init_matrix
	var width = -1
	var height = 0
	# Read the file line by line
	while not file.eof_reached():
		var line = file.get_line()
		if line == "":
			continue
		
		# Split the line into values assuming comma separation
		var values = line.split(",")
		if width < 0:
			width = values.size()
		height += 1
		
		# Convert each value to byte and add to PackedByteArray
		for value in values:
			var byte_value = int(value)  # Assuming the CSV contains integer values that fit in a byte
			result.push_back(byte_value)

	file.close()

	board.init_matrix = result
	board.columns = width
	board.rows = height
	board.speed_changed.connect(board_speed_changed)
	board.ruleset["wrap_around"] = $WrapAroundButton.button_pressed
	add_child(board)


func _on_speed_up_button_pressed():
	if board != null:
		board.change_speed(2)


func _on_slow_down_button_pressed():
	if board != null:
		board.change_speed(0.5)

func board_speed_changed(speed):
	$CurrentSpeed.text = "Speed: " + str(speed) + " FPS"
