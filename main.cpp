#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <string.h>
#include <random>
#include <climits>
#include <functional>

#include "workable_nn.h"
#include "codified_nn.h"
#include "geneticalgorithm.h"

#define TYPE short
#define N 10
#define NORMALIZE_NUMBER double(100)
#define E 2.71828182845

/* Generador de números aleatorios para el grafo
 * */
std::pair<bool, TYPE> random (unsigned prob) {
  if (std::rand() % 100 < prob)
    return {true, 100 - std::rand() % 200};
  else
    return {false, 0};
}

/* Iterador genérico para aplicar a toda una fila una operación excepto
 * al elemento index. Desde begin hasta end
 * */
void iterate_avoiding_index (std::function<void(unsigned)> func, unsigned begin,
                             unsigned index, unsigned end) {
  for (unsigned k = begin; k < index; k++)
    func(k);
  for (unsigned k = index + 1; k < end; k++)
    func(k);
}

/* Inicializa una matríz de costes
 * */
void random_values_generator (std::vector<std::vector<std::pair<bool, TYPE>>>& cost) {
  cost.resize(N);
  for (unsigned i = 0; i < N; i++) {
    cost[i].resize(N);
    cost[i][i] = {false, 0};
    iterate_avoiding_index([&](unsigned j){
      cost[i][j] = random(15);
    }, 0, i, N);
  }
}

dna cross_dna (dna& A, dna& B) {
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

void add_mutations (dna& DNA) {
  unsigned old_sz = DNA.byte_sz;
//  memcpy(&old_sz, DNA.sequence, sizeof(old_sz));
  unsigned new_sz = old_sz;
/*
  if (rand() % 12 < 1) {
    new_sz += (rand() % 10 - 5);
    // ajusta el mínimo de neuronas a al menos las de entrada y salida
    if (new_sz < (DNA.input_neurons + DNA.output_neurons))
      new_sz += (DNA.input_neurons + DNA.output_neurons) - new_sz + 1;

    char* sequence = (char*)malloc(new_sz);
    memcpy(sequence, DNA.sequence, std::min(new_sz, old_sz));

    free (DNA.sequence);
    DNA.sequence = sequence;

    memcpy(DNA.sequence, &new_sz, sizeof(new_sz));
  }
*/
  unsigned first_index;
  unsigned second_index;

  first_index = old_sz;
  /*
  if (new_sz > old_sz)

  else
    first_index = new_sz;
  */

//  second_index = new_sz;
  // TODO Aparentemente los mejores resultados los da el método a nivel de bit

  // Genera mutaciones para el genoma ya existente
  // Método numérico
  /*
  for (unsigned i = sizeof(TYPE); i < first_index; i+=sizeof(old_sz)) {
    int a = 100 - std::rand() % 200;
    int b;
    memcpy(&b, DNA.sequence + i, sizeof(TYPE));
    b += a;    
    if (b < -100)
      b = -100;
    else if (b > 100)
      b = 100;
    memcpy(DNA.sequence + i, &b, sizeof(TYPE));
  }
  */
  // Método a nivel de bit 

  for (unsigned i = sizeof(unsigned); i < first_index; i+=sizeof(TYPE)) {
    if (rand() % 100 < 1) {
      DNA.sequence[i] ^= 1;
      i+=sizeof(bool) + 1;
      for (unsigned j = 0; j < 2; j++) {
        DNA.sequence[i] ^= 1 << j;
      }
    }
  }

/*
  // Genera la nueva información
  // Método numético
  for (unsigned i = first_index; i < second_index; i+=sizeof(old_sz)) {
    int a = 2 - std::rand() % 4;
    int b;
    memcpy(&b, DNA.sequence + i, sizeof(TYPE));
    b += a;
    if (b < -100)
      b = -100;
    else if (b > 100)
      b = 100;
    memcpy(DNA.sequence + i, &b, sizeof(TYPE));
  }
*/
  // Método a nivel de bit
/*
  for (unsigned i = first_index; i < second_index; i+=sizeof(TYPE)) {
    for (unsigned j = 0; j < 3; j++)
      DNA.sequence[i] ^= (rand() % 7 < 1) << j;
    for (unsigned j = 4; j < sizeof(TYPE); j++)
      DNA.sequence[i] &= 0 << j;
  }
*/
}

double evaluate (workable_nn& w, std::vector<double> input) {
  std::vector<std::vector<TYPE>> matrix = w.get_cost_matrix();
  unsigned ac = 0;

  std::vector<double> output;
  w.calculate(input, output);
  double v = 0;

  unsigned i = 1;
  for (auto& e : output) {
    v += e;
  }
  return v;
}

int main(int argc, char **argv) {
  srand(time(nullptr));

  auto cross = [](dna& A, dna& B) -> dna{
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
  };

  auto evaluate = [](const dna& DNA) -> double {
    workable_nn w (DNA);
    std::vector<std::vector<TYPE>> matrix = w.get_cost_matrix();
    unsigned ac = 0;

    std::vector<double> output;
    std::vector<double> input = {1, 1, 1, 1};
    w.calculate(input, output);
    double v = 0;

    unsigned i = 1;
    for (auto& e : output) {
      v += e;
    }
    return v;
  };

  auto mutate = [](dna& DNA) {
    unsigned first_index = DNA.byte_sz;
    for (unsigned i = sizeof(unsigned); i < first_index; i+=sizeof(TYPE)) {
      if (rand() % 100 < 1) {
        DNA.sequence[i] ^= 1;
        i+=sizeof(bool) + 1;
        for (unsigned j = 0; j < 2; j++) {
          DNA.sequence[i] ^= 1 << j;
        }
      }
    }
  };

  std::vector<std::vector<std::pair<bool, TYPE>>> v1;
  std::vector<std::vector<std::pair<bool, TYPE>>> v2;

  random_values_generator(v1);
  random_values_generator(v2);

  workable_nn parent_1 (v1, 4, 2);
  workable_nn parent_2 (v2, 4, 2);

  dna serialized_nn_1 = parent_1.to_dna();
  dna serialized_nn_2 = parent_2.to_dna();

  GeneticAlgorithm<dna> genetic (cross, mutate, evaluate, 10, 2);
  genetic.set_initial_poblation({serialized_nn_1, serialized_nn_2});

  genetic.step();

  /*
  std::vector<dna> sons_dna (12);
  std::vector<workable_nn> sons_wrk (12);

  char user_input = 1;
  while (user_input != 'q') {
    sons_dna[0] = cross_dna(serialized_nn_1, serialized_nn_2);
    sons_wrk[0] = workable_nn (sons_dna[0]);

    sons_dna[10] = serialized_nn_1;
    sons_dna[11] = serialized_nn_2;

    sons_wrk[10] = workable_nn (sons_dna[10]);
    sons_wrk[11] = workable_nn (sons_dna[11]);

    // Generar 10 clones y aplicar mutaciones
    for (unsigned i = 1; i < 10; i++) {
      sons_dna[i] = sons_dna[0];
      add_mutations(sons_dna[i]);
      sons_wrk[i] = workable_nn (sons_dna[i]);
    }

    int best_index = -1;
    double best_distance = 0.00001;
    double optimal_value = 10.93;

    unsigned i = 0;
    unsigned counter = 0;
    while (best_index == -1) {
      i = 0;
      if (++counter > 1000)
        best_index = 0;
      for (auto& nn : sons_wrk) {
        double eval_value1 = evaluate(nn, { 1, 1, 1, 1});
        double eval_value2 = evaluate(nn, {-1,-1,-1,-1});

        double d2 = fabs(eval_value1) + eval_value2;


        if (eval_value1 < best_distance && eval_value1 <= 0 && eval_value2 >= 0) {
          best_distance = eval_value1;
          best_index = i;
        }
        i++;
      }
    }

    system("clear");
    std::cout << "\n\n\nFormato matricial\n\n\n" << std::flush;   
    std::cout << "Índice " << best_index << std::endl;
    sons_wrk[best_index].print();
    std::cout << "Con un valor de " << "\n" <<
                 evaluate(sons_wrk[best_index], { 1, 1, 1, 1}) << "\n" <<
                 evaluate(sons_wrk[best_index], {-1,-1,-1,-1}) << "\n" <<
                 evaluate(sons_wrk[best_index], {-1, 0, 0,-1}) << std::endl;

    scanf("%c", &user_input);

    free(serialized_nn_2.sequence);
    free(serialized_nn_1.sequence);

    serialized_nn_1 = sons_wrk[std::rand() % 12].to_dna();
    serialized_nn_2 = sons_wrk[best_index].to_dna();
  }
  */

  return 0;
}
