extends KinematicBody2D

var aux
var pos 

func _ready():
	var simpleclass = load("res://simple_class.gdns").new();
	print(simpleclass.method("Test argument"));
	simpleclass.test()	
		
	var b = [[1, 2, 3, 4],
			 [5, 6, 7, 8]]
	simpleclass.set_matrix(b)
	
	aux = simpleclass.get_matrix()	
	pos = position
	
	print(aux)
	
	set_process(true)

func _process(delta):
	pos.x += (aux[1][0] * 0.01)
	set_position (pos)
	