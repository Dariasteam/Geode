extends Panel

export(Color) var input_n_color
export(Color) var output_n_color

var neurons = 10
var neuron_radius = 10

onready var square_margins = Vector2(neuron_radius, neuron_radius)

var input_neurons
var regular_neurons
var output_neurons

var margins = 10

onready var width = rect_size.x
onready var height = rect_size.y

onready var net_radius = min(width / 2 - margins * 2, height / 2 - margins * 2)

var neuron_coordinates = []

onready var origin = Vector2(width / 2, height / 2)

onready var axon_scene = preload("axon.tscn")

var cost_net
var graph_net 

func draw_nodes ():
	var degree_pass = deg2rad(float(360) / neurons)	
	
	# Input neurons
	for i in range(input_neurons[0], input_neurons[1]):		
		var pos = Vector2(cos(i * degree_pass) * net_radius, sin(i * degree_pass) * net_radius)
		
		var rectangle = Rect2(origin + pos - square_margins, square_margins * 2)
		draw_rect(rectangle, input_n_color, true)
		neuron_coordinates.push_back(origin + pos)
	
	# Regualr neurons	
	for i in range(regular_neurons[0], regular_neurons[1]):		
		var pos = Vector2(cos(i * degree_pass) * net_radius, sin(i * degree_pass) * net_radius)	
		draw_circle(origin + pos, neuron_radius, Color(255,255,255))
		neuron_coordinates.push_back(pos + origin)
		
	# Output neurons
	for i in range(output_neurons[0], output_neurons[1]):
		var pos = Vector2(cos(i * degree_pass) * net_radius, sin(i * degree_pass) * net_radius)
		var rectangle = Rect2(origin + pos - square_margins, square_margins * 2)
		draw_rect(rectangle, output_n_color, true)
		neuron_coordinates.push_back(origin + pos)

func draw_axon(p1, p2, value):
	var axon = axon_scene.instance()
	var distance = sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2))
	var angleDeg = atan2(p2.y - p1.y, p2.x - p1.x) * 180 / PI;
		
	axon.set_region_rect(Rect2(0, 0, distance * 2, 40))
	axon.set_position(Vector2(p1.x, p1.y))
	axon.set_rotation_degrees(angleDeg)
		
	var axon_color
	if (abs(value / 1000) < 0.1):
		axon_color = Color(0, 0, 0)
	else:
		axon_color = Color(255 - abs(value / 100), 255, 255 - max(value / 100, 0))
		
	axon.modulate = axon_color
	add_child(axon)	

func _draw():
	draw_nodes()
	
	for i in graph_net.size():
		var p1 = neuron_coordinates[i]
		for j in range(i + 1, graph_net[i].size()):
			if (graph_net[i][j]):
				var p2 = neuron_coordinates[j]
				draw_axon(p1, p2, cost_net[i][j])

func set_network(graph_network, inputs, outputs):
	for child in get_children():
		child.queue_free()
	
	graph_net = graph_network[0]
	cost_net = graph_network[1]
	
	neurons = graph_network[0].size()
	input_neurons = [0, inputs]	
	regular_neurons = [inputs, neurons - outputs]
	output_neurons = [neurons - outputs, neurons]
	update()
	
func _ready():
	pass
	


