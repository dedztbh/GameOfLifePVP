extends Node

const board_scene : PackedScene = preload("res://board.tscn")

var board

# Called when the node enters the scene tree for the first time.
func _ready():
	board = board_scene.instantiate()
	var bytearray = PackedByteArray()
	bytearray.resize(100 * 100)
	bytearray[0 * 100 + 1] = 1
	bytearray[1 * 100 + 2] = 1
	bytearray[2 * 100 + 0] = 1
	bytearray[2 * 100 + 1] = 1
	bytearray[2 * 100 + 2] = 1
	board.init_matrix = bytearray
	board.columns = 100
	add_child(board)


func _on_load_board_dialog_file_selected(path):
	if board != null:
		remove_child(board)
		
	var file = FileAccess.open(path, FileAccess.READ)
	if not file:
		print("Failed to open the file.")
		return

	var result = PackedByteArray()
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
	
	board = board_scene.instantiate()
	board.init_matrix = result
	board.columns = width
	assert(width == height) # temporary restriction
	add_child(board)
