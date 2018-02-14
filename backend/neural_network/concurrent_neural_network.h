/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2018  Daniel Darias Sánchez <dariasteam94@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef WORKABLE_NN_H
#define WORKABLE_NN_H

#include <cmath>
#include <limits>
#include <vector>
#include <iostream>
#include <memory>
#include <functional>
#include <future>
#include <map>
#include <stack>
#include <random>

#include "dna.h"
#include "neuron.h"
#include "feedback_bus.h"

#define TYPE short
#define E 2.71828182845


/**
 * @brief Representa una red neuronal en formato matriz de costes.
 */
class concurrent_neural_network {
private:
  /* Matriz de costes que representa para cada elemento ij el peso de un
   * axón que conecte la neurona i con la j o en el caso de i = j el umbral de la neurona. */
  std::vector<std::vector<TYPE>> cost_matrix;

  /* Matriz que representa la existencia o no de un axón que conecta cada neurona i
   * con una j. El caso de i = j no afecta, una neurona siempre existe. */
  std::vector<std::vector<bool>> graph_matrix;

  /*Número de neuronas de entrada, las neuronas desde 0 hasta #input_neurons
   * se consideran neuronas de entrada. Primeras #input_neurons neuronas. */
  unsigned n_inputs;

  /* Número de neuronas de entrada, las neuronas desde 0 hasta #output_neurons
   * se consideran neuronas de entrada. Primeras #output_neurons neuronas. */
  unsigned n_outputs;

  // NEW STUFF -------------
  std::vector<axon*> input_axons;
  std::vector<axon*> output_axons;

  std::vector<neuron*> neurons;
  std::vector<unsigned> concurrent_steps;

  std::map<unsigned, feedback_bus*> feedbackers;

  void add_feedbacker (unsigned origin_neuron, unsigned destiny_neuron, double w);
  void propagate_feedback ();

  void optimize ();
  void build_from_matrixes ();

  template <class T>
  void delete_row_col (std::vector<std::vector<T>>& original, unsigned node) {
    unsigned size = original.size();
    std::vector<std::vector<T>> aux (size - 1);
    for (auto& row : aux)
      row.resize(size - 1);

    // First quadrant (a)
    for (unsigned i = 0; i < node; i++) {
      for (unsigned j = 0; j < node; j++) {
        aux[i][j] = original[i][j];
      }
    }

    // Second quadrant (b)
    for (unsigned i = 0; i < node; i++) {
      for (unsigned j = node + 1; j < size; j++) {
        aux[i][j - 1] = original[i][j];
      }
    }

    // Third quadrant (c)
    for (unsigned i = node + 1; i < size; i++) {
      for (unsigned j = 0; j < node; j++) {
        aux[i - 1][j] = original[i][j];
      }
    }

    // Fourth quadrant (d)
    for (unsigned i = node + 1; i < size; i++) {
      for (unsigned j = node + 1; j < size; j++) {
        aux[i - 1][j - 1] = original[i][j];
      }
    }

    original = aux;
  }

  /**
   * @brief Delete neurons with no predecesors (excluding inputs and outputs)
   *
   * @param vec_graph p_vec_graph:...
   * @param vec_costs p_vec_costs:...
   * @param inputs p_inputs:...
   * @param outputs p_outputs:...
   */
  void delete_unreachable_nodes ();


  /**
   * @brief Find deathend nodes to delete them and search in cascade new posible
   * deathend nodes. Inputs and outputs neuron won't be affected
   *
   * @param vec p_vec: newral network graph
   * @param inputs p_inputs: number of input neurons
   * @param outputs p_outputs: number of output neurons
   */
  void delete_deathend_nodes ();

  /**
   * @brief Extracts the hidden layers of the net and creates a vector of groups
   * of neurons that can be safely calculated concurrently.
   *
   * @param vec p_vec: Cost matriz of the net
   * @return std::vector< unsigned int > groups of neurons conccurent-safe
   */
  std::vector<unsigned> generate_concurrent_steps ();

  /**
   * @brief Generates a vector containing the number of predecesors of each node
   * This function is used by #generate_concurrent_steps to which neurons can
   * be calculated concurrently.
   *
   * @param vec p_vec:...
   * @return std::vector< unsigned int >
   */
  std::vector<unsigned> generate_visited_nodes ();
  // ------------------------

  void get_mem (void* mem, unsigned& index, const char* seq, size_t size);
  void copy_mem (char* seq, unsigned& index, const void* mem, size_t size);

  double saturate (double v) const {
    v  /= std::numeric_limits<TYPE>::max();
    if (v > 1)
      return 1;
    else if (v < -1)
      return -1;
    else
      return v;
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
public:
  explicit concurrent_neural_network (const std::vector<std::vector<std::pair<bool, TYPE>>>& cost,
                             unsigned inputs, unsigned ouputs);


  /**
  * @brief Random values default constructor
  *
  * @param n_neurons p_n_neurons:...
  * @param input p_input:...
  * @param output ${p_output:...}
  */
  explicit concurrent_neural_network (unsigned n_neurons, unsigned input, unsigned output);

  explicit concurrent_neural_network (const concurrent_neural_network& aux);

  /**
   * @brief Genera la red neuronal a partir de un adn válido, según la
   * codificación contemplada en #dna
   * @param dna adn empleado para la construcción
   */
  concurrent_neural_network (const dna& DNA);

  /**
   * @brief Genera un adn que codifique la información de la red neuronal contenida
   * @return dna Adn con la información de la red neuronal.
   */
  dna to_dna ();

  void operator= (const concurrent_neural_network& aux);

  std::vector<std::vector<TYPE>> get_cost_matrix () const { return cost_matrix; }
  std::vector<std::vector<bool>> get_graph_matrix () const { return graph_matrix; }

  /**
   * @brief Calcula la respuesta de la red neuronal ante una entrada
   * @param inputs Datos con los que se alimenta a las neuronas de entrada
   * @param outputs Contendrá los resultados de las neuronas de salida tras la operación
   */
  bool calculate (std::vector<double>& input_values,
                  std::vector<double>& output_values);

  /**
   * @brief Imprime la red neuronal como matriz de costes
   */
  void print () const;
  unsigned c_steps () { return concurrent_steps.size(); }
};

#endif // WORKABLE_NN_H
