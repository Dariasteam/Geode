#include <core/Godot.hpp>
#include <Reference.hpp>

#include <vector>

#include "geneticalgorithm.h"
#include "dna.h"
#include "workable_nn.h"

#define N 10


using namespace godot;


void mutate (dna& DNA) {
  unsigned first_index = DNA.byte_sz;
  for (unsigned i = sizeof(unsigned); i < first_index; i+=sizeof(TYPE)) {
    if (rand() % 50 < 1) {
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

double evaluate (const dna& A) {
  return 0;
}

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


std::pair<bool, TYPE> random (unsigned prob) {
  if (std::rand() % 100 < prob)
    return {true, 100 - std::rand() % 200};
  else
    return {false, 0};
}


void iterate_avoiding_index (std::function<void(unsigned)> func, unsigned begin,
                             unsigned index, unsigned end) {
  for (unsigned k = begin; k < index; k++)
    func(k);
  for (unsigned k = index + 1; k < end; k++)
    func(k);
}


void random_values_generator (std::vector<std::vector<std::pair<bool, TYPE>>>& cost) {
  cost.resize(N);
  for (unsigned i = 0; i < N; i++) {
    cost[i].resize(N);
    cost[i][i] = {false, 0};
    iterate_avoiding_index([&](unsigned j){
      cost[i][j] = random(0);
    }, 0, i, N);
  }
}

void create_algorithm () {
  GeneticAlgorithm<dna>(cross, mutate, evaluate, 80, 20);
}

class GeneticConnector : public GodotScript<Reference> {
  GODOT_CLASS(GeneticConnector);
private:
  GeneticAlgorithm <dna>* genetic;
public:
  GeneticConnector()
  {

/*
    srand(time(nullptr));

    std::vector<std::vector<std::pair<bool, TYPE>>> v1;
    std::vector<std::vector<std::pair<bool, TYPE>>> v2;

    random_values_generator(v1);
    random_values_generator(v2);

    workable_nn parent_1 (v1, 4, 1);
    workable_nn parent_2 (v2, 4, 1);

    dna serialized_nn_1 = parent_1.to_dna();
    dna serialized_nn_2 = parent_2.to_dna();

    std::vector<dna> initial_candidates = {serialized_nn_1, serialized_nn_2};
    genetic.set_initial_poblation(initial_candidates);
*/
  }

/*
  Array nn_to_array (workable_nn& wn) {
    Array array_cost;
    Array array_graph;

    auto cost_matrix = wn.get_cost_matrix();
    auto graph_matrix = wn.get_graph_matrix();

    int size = cost_matrix.size();

    for (int i = 0; i < size; i++) {
      Array aux;
      for (int j = 0; i < size; j++) {
        aux.push_back(cost_matrix[i][j]);
      }
      array_cost.push_back(aux);
    }

    for (int i = 0; i < size; i++) {
      Array aux;
      for (int j = 0; i < size; j++) {
        aux.push_back(graph_matrix[i][j]);
      }
      array_cost.push_back(aux);
    }

    Array final_array;

    final_array.push_back (array_cost);
    final_array.push_back (array_graph);

    return final_array;
  }
*/
  Array get_new_poblation (void) {
/*
    genetic.semi_step();
    std::vector<dna> poblation = genetic.get_poblation();
    Array array;
    for (auto& element : poblation) {
      Array aux;
      aux.push_back(element.sequence);
      aux.push_back(element.byte_sz);
      aux.push_back(element.input_neurons);
      aux.push_back(element.output_neurons);
      array.push_back(aux);
    }

    return array;
    */
  }

  int set_score (Array o) {
    Array aux = o[0];
    int m = aux[1];
    Godot::print (m);
    return m;
  }

  static void _register_methods() {
    register_method("set_score",  &GeneticConnector::set_score);
    register_method("generate_new_poblation",  &GeneticConnector::get_new_poblation);

    register_property((char *)"base/name", &GeneticConnector::_name, String("GeneticConnector"));
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
