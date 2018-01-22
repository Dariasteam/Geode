#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <string.h>
#include <random>
#include <climits>
#include <functional>

#include "neural_network/workable_nn.h"
#include "genetic_algorithm/geneticalgorithm.h"

#define N 22

std::pair<bool, TYPE> random (unsigned prob) {
  if (std::rand() % 100 < prob)
    return {true, 100 - std::rand() % 200};
  else
    return {false, 0};
}

/* Iterador genérico para aplicar a toda una fila una operación excepto
 * al elemento index. Desde begin hasta end
 * */

/**
 * @brief iterate_avoiding_index Generic iterator that aplies the same operation
 * to all elements in a collection except the element at index. From begin to end.
 * @param func Operation to apply
 * @param begin
 * @param index element for which operation won't be applied
 * @param end
 */
void iterate_avoiding_index (std::function<void(unsigned)> func, unsigned begin,
                             unsigned index, unsigned end) {
  for (unsigned k = begin; k < index; k++)
    func(k);
  for (unsigned k = index + 1; k < end; k++)
    func(k);
}

/**
 * @brief random_values_generator Initialize a graph/cost matrix
 * @param cost matrix to be initialized
 */
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

/**
 * @brief cross Cross operator for the genetic algorithm. Cuts both father
 * and mother by half to create the son.
 * @param A father
 * @param B mother
 * @return son
 */
dna cross (const dna& A, const dna& B) {  
  unsigned a_size;
  unsigned b_size;

  memcpy(&a_size, A.sequence, sizeof(a_size));
  memcpy(&b_size, B.sequence, sizeof(b_size));

  unsigned final_size = (a_size / 2) + (b_size / 2);
  std::cout << a_size << " " << b_size << std::endl;

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
  workable_nn w (DNA);

  std::vector<double> output;
  std::vector<double> input_a = { 1, 1, 1};
  std::vector<double> input_b = {-1,-1,-1};

  double v1 = 0;
  double v2 = 0;

  w.calculate(input_a, output);
  for (auto& e : output)
    v1 += e;

  w.calculate(input_b, output);
  for (auto& e : output)
    v2 += e;      

  if ((v1 > 0 && v2 > 0) || (v1 < 0 && v2 < 0))
    return fabs(fabs(v1) - v2);
  else
    return fabs(v1) + fabs(v2);
}

/**
 * @brief mutate Mutate operator for the genetic algorithm.
 * @param DNA dna to be mutated.
 * @param mutation_rate probability of changing a bit between [100,1]
 */
void mutate (dna& DNA, unsigned mutation_rate) {
  unsigned first_index = DNA.byte_sz;
  for (unsigned i = sizeof(unsigned); i < first_index; i+=sizeof(TYPE)) {
    if (rand() % 100 < mutation_rate) {
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

int main(int argc, char **argv) {
  srand(time(nullptr));

  std::vector<std::vector<std::pair<bool, TYPE>>> v1;
  std::vector<std::vector<std::pair<bool, TYPE>>> v2;

  random_values_generator(v1);
  random_values_generator(v2);

  workable_nn parent_1 (v1, 3, 1);
  workable_nn parent_2 (v2, 3, 1);

  dna serialized_nn_1 = parent_1.to_dna();
  dna serialized_nn_2 = parent_2.to_dna();

  GeneticAlgorithm<dna> genetic (cross, mutate, evaluate, 50, 15, 10);
  genetic.set_poblation_parameters(16,10,10);
  std::vector<dna> initial_candidates = {serialized_nn_1, serialized_nn_2};
  genetic.set_initial_poblation(initial_candidates); 

  while (true) {
    genetic.step();  
    std::cout << "Next step" << std::endl;
  }
  return 0;
}
