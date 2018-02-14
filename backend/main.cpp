#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <string.h>
#include <random>
#include <climits>
#include <functional>
#include <fstream>

#include "neural_network/concurrent_neural_network.h"
#include "genetic_algorithm/geneticalgorithm.h"

#define N 100

/**
 * @brief cross Cross operator for the genetic algorithm. Cuts both father
 * and mother by half to create the son.
 * @param A father
 * @param B mother
 * @return son
 */
dna cross (const dna& A, const dna& B) {
  return A;
  unsigned a_size;
  unsigned b_size;

  memcpy(&a_size, A.sequence, sizeof(a_size));
  memcpy(&b_size, B.sequence, sizeof(b_size));

  unsigned final_size = (a_size / 2) + (b_size / 2);
  //std::cout << a_size << " " << b_size << std::endl;

  char* sequence = new char[(A.byte_sz / 2) + (B.byte_sz / 2)];

  memcpy(sequence, A.sequence, (A.byte_sz / 2));
  memcpy(sequence + (A.byte_sz / 2), B.sequence + (B.byte_sz / 2), (B.byte_sz / 2));
  memcpy(sequence, &final_size, sizeof(final_size));

  return {sequence, A.byte_sz, A.input_neurons, A.output_neurons};
}

/**
 * @brief evaluate_q Evaluator for the genetic algorithm. Fitness function is
 * the dsitance between the output values when the inputs are 1,1,1 and -1,-1,-1.
 * @param DNA dna to create the neural network.
 * @return fitness values.
 */
double evaluate (const dna& DNA) {
  concurrent_neural_network w (DNA);

  std::vector<double> output;
  std::vector<double> input_a = {1, 1, 1};
  //std::vector<double> input_b = {-1,-1,-1};

  double v1 = 0;
  //double v2 = 0;

  w.calculate(input_a, output);
  for (auto& e : output)
    v1 += e;
  /*
  w.calculate(input_b, output);
  for (auto& e : output)
    v2 += e;
  */

  return v1;

  /*
  if ((v1 > 0 && v2 > 0) || (v1 < 0 && v2 < 0))
    return fabs(fabs(v1) - v2);
  else
    return fabs(v1) + fabs(v2);
  */
}

/**
 * @brief mutate Mutate operator for the genetic algorithm.
 * @param DNA dna to be mutated.
 * @param mutation_rate probability of changing a bit between [100,1]
 */
void mutate (dna& DNA, unsigned mu) {
  unsigned first_index = DNA.byte_sz;
  for (unsigned i = sizeof(unsigned); i < first_index; i+=sizeof(TYPE)) {
    if (rand() % mu < 1) {
      DNA.sequence[i] ^= 1;
      i+=sizeof(bool);
      for (unsigned j = 0; j < sizeof(TYPE); j++) {
        for (unsigned k = 0; k < 8; k++) {
          if (rand() % 8 < 1)
            DNA.sequence[i+j] ^= 1 << k;
        }
      }
    } else {
      i+=sizeof(bool);
    }
  }
}

void read_net_from_file (std::string filename,
                         std::vector<std::vector<std::pair<bool, TYPE>>>& graph) {
  std::ifstream file;

  graph.resize(0);

  unsigned size;

  file.open(filename);
  if (file.is_open()) {
    file >> size;
    graph.resize(size);

    // READ GRAPH

    for (unsigned i = 0; i < size; i++) {
      graph[i].resize(size);
      for (unsigned j = 0; j < size; j++) {
        bool aux;
        file >> aux;
        graph[i][j].first = aux;
      }
    }

    // READ COSTS

    for (unsigned i = 0; i < size; i++) {
      for (unsigned j = 0; j < size; j++) {
        double aux;
        file >> aux;
        graph[i][j].second = aux;
      }
    }
  }
}

int main(int argc, char **argv) {
  srand(time(nullptr));

  concurrent_neural_network parent_1 (N, 3, 1);
  concurrent_neural_network parent_2 (N, 3, 1);

  dna serialized_nn_1 = parent_1.to_dna();
  dna serialized_nn_2 = parent_2.to_dna();

  GeneticAlgorithm<dna> genetic (cross, mutate, evaluate, 50, 10, 15);
  genetic.set_population_parameters(5,2,10);
  std::vector<dna> initial_candidates = {serialized_nn_1};
  genetic.set_initial_population(initial_candidates);

  std::vector<std::vector<std::pair<bool, TYPE>>> vec;
  read_net_from_file("testfile.dat", vec);

  concurrent_neural_network w (vec, 1, 1);

  double last = 0;
  std::vector<double> input_a = {1};
  std::vector<double> output;

  while (true) {
    double val = 0;
    w.calculate(input_a, output);
    for (auto& e : output)
      val += e;

    if (val < last)
      std::cout << "Problema" << std::endl;
    if (val > last)
      std::cout << "Recupero" << std::endl;
    std::cout << val << " vs " << last << std::endl;

    last = val;
  }
  return 0;
}
