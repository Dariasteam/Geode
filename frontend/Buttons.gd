extends Control

onready var controller = get_parent()

onready var play_txt = "Start simulation"
onready var stop_txt = "End simulation"
onready var continue_txt = "Continuar simulación"
onready var pause_txt = "Pausar simulación"

onready var simulating = false

onready var mutation_rate = 5

signal udpdate_genetic_parameters

func _ready():
	$GridContainer/SpinBoxMutationRate.set_value(mutation_rate)

func _on_ButtonPlayStop_pressed():
	if (simulating):
		$GridContainer/ButtonPlayStop.set_text(play_txt)
		controller.stop_simulation()
	else:
		$GridContainer/ButtonPlayStop.set_text(stop_txt)
		controller.start_simulation()
	simulating = !simulating

func _on_SpinBoxMutationRate_value_changed(value):
	mutation_rate = value
	update_genetic_parameters()

func _on_SpinBoxLifeTime_value_changed(value):	
	controller.set_time(value)
	


func _on_ButtonSaveAgent_pressed():
	controller.save()
	pass # replace with function body

func _on_ButtonLoadAgent_button_down():
	controller.load()

func _on_SpinBoxPoblationSize_value_changed( value ):
	if (value < $GridContainer/SpinBoxCandidatesSize.value):
		$GridContainer/SpinBoxPoblationSize.set_value(value + 2)
	update_genetic_parameters()

func update_genetic_parameters():
	emit_signal("udpdate_genetic_parameters", $GridContainer/SpinBoxPoblationSize.value, $GridContainer/SpinBoxCandidatesSize.value, mutation_rate)

func _on_SpinBoxCandidatesSize_value_changed( value ):
	if (value > $GridContainer/SpinBoxPoblationSize.value):
		$GridContainer/SpinBoxCandidatesSize.set_value(value - 2)
	update_genetic_parameters()

