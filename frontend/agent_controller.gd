extends Node2D

func _ready():
	
	var neural_network = load("res://neural_network_connector.gdns").new();
			
	# Matriz del grafo de la red neuronal
	var g = [
		[true, false, false, true, true],
		[false, true, false, true, true],
		[false, false, true, true, true],
		[false, false, false, true, false],
		[false, false, false, false, true],
	]
	
	# Matriz de pesos de los axones
	var c = [
		[1, 1, 1, 1, 1000000],
		[0, 1, 1, 1, 1000000],
		[0, 0, 1, 1, 1000000],
		[0, 0, 0, 1, 0],
		[0, 0, 0, 0, 1],
	]
	
	# Inicia la red neuronal 
	neural_network.set_content(g, c, 3, 2)
	
	var evaluations = neural_network.evaluate([1, 1, 1])
	
	print (evaluations)
	
	