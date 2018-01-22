#include <core/Godot.hpp>
#include <Reference.hpp>

#include <vector>
#include <iostream>
#include <functional>
#include <utility>

#include "../../../backend/neural_network/dna.h"
#include "../../../backend/neural_network/workable_nn.h"

using namespace godot;

class NeuralNetworkConnector : public GodotScript<Reference> {
  GODOT_CLASS(NeuralNetworkConnector);
private:

  workable_nn neural_network;

  // Genera el array de la forma adecuada para la red neuronal
  std::vector<std::vector<std::pair<bool, TYPE>>> merge_arrays (Array& a_graph,
                                                                Array& a_cost) {
    unsigned size = a_graph.size();
    std::vector<std::vector<std::pair<bool, TYPE>>> vec(size);
    for (unsigned i = 0; i < size; i++) {
      vec[i].resize(size);
      Array row_graph = a_graph[i];
      Array row_cost = a_cost[i];
      for (unsigned j = 0; j < size; j++) {
        vec[i][j] = std::make_pair<bool, TYPE> (row_graph[j], row_cost[j]);
      }
    }

    return vec;
                                                                }

public:

  NeuralNetworkConnector() {
    //Godot::print("Red neuronal construida");
  }

  // Establece el contenido inicial de la red neuronal
  void set_content (Array a_graph, Array a_cost, unsigned inputs, unsigned outputs) {
    auto vec = merge_arrays (a_graph, a_cost);
    neural_network = workable_nn (vec, inputs, outputs);
  }

  // Devuelve la salida de la red neuronal para la entrada dada
  Array evaluate (Array a_inputs) {
    unsigned i_size = a_inputs.size();

    std::vector<double> inputs (i_size);
    std::vector<double> outputs;

    for (unsigned i = 0; i < i_size; i++)
      inputs[i] = a_inputs[i];

    neural_network.calculate (inputs, outputs);

    Array a_outputs;
    unsigned o_size = outputs.size();

    for (unsigned i = 0; i < o_size; i++)
      a_outputs.push_back (outputs[i]);

    return a_outputs;
  }

  static void _register_methods() {
    register_method("set_content",  &NeuralNetworkConnector::set_content);
    register_method("evaluate",  &NeuralNetworkConnector::evaluate);

    /**
     * How to register exports like gdscript
     * export var _name = "NeuralNetworkConnector"
     **/
    register_property((char *)"base/name", &NeuralNetworkConnector::_name, String("NeuralNetworkConnector"));

    /** For registering signal **/
    // register_signal<NeuralNetworkConnector>("signal_name");
  }

  String _name;
};

/** GDNative Initialize **/
GDNATIVE_INIT(godot_gdnative_init_options *options) {
}

/** GDNative Terminate **/
GDNATIVE_TERMINATE(godot_gdnative_terminate_options *options) {

}

/** NativeScript Initialize **/
NATIVESCRIPT_INIT() {
  register_class<NeuralNetworkConnector>();
}
