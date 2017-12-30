#ifndef WORKABLE_NN_H
#define WORKABLE_NN_H

#include <math.h>
#include <limits>
#include <vector>
#include <iostream>

#include "dna.h"

#define TYPE short
#define E 2.71828182845

/**
 * @brief Representa una red neuronal en formato matriz de costes.
 */
class workable_nn {
private:
  /* Matriz de costes que representa para cada elemento ij el peso de un
   * axón que conecte la neurona i con la j o en el caso de i = j el umbral de la neurona. */
  std::vector<std::vector<TYPE>> cost_matrix;

  /* Matriz que representa la existencia o no de un axón que conecta cada neurona i
   * con una j. El caso de i = j no afecta, una neurona siempre existe. */
  std::vector<std::vector<bool>> graph_matrix;

  /*Número de neuronas de entrada, las neuronas desde 0 hasta #input_neurons
   * se consideran neuronas de entrada. Primeras #input_neurons neuronas. */
  unsigned input_neurons;

  /* Número de neuronas de entrada, las neuronas desde 0 hasta #output_neurons
   * se consideran neuronas de entrada. Primeras #output_neurons neuronas. */
  unsigned output_neurons;

  void get_mem (void* mem, unsigned& index, const char* seq, size_t size);
  void copy_mem (char* seq, unsigned& index, const void* mem, size_t size);

  double saturate (double v) const {
    v = v / std::numeric_limits<short>::max() ;
    return v;
  }
public:
  workable_nn () {}
  workable_nn (std::vector<std::vector<std::pair<bool, TYPE>>> cost_matrix, unsigned input,
                                                                            unsigned ouput);

  workable_nn (unsigned n_neurons, unsigned input, unsigned output) {

    cost_matrix.resize(n_neurons);
    graph_matrix.resize(n_neurons);

    for (unsigned i = 0; i < n_neurons; i++) {
      cost_matrix[i].resize(n_neurons);
      graph_matrix[i].resize(n_neurons);
      for (unsigned j = 0; j < n_neurons; j++) {
        graph_matrix[i][j] = rand() % 1;
        cost_matrix[i][j] = 1 + rand() % 10000;
      }
    }
  }

  workable_nn (const workable_nn& aux);

  /**
   * @brief Genera la red neuronal a partir de un adn válido, según la
   * codificación contemplada en #dna
   * @param dna adn empleado para la construcción
   */
  workable_nn (const dna& DNA);

  /**
   * @brief Genera un adn que codifique la información de la red neuronal contenida
   * @return dna Adn con la información de la red neuronal.
   */
  dna to_dna ();

  void operator= (const workable_nn& aux);

  unsigned get_n_axons () const {
    unsigned n_axons = 0;
    unsigned size = graph_matrix.size();
    for (unsigned i = 0; i < size; i++) {
      for (unsigned j = 0; j < size; j++) {
        n_axons += graph_matrix[i][j];
      }
    }
    return n_axons;
  }

  std::vector<std::vector<TYPE>> get_cost_matrix () const { return cost_matrix; }
  std::vector<std::vector<bool>> get_graph_matrix() const { return graph_matrix; }

  /**
   * @brief Calcula la respuesta de la red neuronal ante una entrada
   * @param inputs Datos con los que se alimenta a las neuronas de entrada
   * @param outputs Contendrá los resultados de las neuronas de salida tras la operación
   */
  void calculate (std::vector<double>& inputs, std::vector<double>& outputs);

  /**
   * @brief Imprime la red neuronal como matriz de costes
   */
  void print () const;
};

#endif // WORKABLE_NN_H
