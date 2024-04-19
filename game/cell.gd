extends TextureButton


func texture_from_color(color: Color):
	var img = Image.create(1, 1, false, Image.FORMAT_RGB8)
	img.fill(color)
	return ImageTexture.create_from_image(img)


func set_color(color: Color):
	var color_darker = color
	color_darker.v *= 0.9
	var color_darkerer = color
	color_darkerer.v *= 0.7
	
	var texture = texture_from_color(color)
	var texture_darker = texture_from_color(color_darker)
	var texture_darkerer = texture_from_color(color_darkerer)
	
	texture_normal = texture
	texture_pressed = texture_darkerer
	texture_hover = texture_darker
	texture_disabled = texture
	

# Called when the node enters the scene tree for the first time.
func _ready():
	set_color(Color.WHITE);
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(_delta):
	pass


func set_read_only(b):
	disabled = b
