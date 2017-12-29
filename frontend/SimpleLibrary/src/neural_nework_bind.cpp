#include <core/Godot.hpp>
#include <Reference.hpp>
#include <Node2D.hpp>
#include <ResourceLoader.hpp>
#include <Texture.hpp>
#include <Sprite.hpp>
#include <RayCast2D.hpp>

#include <vector>
#include <iostream>
#include <functional>

#include "dna.h"
#include "geneticalgorithm.h"
#include "workable_nn.h"

using namespace godot;

class NeuralNetwork : public GodotScript<Node2D> {
  GODOT_CLASS(NeuralNetwork);
private:

  std::function <dna (dna& i, dna& u)> aa = [](dna& i, dna& u){ return i; };
  std::function <void (dna& i)> bb = [](dna& i){};
  std::function <double (const dna&)> cc = [](const dna& a){ return 2;};

  GeneticAlgorithm<dna> genetic;

  Array matrix_cost_to_array (std::vector<std::vector<short>>& matrix) {
    Array a_cost;
    unsigned size = matrix.size();
    for (unsigned i = 0; i < size; i++) {
      Array row;
      for (unsigned j = 0; j < size; j++) {
        row.push_back((short)matrix[i][j]);
      }
      a_cost.push_back(row);
    }
    return a_cost;
  }

  Array matrix_graph_to_array (std::vector<std::vector<bool>>& matrix) {
    Array a_cost;
    unsigned size = matrix.size();
    for (unsigned i = 0; i < size; i++) {
      Array row;
      for (unsigned j = 0; j < size; j++) {
        row.push_back((bool)matrix[i][j]);
      }
      a_cost.push_back(row);
    }
    return a_cost;
  }

public:

  NeuralNetwork() :
    genetic (aa, bb, cc, 10, 10)
    {
    Godot::print("Algoritmo genético construido");
  }

  void semi_step (void) {
    genetic.semi_step();
    Godot::print ("Avanzado medio paso");
  }

  void set_evaluations (Array a_evaluations) {
    unsigned size = a_evaluations.size();
    std::vector <double> evaluations (size);
    for (unsigned i = 0; i < size; i++)
      evaluations[i] = (double)a_evaluations[i];

    genetic.set_external_evaluations(evaluations);
  }

  void generate_initial_poblation () {
    std::vector<std::vector<std::pair<bool, short>>> v = {
      {{0, 0}, {0, 0}, {0, 0}, {0, 0}},
      {{0, 0}, {0, 0}, {0, 0}, {0, 0}},
      {{0, 0}, {0, 0}, {0, 0}, {0, 0}},
      {{0, 0}, {0, 0}, {0, 0}, {0, 0}},
      {{0, 0}, {0, 0}, {0, 0}, {0, 0}},
    };

    workable_nn parent_1 (v, 4, 1);
    workable_nn parent_2 (v, 4, 1);

    dna serialized_nn_1 = parent_1.to_dna();
    dna serialized_nn_2 = parent_2.to_dna();

    std::vector<dna> initial_candidates = {serialized_nn_1, serialized_nn_2};
    genetic.set_initial_poblation(initial_candidates);
  }

  Array get_poblation () {
    Array a_poblation;
    std::vector<dna> poblation = genetic.get_poblation();

    unsigned size = poblation.size();

    // generar individuos en forma de red neuronal
    for (unsigned i = 0; i < size; i++) {
      Array a_cost;
      Array a_graph;
      workable_nn aux_nn (poblation[i]);

      auto v1 = aux_nn.get_cost_matrix ();
      auto v2 = aux_nn.get_graph_matrix ();

      a_cost = matrix_cost_to_array (v1);
      a_cost = matrix_graph_to_array (v2);

      a_poblation.push_back(a_cost);
      a_poblation.push_back(a_graph);
    }

    return a_poblation;
  }

  static void _register_methods() {
    register_method("semi_step",  &NeuralNetwork::semi_step);
    register_method("get_poblation",  &NeuralNetwork::get_poblation);
    register_method("set_evaluations", &NeuralNetwork::set_evaluations);
    register_method("generate_initial_poblation",&NeuralNetwork::generate_initial_poblation);

    /**
     * How to register exports like gdscript
     * export var _name = "NeuralNetwork"
     **/
    register_property((char *)"base/name", &NeuralNetwork::_name, String("NeuralNetwork"));

    /** For registering signal **/
    // register_signal<NeuralNetwork>("signal_name");
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
  register_class<NeuralNetwork>();
}
