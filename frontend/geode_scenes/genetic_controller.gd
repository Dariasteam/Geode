extends Node2D

onready var genetic_connector = load("res://geode_scenes/genetic_connector.gdns").new()
onready var text_label = $Buttons/TextOverview
onready var statistic_plotter = $PlotPanel
onready var net_viewer  = $NeuralPanel
onready var score_tree  = $ScorePanel
onready var agent_name  = $Buttons/GridContainer/LineEditAgentName
onready var buttons  = $Buttons

export(PackedScene) var agent_scene 

var inputs
var outputs

var neural_poblation
var agents_death = 0
var scores = []
var agents_alive = []
var population_size = 50
var n_neurons = 14

var raw_matrixes = []

var best_value = 0
var generation = 0
var last_best_generation = 0

var simulating = true
var agent_lifetime = 5

func agent_inform_death(index, score):
	agents_death += 1	
	scores[index] = score
	if (agents_death == agents_alive.size()):
		agents_death = 0	
		for i in range(agents_alive.size()):
			agents_alive[i].queue_free()
		agents_alive.clear()
							
		prepare_next_single_simulation()
		
		if (simulating):
			start_single_simulation()
		

func generate_poblation(raw_poblation):
	var neural_networks = []
	for i in range(raw_poblation.size() / 2):
		var aux = load("res://geode_scenes/neural_network_connector.gdns").new()
		aux.set_content (raw_poblation[i * 2], raw_poblation[i * 2 + 1], inputs, outputs)
		neural_networks.push_back(aux)
	return neural_networks

# Instancia agentes y comienza una simulación con ellos
func start_single_simulation():	
	var raw_poblation = genetic_connector.get_poblation() # TODO
	neural_poblation = generate_poblation(raw_poblation)
	
	scores.resize(neural_poblation.size())	
		
	raw_matrixes = raw_poblation
		
	for i in range(neural_poblation.size()):
		# Añade un agente y le envia su neural_network
		var aux_agent = agent_scene.instance()
		aux_agent.set_neural_network(neural_poblation[i], i, agent_lifetime)
		aux_agent.connect("dead", self, "agent_inform_death")
		agents_alive.push_back(aux_agent)
		$AgentInstancer.add_child(aux_agent)
		
	neural_poblation.clear()
	
# Emplea el algoritmo genétco para preparar la siguiente simulacion
func prepare_next_single_simulation():	
	
	# establece las evaluaciones y selecciona a los mejores
	print ("esto ocurre")
	genetic_connector.set_evaluations(scores);
	
	# Informa al representador de puntuaciones
	score_tree.update_scores(scores)
	
	# Crea un vector de [puntuaciones, redes_neuronales] para poder acceder 
	# a cada red en función del valor obtenido
	for i in scores.size():
		scores[i] = [scores[i], raw_matrixes[i * 2], raw_matrixes[i * 2 + 1]]	
		
	scores.sort_custom(agents_sorter, "sort")
	
	if (scores[-1][0] > best_value):
		best_value = scores[-1][0] + 0.0000001
		last_best_generation = generation	
	
	var mean = 0
	for element in scores:
		mean += element[0]
	mean /= scores.size()	
		
	var text = str("Generation: ", "%2d" % generation, "\nBest score: ", "%2.3f" % best_value, "\nSince generation: ", "%2d" % last_best_generation)
	
	net_viewer.set_network([scores[-1][1], scores[-1][2]], inputs, outputs)
	statistic_plotter.add_entry([best_value, mean, scores[0][0]])
		
	print (text)
	text_label.set_text(text)	
	generation += 1
	
	# cruza y muta
	genetic_connector.semi_step();

func _ready():
	score_tree.connect("set_net", self, "view_net_at")
	buttons.connect("udpdate_genetic_parameters", self, "set_genetic_parameters")
	var aux_agent = agent_scene.instance()
	inputs = aux_agent.inputs
	outputs = aux_agent.outputs
	aux_agent.queue_free()
	
func start_simulation ():	
	statistic_plotter.clear()	
	
	score_tree.set_n_agents(population_size)	
	
	simulating = true	
	
	genetic_connector.generate_initial_poblation(n_neurons, inputs, outputs)
	print (n_neurons)
	genetic_connector.semi_step();	# cruza y muta		
	
	var text = str("Generation: ", "%2d" % 0, "\nBest score: ", "%2.3f" % 0, "\nSince generation: ", "%2d" %0)		
	text_label.set_text(text) 
	start_single_simulation()
	
func stop_simulation ():
	simulating = false
	for child in $AgentInstancer.get_children():
		child.queue_free()
	scores.clear()
	agents_alive.clear()
	best_value = 0
	last_best_generation = 0
	agents_death = 0

func save():	
	var file = File.new()
	if (scores.size() > 1):
		file.open(str(agent_name.text, ".dat"), file.WRITE)
		var i = score_tree.currently_selected;
		file.store_var([raw_matrixes[i * 2], raw_matrixes[i * 2 + 1]])
		file.close()
		print ("Agent saved")
	else:
		print ("There is no agent to save")

func load():
	var file = File.new()
	if (agent_name.text != ""):
		file.open(str(agent_name.text, ".dat"), file.READ)
		var content = file.get_var()
		file.close()
		print ("Agent loaded")
	
		net_viewer.set_network(content, 3, 2)
		var aux_network = load("res://geode_scenes/neural_network_connector.gdns").new()
		aux_network.set_content (content[0], content[1], 3, 2)
		var aux_agent = agent_scene.instance()
		aux_agent.set_neural_network(aux_network, 0, agent_lifetime)
		add_child(aux_agent)
		return content

func set_time (new_time):
	agent_lifetime = new_time
	
func view_net_at(i):
	if (typeof(scores[(-i)-1]) == TYPE_ARRAY):
		net_viewer.set_network([raw_matrixes[i * 2], raw_matrixes[i * 2 + 1]], inputs, outputs)
		
# population_size / candiadtes_size / mutation_rate
func set_genetic_parameters(p_s, c_s, m_r):	
	print (p_s, " ", c_s)	
	genetic_connector.set_genetic_parameters(p_s, c_s, m_r)
	population_size = p_s
	
# ordena en base a el primer elemento del array, se usa para ordenar los arrays de
# [puntuaciones, redes neuronales]
class agents_sorter:
	static func sort(a, b):	
		if a[0] < b[0]:
			return true
		return false

func _on_SpinBoxNeuronSize_value_changed(value):
	n_neurons = value
