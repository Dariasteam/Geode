extends Node2D

var neural_network
var index 

signal dead

var spin = 0
var puntuation = 0

func set_neural_network(nn, i):
	neural_network = nn
	index = i

func _physics_process(delta):
	var pos = $RigidBody2D.global_position
	
	var inputs = []
	
	# Recoger todos los valores de los sensores
	for item in $Raycast.get_children():
		if (item.is_colliding()):
			var distance = item.get_collision_point().distance_to(item.global_position)
								
			var mid_distance = item.cast_to.length() / 2
								
			if (distance > mid_distance):				
				distance = (distance - mid_distance) / mid_distance * - 1				
			else:				
				distance = (distance / mid_distance)
		
			var color_g = max(255 * -distance , 1)
			var color_b = max(255 * distance , 1)
				
			
			item.modulate = Color(0, color_g, color_b)
			
			inputs.push_back(max(distance, -1))
		else:		
			item.modulate = Color(0, 255, 0)
			inputs.push_back(-1)
		
	#print (inputs)
	
	var outputs = neural_network.evaluate(inputs)
	
	if (abs(spin) > 20):
		die(-255)	
			
	spin += outputs[0]	
	
	rotate(outputs[0])
	move_local_y(outputs[1] * 10)
	puntuation += outputs[1] * 8
	
func _ready():
	$Timer.set_wait_time(20)
	$Timer.start()
	set_physics_process(true)
	
func die(time):
	$Timer.stop()
	$Sprite.modulate = Color(255, 0, 0)
	set_physics_process(false)
	emit_signal("dead", index, puntuation)		

func _on_Timer_timeout():
	die($Timer.wait_time)

func _on_RigidBody2D_body_entered(body):	
	die($Timer.wait_time - $Timer.get_time_left())
