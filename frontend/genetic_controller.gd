extends Node2D

onready var genetic_connector = load("res://genetic_connector.gdns").new();
var agent_scene = preload("res://agent.tscn")

var neural_poblation
var agents_death = 0

func agent_inform_death():
	agents_death += 1
	if (agents_death == neural_poblation.size()):
		print ("Todo el mundo está muerto")
		prepare_next_single_simulation()
		start_single_simulation()
		agents_death = 0

func generate_poblation(raw_poblation):
	var neural_networks = []
	for i in range(raw_poblation.size() / 2):		
		var aux = load("res://neural_network_connector.gdns").new();
		aux.set_content (raw_poblation[i * 2], raw_poblation[i * 2 + 1], 3, 2)
		neural_networks.push_back(aux)
	return neural_networks

func evaluate_poblation(neural_poblation):
	var scores = []
	for i in range (neural_poblation.size()):
		scores.push_back(0.64 - (neural_poblation[i].evaluate([1, 1, 1])[0]))
	return scores

# Instancia agentes y comienza una simulación con ellos
func start_single_simulation():
	print ("Simulando")
	var raw_poblation = genetic_connector.get_poblation()
	neural_poblation = generate_poblation(raw_poblation)	
	for i in range(neural_poblation.size()):
		# Añade un agente y le envia su neural_network
		var aux_agent = agent_scene.instance()		
		aux_agent.set_neural_network(neural_poblation[i])
		aux_agent.connect("dead",self,"agent_inform_death")
		add_child(aux_agent)	
	
# Emplea el algoritmo genétco para preparar la siguietne simulacion
func prepare_next_single_simulation():
	var evaluations = evaluate_poblation (neural_poblation)
	print (evaluations[0])
	# establece las evaluaciones y selecciona a los mejores
	genetic_connector.set_evaluations(evaluations);	
	# cruza y muta
	genetic_connector.semi_step();

func _ready():
	genetic_connector.generate_initial_poblation()	
	## cruza y muta
	genetic_connector.semi_step();
	
	start_single_simulation()
	
	
		