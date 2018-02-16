doc:
	rm -rf docs/*
	doxygen
	mv docs/html/* docs/
