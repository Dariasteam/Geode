extends Node2D

var neural_network

signal dead

func set_neural_network(nn):
	neural_network = nn

func _physics_process(delta):
	var pos = $StaticBody2D.global_position
	
	var inputs = []
	
	if ($Raycast/ray_a.is_colliding()):
		var distance = $Raycast/ray_a.get_collision_point().distance_to(pos)
		inputs.push_back(distance)		
	else:
		inputs.push_back(-1)
		
	if ($Raycast/ray_b.is_colliding()):
		var distance = $Raycast/ray_b.get_collision_point().distance_to(pos)		
		inputs.push_back(distance)
	else:
		inputs.push_back(-1)
		
	if ($Raycast/ray_c.is_colliding()):
		var distance = $Raycast/ray_c.get_collision_point().distance_to(pos)		
		inputs.push_back(distance)
	else:
		inputs.push_back(-1)
		
	var outputs = neural_network.evaluate(inputs)
	
	move_local_x(outputs[0])
	move_local_y(outputs[1])
	
func _ready():
	set_physics_process(true)	
	
	
func _on_Timer_timeout():
	emit_signal("dead")
	queue_free()

func _on_RigidBody2D_body_entered(body):
	emit_signal("dead")
	queue_free()
