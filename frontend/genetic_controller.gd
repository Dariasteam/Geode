extends Node2D

onready var genetic_connector = load("res://genetic_connector.gdns").new();
var agent_scene = preload("res://agent.tscn")

var neural_poblation
var agents_death = 0
var scores = []
var agents_alive = []

func agent_inform_death(index, score):
	agents_death += 1
	# print (index, " ha muerto con una puntuación de ", score)
	scores[index] = score;	
	if (agents_death == agents_alive.size()):
		agents_death = 0	
		for i in range(agents_alive.size()):
			agents_alive[i].queue_free()
		agents_alive.clear()
		
		print ("Todo el mundo está muerto")		
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
	print ("Simulando")
	var raw_poblation = genetic_connector.get_poblation()
	neural_poblation = generate_poblation(raw_poblation)
	
	scores.resize(neural_poblation.size())	
	
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
	print (scores[-1])
	
	# cruza y muta
	genetic_connector.semi_step();

func _ready():
	genetic_connector.generate_initial_poblation()	
	## cruza y muta
	genetic_connector.semi_step();
	
	start_single_simulation()
	
	
		