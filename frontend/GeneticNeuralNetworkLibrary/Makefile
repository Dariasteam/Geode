all:
	# DNA
	clang -fPIC -o src/dna.os -c ../../backend/neural_network/dna.cpp  -g -O3 -std=c++14 -Icpp_bindings/include -Igodot_headers -Isrc
	# NeuralNetwork
	clang -fPIC -o src/neural_network.os -c ../../backend/neural_network/workable_nn.cpp -g -O3 -std=c++14 -Icpp_bindings/include -Igodot_headers
	# Genetic Algorithm Connector
	clang -fPIC -o src/genetic_connector.os -c src/genetic_connector.cpp  -g -O3 -std=c++14 -Icpp_bindings/include -Igodot_headers -Isrc
	clang -o lib/libgenetic.so -shared src/genetic_connector.os src/dna.os src/neural_network.os  -Llib -lgodot_cpp_bindings
	# Neural Network Connector
	clang -fPIC -o src/neural_network_connector.os -c src/neural_network_connector.cpp  -g -O3 -std=c++14 -Icpp_bindings/include -Igodot_headers -Isrc
	clang -o lib/lbneural.so -shared src/neural_network_connector.os src/dna.os src/neural_network.os  -Llib -lgodot_cpp_bindings
clean:
	rm -rf src/*.os src/*.so
