all:
	# NeuralNetwork
	clang -fPIC -o src/neural_network.os -c src/neural_nework_bind.cpp -g -O3 -std=c++14 -Icpp_bindings/include -Igodot_headers
	clang -o lib/libneural.so -shared src/dna.os src/workable_nn.os src/neural_network.os  -Llib -lgodot_cpp_bindings
	# Genetic Algorithm
	clang -fPIC -o src/genetic_connector.os -c src/genetic_connector.cpp  -g -O3 -std=c++14 -Icpp_bindings/include -Igodot_headers -Isrc
	clang -o lib/libgenetic.so -shared src/dna.os src/workable_nn.os src/genetic_connector.os  -Llib -lgodot_cpp_bindings
	# Connector
	clang -fPIC -o src/neural_network_connector.os -c src/neural_network_connector.cpp  -g -O3 -std=c++14 -Icpp_bindings/include -Igodot_headers -Isrc
	clang -o lib/genetic_connector.so -shared src/genetic_connector.os -Llib -lgodot_cpp_bindings
