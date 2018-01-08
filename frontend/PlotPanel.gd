extends Panel

func add_entry(entries):
	$Best.add_entry(entries[0])
	$Mean.add_entry(entries[1])
	$Worst.add_entry(entries[2])