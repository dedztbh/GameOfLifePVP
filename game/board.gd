extends GridContainer

# Called when the node enters the scene tree for the first time.
func _ready():
	for i in range(columns):
		for j in range(columns):
			var new_cell = ColorRect.new()
			new_cell.custom_minimum_size = Vector2(10, 10)
			add_child(new_cell)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
