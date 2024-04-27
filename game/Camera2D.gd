extends Camera2D

# Speed of camera movement
@export var speed = 200
# Zoom speed
@export var zoom_speed = 0.1
# Minimum and maximum zoom levels
@export var min_zoom = 0.5
@export var max_zoom = 2.0

func _process(delta):
	var motion = Vector2.ZERO

	# Check for movement keys
	if Input.is_action_pressed("ui_up"):
		motion.y -= 1
	if Input.is_action_pressed("ui_down"):
		motion.y += 1
	if Input.is_action_pressed("ui_left"):
		motion.x -= 1
	if Input.is_action_pressed("ui_right"):
		motion.x += 1

	# Normalize to prevent faster diagonal movement
	motion = motion.normalized() * speed

	# Apply the motion to the camera's position
	position += motion * delta

	# Zoom out
	if Input.is_action_pressed("ui_zoom_out"):
		zoom = Vector2(zoom.x - zoom_speed * delta, zoom.y - zoom_speed * delta)
		zoom.x = max(min_zoom, zoom.x)
		zoom.y = max(min_zoom, zoom.y)

	# Zoom in
	if Input.is_action_pressed("ui_zoom_in"):
		zoom = Vector2(zoom.x + zoom_speed * delta, zoom.y + zoom_speed * delta)
		zoom.x = min(max_zoom, zoom.x)
		zoom.y = min(max_zoom, zoom.y)
