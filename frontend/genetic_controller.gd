extends Node2D

onready var genetic_connector = load("res://genetic_connector.gdns").new();
onready var text_label = get_parent().get_node("Text")
onready var plotter = get_parent().get_node("PlotPanel")
onready var viewer  = get_parent().get_node("NeuralPanel")

var agent_scene = preload("res://agent.tscn")

var neural_poblation
var agents_death = 0
var scores = []
var agents_alive = []

var best_value = 0
var generation = 0
var last_best_generation = 0

func agent_inform_death(index, score):
	agents_death += 1	
	scores[index] = score;	
	if (agents_death == agents_alive.size()):
		agents_death = 0	
		for i in range(agents_alive.size()):
			agents_alive[i].queue_free()
		agents_alive.clear()
				
		prepare_next_single_simulation()
		start_single_simulation()
		

func generate_poblation(raw_poblation):
	var neural_networks = []
	for i in range(raw_poblation.size() / 2):
		var aux = load("res://neural_network_connector.gdns").new();
		aux.set_content (raw_poblation[i * 2], raw_poblation[i * 2 + 1], 3, 2)
		neural_networks.push_back(aux)
	return neural_networks

# Instancia agentes y comienza una simulación con ellos
func start_single_simulation():		
	var raw_poblation = genetic_connector.get_poblation()
	neural_poblation = generate_poblation(raw_poblation)
	
	scores.resize(neural_poblation.size())	
	viewer.set_network([raw_poblation[0], raw_poblation[1]], 3, 2)
		
	for i in range(neural_poblation.size()):
		# Añade un agente y le envia su neural_network
		var aux_agent = agent_scene.instance()
		aux_agent.set_neural_network(neural_poblation[i], i)
		aux_agent.connect("dead",self,"agent_inform_death")
		agents_alive.push_back(aux_agent)
		add_child(aux_agent)	
		
	neural_poblation.clear()
	
# Emplea el algoritmo genétco para preparar la siguietne simulacion
func prepare_next_single_simulation():	
	
	# establece las evaluaciones y selecciona a los mejores
	genetic_connector.set_evaluations(scores);
		
	scores.sort()
	
	
	
	if (scores[-1] > best_value):
		best_value = scores[-1] + 0.0001
		last_best_generation = generation
	
	var mean = 0
	for element in scores:
		mean += element
	mean /= scores.size()	
	
	var text = str("Generation: ", "%2d" % generation, "\nBest score: ", "%2.3f" % best_value, "\nSince generation: ", "%2d" % last_best_generation)
	
	plotter.add_entry([best_value, mean, scores[0]])
	
	
	print (text)
	text_label.set_text(text)
	
	generation += 1
	
	# cruza y muta
	genetic_connector.semi_step();

func _ready():
	genetic_connector.generate_initial_poblation()	
	## cruza y muta
	genetic_connector.semi_step();
	
	var text = str("Generation: ", "%2d" % 0, "\nBest score: ", "%2.3f" % 0, "\nSince generation: ", "%2d" %0)		
	text_label.set_text(text)
	
	start_single_simulation()
	
	
		