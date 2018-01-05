extends ReferenceRect

#Valores de entrada para la representacion
var entries = []

onready var width = rect_size.x
onready var height = rect_size.y
onready var color = Color(255,0,0)

func add_entry(value):
	if (value == 0):
		value += 0.0001
	entries.push_back(value)
	update()

func calc_y_element (value):	
	return height - (value * height / entries[-1])

func print_axis():
	# X
	var v1a = Vector2(0, 0)
	var v2a = Vector2(0, height)
	draw_line(v1a, v2a, Color(0,0,0), 1)
	# Y
	var v1b = Vector2(0, height)
	var v2b = Vector2(width, height)
	draw_line(v1b, v2b, Color(0,0,0), 1)

func _draw():	
	var element_w = width / entries.size()
	print_axis()
	for i in range(1, entries.size()):
		var v1 = Vector2((i - 1) * element_w, calc_y_element(entries[i - 1]))
		var v2 = Vector2(i * element_w, calc_y_element(entries[i]))
		draw_line(v1, v2, color, 1)


func _ready():	
	add_entry(0.0000001)
	add_entry(0.0000002)
	


	