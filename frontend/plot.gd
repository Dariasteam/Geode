extends Panel

#Valores de entrada para la representacion
var best_entries = []
var mean_entries = []
var worst_entries = []

export(Color) var colorBest
export(Color) var colorMean
export(Color) var colorWorst

export(int) var margin = 10

onready var width = rect_size.x - margin
onready var height = rect_size.y - margin

func add_entry(value):	
	best_entries.push_back(value[0] + 0.000000001)
	mean_entries.push_back(max(value[1], 0))
	worst_entries.push_back(max(value[2], 0))	
	update()

func calc_y_element (value):	
	return height - (value * (height - margin) / best_entries[-1])

func print_axis():
	# X
	var v1a = Vector2(margin, margin)
	var v2a = Vector2(margin, height)
	draw_line(v1a, v2a, Color(255,255,255), 1)
	# Y
	var v1b = Vector2(margin, height)
	var v2b = Vector2(width, height)
	draw_line(v1b, v2b, Color(255,255,255), 1)

func draw_polygon(entry, color):
	var element_w = width / entry.size()
	for i in range(1, entry.size()):
		var v1 = Vector2(margin + (i - 1) * element_w, calc_y_element(entry[i - 1]))
		var v2 = Vector2(margin + i * element_w, calc_y_element(entry[i]))
		draw_line(v1, v2, color, 1)

func _draw():	
	print_axis()
		
	draw_polygon(best_entries, colorBest)
	draw_polygon(mean_entries, colorMean)
	draw_polygon(worst_entries, colorWorst)
	
func _ready():	
	add_entry([0, 0, 0])
	add_entry([0, 0, 0])