extends Node2D

var neural_network
var index 

signal dead

var spin = 0
var score = 0
var alive = true

func set_neural_network(nn, i):
	neural_network = nn
	index = i
	$Timer.set_wait_time(4)
	$Timer.start()	
	set_physics_process(true)

func _physics_process(delta):
	var pos = $RigidBody2D.global_position
	
	var inputs = []
	
	if ($Weapons/weapon.is_colliding()):		
		var collider = $Weapons/weapon.get_collider()
		if (collider != null):
			collider.get_parent().die_eated()
			eat()
	
	# Recoger todos los valores de los sensores
	for item in $Raycast.get_children():
		if (item.is_colliding()):
			var distance = item.get_collision_point().distance_to(item.global_position)
								
			var mid_distance = item.cast_to.length() / 2
			
			item.modulate = Color(1, distance / mid_distance * 2 * 255, 1)
								
			if (distance > mid_distance):				
				distance = (distance - mid_distance) / mid_distance * - 1				
			else:				
				distance = (distance / mid_distance)
											
			var color_g = max(255 * -distance , 0.1)
			var color_b = max(255 *  distance , 0.1)					
			
			inputs.push_back(max(distance, -1))
			
		else:		
			item.modulate = Color(1, 1, 1)
			inputs.push_back(-1)
			
			
	var outputs = neural_network.evaluate(inputs)
	
	if (abs(spin) > 20):
		die(-255)	
			
	spin += outputs[0]	
	
	rotate(outputs[0])
	move_local_y(outputs[1] * 200) 
	score += outputs[1]
	
func _ready():
	#var size = get_viewport().size
	set_global_position(get_parent().get_global_position())
	#set_global_position(Vector2(rand_range(80, size.x * 2 - 80), rand_range(80, size.y * 2 - 80)))	
	#set_physics_process(false)
	pass
	
func die(time):
	if (alive):
		alive = false
		set_physics_process(false)
		$Timer.stop()
		$Sprite.modulate = Color(255, 0, 0)
		$RigidBody2D.queue_free()
		$Weapons/weapon.set_enabled(false)
		emit_signal("dead", index, score)

func _on_Timer_timeout():	
	die(0)	
	
func eat():		
	$Timer.disconnect("timeout", self, "_on_Timer_timeout")
	var time_left = $Timer.get_time_left()
	score += 5
	$Timer.stop()
	$Timer.set_wait_time(time_left + 5)	
	$Timer.connect("timeout", self, "_on_Timer_timeout")
	$Timer.start()

func die_eated():
	die($Timer.wait_time - $Timer.get_time_left())

func _on_RigidBody2D_body_entered(body):	
	die($Timer.wait_time - $Timer.get_time_left())
