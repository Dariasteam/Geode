extends Node2D

var aux
var pos 

var genetic_connector
var input
var result

func _ready():
	genetic_connector = load("res://neural_network.gdns").new();
	
	genetic_connector