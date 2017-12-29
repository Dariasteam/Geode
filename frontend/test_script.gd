extends KinematicBody2D

var aux
var pos 

func _ready():
	var simpleclass = load("res://simple_class.gdns").new();		
		
	var b = [[1, 2, 3, 4],
			 [5, 6, 7, 8]]
	simpleclass.set_puntuations(b)
	
	aux = simpleclass.get_new_poblation()
	#print(aux)
	
	set_process(true)

func _process(delta):
	pass
	