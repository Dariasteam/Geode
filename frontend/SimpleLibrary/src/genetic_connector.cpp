#include <core/Godot.hpp>
#include <Reference.hpp>
#include <Node2D.hpp>

#include <vector>
#include <iostream>
#include <functional>

#include "dna.h"
#include "geneticalgorithm.h"
#include "workable_nn.h"

using namespace godot;

dna cross (const dna& A, const dna& B) {

  unsigned a_size;
  unsigned b_size;

  memcpy(&a_size, A.sequence, sizeof(a_size));
  memcpy(&b_size, B.sequence, sizeof(b_size));

  unsigned final_size = (a_size / 2) + (b_size / 2);
  std::cout << a_size << " " << b_size << std::endl;

  char* sequence = (char*)malloc((A.byte_sz / 2) + (B.byte_sz / 2));

  memcpy(sequence, A.sequence, (A.byte_sz / 2));
  memcpy(sequence + (A.byte_sz / 2), B.sequence + (B.byte_sz / 2), (B.byte_sz / 2));
  memcpy(sequence, &final_size, sizeof(final_size));

  return {sequence, A.byte_sz, A.input_neurons, A.output_neurons};
}

double evaluate (const dna& DNA) {

}

void mutate (dna& DNA) {
  unsigned first_index = DNA.byte_sz;
  for (unsigned i = sizeof(unsigned); i < first_index; i+=sizeof(TYPE)) {
    if (rand() % 70 < 1) {
      DNA.sequence[i] ^= 1;
      i+=sizeof(bool);
      DNA.sequence[i] ^= 1;
      for (unsigned j = 0; j < sizeof(TYPE); j++) {
        for (unsigned k = 0; k < 8; k++) {
          if (rand() % 8 < 1)
            DNA.sequence[i+k] ^= 1 << k;
        }
      }
    } else {
      i+=sizeof(bool);
    }
  }
}

class GeneticConnector : public GodotScript<Node2D> {
  GODOT_CLASS(GeneticConnector);
private:

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

  GeneticConnector() :
  genetic (cross, mutate, evaluate, 50, 10)
  {
    Godot::print("Algoritmo genético construido");
  }

  void semi_step (void) {
    genetic.semi_step();
  }

  void set_genetic_parameters (unsigned poblation_size, unsigned candidates_size) {
    genetic.set_poblation_parameters (poblation_size, candidates_size);
  }

  void set_evaluations (Array a_evaluations) {
    unsigned size = a_evaluations.size();
    std::vector <double> evaluations (size);
    for (unsigned i = 0; i < size; i++) {
      evaluations[i] = (double)a_evaluations[i];
    }

    genetic.set_external_evaluations(evaluations);
  }

  void generate_initial_poblation (unsigned n_neurons, unsigned n_inputs,
                                   unsigned n_outputs) {
    workable_nn parent_1 (n_neurons, n_inputs, n_outputs);
    workable_nn parent_2 (n_neurons, n_inputs, n_outputs);

    dna serialized_nn_1 = parent_1.to_dna();
    dna serialized_nn_2 = parent_2.to_dna();

    std::vector<dna> initial_candidates = {serialized_nn_1, serialized_nn_2};
    genetic.set_initial_poblation(initial_candidates);
    Godot::print ("Población inicial generada");
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

      a_poblation.push_back(matrix_graph_to_array (v2));
      a_poblation.push_back(matrix_cost_to_array (v1));
    }
    return a_poblation;
  }

  static void _register_methods() {
    register_method("semi_step",  &GeneticConnector::semi_step);
    register_method("get_poblation",  &GeneticConnector::get_poblation);
    register_method("set_evaluations", &GeneticConnector::set_evaluations);
    register_method("generate_initial_poblation",&GeneticConnector::generate_initial_poblation);
    register_method("set_genetic_parameters",&GeneticConnector::set_genetic_parameters);

    /**
     * How to register exports like gdscript
     * export var _name = "GeneticConnector"
     **/
    register_property((char *)"base/name", &GeneticConnector::_name, String("GeneticConnector"));

    /** For registering signal **/
    // register_signal<GeneticConnector>("signal_name");
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
  register_class<GeneticConnector>();
}
