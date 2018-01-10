extends Tree

var n_agents

var scores_index = []
var aux_array = []

var currently_selected = 0

signal set_net

func _ready():
	set_columns(2)
	set_column_title(0, "Index")
	set_column_title(1, "Score")
	set_column_titles_visible(true)
	set_hide_root(true)
	set_select_mode(SELECT_SINGLE)

func set_n_agents(n):
	n_agents = n
	for i in range(n_agents):
		scores_index.push_back([i, 0])
	

func update_scores(scores):
	clear()
	for i in range(n_agents):
		scores_index[i][1] = scores[i]
	
	aux_array = scores_index
	
	aux_array.sort_custom(scores_sorter, "sort")
	
	for element in aux_array:
		var child = create_item(self)
		child.set_text(0, str("Agent ", element[0]))
		child.set_text(1, str(element[1]))		

		
# ordena en base a el segundo elemento del array, se usa para ordenar los arrays de
# [índice, score]
class scores_sorter:
	static func sort(a, b):	
		if a[1] > b[1]:
			return true
		return false


func _on_ScorePanel_cell_selected():	
	currently_selected = int(get_selected().get_text(0).split(" ")[1])
	print (currently_selected)
	emit_signal ("set_net", currently_selected)
	

	pass # replace with function body
