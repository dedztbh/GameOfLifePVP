extends Label


# Called when the node enters the scene tree for the first time.
func _ready():
	text = Engine.get_license_text()
	visible = false


func _unhandled_input(event):
	if event is InputEventKey:
		if event.pressed and event.keycode == KEY_L:
			visible = !visible
