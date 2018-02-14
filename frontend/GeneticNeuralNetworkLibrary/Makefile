all:
	# DNA
	g++ -fPIC -o src/dna.os -c ../../backend/neural_network/dna.cpp  -g -O3 -std=c++14 -Icpp_bindings/include -Igodot_headers -Isrc
	# NEURON
	g++ -fPIC -o src/neuron.os -c ../../backend/neural_network/neuron.cpp  -g -O3 -std=c++14 -Icpp_bindings/include -Igodot_headers -Isrc
	# FEEDBACK_BUS
	g++ -fPIC -o src/feedback_bus.os -c ../../backend/neural_network/feedback_bus.cpp  -g -O3 -std=c++14 -Icpp_bindings/include -Igodot_headers -Isrc
	# NeuralNetwork
	g++ -fPIC -o src/neural_network.os -c ../../backend/neural_network/concurrent_neural_network.cpp -g -O3 -std=c++14 -Icpp_bindings/include -Igodot_headers
	# Genetic Algorithm Connector
	g++ -fPIC -o src/genetic_connector.os -c src/genetic_connector.cpp  -g -O3 -std=c++14 -Icpp_bindings/include -Igodot_headers -Isrc
	g++ -o lib/libgenetic.so -shared src/genetic_connector.os src/dna.os src/neural_network.os src/neuron.os src/feedback_bus.os -Llib -lgodot_cpp_bindings
	# Neural Network Connector
	g++ -fPIC -o src/neural_network_connector.os -c src/neural_network_connector.cpp  -g -O3 -std=c++14 -Icpp_bindings/include -Igodot_headers -Isrc
	g++ -o lib/libneural.so -shared src/neural_network_connector.os src/dna.os src/neural_network.os src/neuron.os src/feedback_bus.os -Llib -lgodot_cpp_bindings
clean:
	rm -rf src/*.os src/*.so
