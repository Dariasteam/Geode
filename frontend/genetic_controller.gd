extends Node2D

onready var genetic_connector = load("res://genetic_connector.gdns").new();

func generate_poblation(raw_poblation):
	var neural_networks = []
	for i in range(raw_poblation.size() / 2):		
		var aux = load("res://neural_network_connector.gdns").new();
		aux.set_content (raw_poblation[i * 2], raw_poblation[i * 2 + 1], 3, 1)
		neural_networks.push_back(aux)
	return neural_networks

func evaluate_poblation(neural_poblation):
	var scores = []
	for i in range (neural_poblation.size()):
		scores.push_back(0.64 - neural_poblation[i].evaluate([1, 1, 1])[0])
	return scores

func _ready():
	# Genera los N primeros individuos (N = 10 definido en c++, hay
	# que cambiarlo a que se pueda aquí
	genetic_connector.generate_initial_poblation()
	
	## cruza y muta
	genetic_connector.semi_step();
	
	for i in range (1000):
		var raw_poblation = genetic_connector.get_poblation();
		var neural_poblation = generate_poblation(raw_poblation)
		
		var agent_output = neural_poblation[0].evaluate([1, 1, 1])
		print ("best ", agent_output)
				
		var evaluations = evaluate_poblation (neural_poblation)
		#print (evaluations)
		
		# establece las evaluaciones y selecciona a los mejores
		genetic_connector.set_evaluations(evaluations);
		# cruza y muta
		genetic_connector.semi_step();