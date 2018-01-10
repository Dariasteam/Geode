extends Node2D

class MyCustomSorter:
	static func sort(a, b):
		if a[0] < b[0]:
			return true
		return false

func _ready():
	var my_items = [[5, "Potato"], [9, "Rice"], [4, "Tomato"]]
	my_items.sort_custom(MyCustomSorter, "sort")
	print (my_items)

