extends Node2D

func _ready():
	# carga de la librería compartida en c++ e instancia 
	# la clase que contiene, la guarda en genetic_connector
	var genetic_connector = load("res://neural_network.gdns").new();
	
	# Genera los N primeros individuos (N = 10 definido en c++, hay
	# que cambiarlo a que se pueda aquí
	genetic_connector.generate_initial_poblation()
	
	## cruza y muta
	genetic_connector.semi_step();	
	
	# N = 10
	var evaluations = [100, 1, 1, 2, 2, 2, 0, 0, 0, 0]
	
	for i in range (2000):
		# establece las evaluaciones y selecciona a los mejores
		genetic_connector.set_evaluations(evaluations);
		# cruza y muta
		genetic_connector.semi_step();
	
	var poblation = genetic_connector.get_poblation();
	
	
	