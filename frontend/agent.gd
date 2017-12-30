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
	
	if ($Raycast/ray_a.is_colliding()):
		var distance = $Raycast/ray_a.get_collision_point().distance_to(pos)
		inputs.push_back(distance / 10)
	else:
		inputs.push_back(-1)
		
	if ($Raycast/ray_b.is_colliding()):
		var distance = $Raycast/ray_b.get_collision_point().distance_to(pos)		
		inputs.push_back(distance / 10)
	else:
		inputs.push_back(-1)
		
	if ($Raycast/ray_c.is_colliding()):
		var distance = $Raycast/ray_c.get_collision_point().distance_to(pos)		
		inputs.push_back(distance / 10)
	else:
		inputs.push_back(-1)
	
	var outputs = neural_network.evaluate(inputs)
	
	if (abs(spin) > 20):
		die(-255)	
			
	spin += outputs[0]	
	
	rotate(outputs[0])
	move_local_y(outputs[1])
	puntuation += pow(abs(outputs[1]), 2)
	
func _ready():
	$Timer.set_wait_time(60)
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
