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

#include "concurrent_neural_network.h"

concurrent_neural_network::concurrent_neural_network(
                         const std::vector<std::vector<std::pair<bool, TYPE>>>& cost,
                         unsigned inputs, unsigned outputs) :
  n_inputs (inputs),
  n_outputs (outputs)
{
  // RETRIEVE THE NET -------------------------------------------------------

  unsigned size = cost.size();
  if (size > 0) {
    cost_matrix.resize(size);
    graph_matrix.resize(size);

    for (unsigned i = 0; i < size; i++) {
      cost_matrix[i].resize(size);
      graph_matrix[i].resize(size);
      for (unsigned j = 0; j < size; j++) {
        graph_matrix[i][j] = cost[i][j].first;
        cost_matrix[i][j] = cost[i][j].second;
      }
    }
    optimize();
    build_from_matrixes();
  }
}

concurrent_neural_network::concurrent_neural_network(unsigned n_neurons,
                                                     unsigned input,
                                                     unsigned output) :
  n_inputs (input),
  n_outputs (output)

  {
  cost_matrix.resize(n_neurons);
  graph_matrix.resize(n_neurons);

  for (unsigned i = 0; i < n_neurons; i++) {
    cost_matrix[i].resize(n_neurons);
    graph_matrix[i].resize(n_neurons);
    for (unsigned j = 0; j < n_neurons; j++) {
      graph_matrix[i][j] = rand() % 1;
      cost_matrix[i][j] = 1000 - rand() % 2000;
    }
  }
}

void concurrent_neural_network::optimize() {
  unsigned old_size = 0;
  unsigned new_size = 0;
  unsigned counter = 0;

  //  OPTIMIZE THE NET  -----------------------------------------------------

  do {
      counter++;
      old_size = graph_matrix.size();
      delete_unreachable_nodes();
      delete_deathend_nodes();
      new_size = graph_matrix.size();
    } while (old_size != new_size);

  // CALCULATE CONCURRENT STEPS -------------------------------------------

  concurrent_steps = generate_concurrent_steps();
}


void concurrent_neural_network::build_from_matrixes() {
  // BUILD THE NET ----------------------------------------------------------

  unsigned size = graph_matrix.size();
  neurons.resize(size);

  // regular neurons
  for (unsigned i = 0; i < size - n_outputs; i++)
    neurons[i] = new neuron();

  // output neurons (can check no inputs)
  for (unsigned i = size - n_outputs; i < size; i++)
    neurons[i] = new output_neuron();

  // Generate the net
  for (unsigned i = 0; i < size; i++) {
    // diagonal (threshold)
    neurons[i]->set_threshold(saturate(cost_matrix[i][i]));

    // upper triangle (propagative)
    for (unsigned j = i + 1; j < size; j++) {
      if (graph_matrix[i][j]) {
        axon* aux = new axon(saturate(cost_matrix[i][j]));
        neurons[i]->add_output(aux);
        neurons[j]->add_input(aux);
      }
    }
    // lower triangle (feedbacker)
    for (unsigned j = 0; j < i; j++) {
      if (graph_matrix[i][j]) {
        add_feedbacker(i, j, saturate(cost_matrix[i][j]));
      }
    }
  }

  // Generate the inputs axons
  input_axons.resize(n_inputs);
  for (unsigned i = 0; i < n_inputs; i++) {
    axon* aux = new axon(1);
    neurons[i]->add_input(aux);
    input_axons[i] = aux;
  }

  // Generate the outputs axons
  output_axons.resize(n_outputs);
  for (unsigned i = 0; i < n_outputs; i++) {
    axon* aux = new axon(1);
    neurons[size - n_outputs + i]->add_output(aux);
    output_axons[i] = aux;
  }
}

concurrent_neural_network::concurrent_neural_network (const dna& DNA)  :
  n_inputs (DNA.input_neurons),
  n_outputs (DNA.output_neurons)
{

  if (n_outputs > 30)
    std::cout << "Error máximo supremo" << std::endl;

  unsigned index = 0;
  const char* seq = DNA.sequence;

  unsigned size;
  get_mem(&size, index, seq, sizeof(size));

  cost_matrix.resize(size);
  graph_matrix.resize(size);

  for (unsigned i = 0; i < size; i++) {
    cost_matrix[i].resize(size);
    graph_matrix[i].resize(size);
    for (unsigned j = 0; j < size; j++) {
      bool b;
      get_mem(&b, index, seq, sizeof(b));
      graph_matrix[i][j] = b;
      get_mem(&cost_matrix[i][j], index, seq, sizeof(TYPE));
    }
  }

  optimize();
  build_from_matrixes();
}

void concurrent_neural_network::get_mem (void* mem, unsigned& index, const char* seq, size_t size) {
  memcpy(mem, seq + index, size);
  index += size;
}

void concurrent_neural_network::copy_mem (char* seq, unsigned& index, const void* mem, size_t size) {
  memcpy(seq + index, mem, size);
  index += size;
}


dna concurrent_neural_network::to_dna() {
  unsigned matrix_size = cost_matrix.size();

  unsigned n_bytes = (matrix_size * matrix_size * (sizeof(TYPE) + sizeof(bool))) +
                      sizeof(unsigned);

  char* sequence = new char[n_bytes];
  unsigned index = 0;

  unsigned c = cost_matrix.size();
  copy_mem(sequence, index, &c, sizeof(c));

  for (unsigned i = 0; i < matrix_size; i++) {
    for (unsigned j = 0; j < matrix_size; j++) {
      bool b = graph_matrix[i][j];
      copy_mem(sequence, index, &b, sizeof(bool));
      copy_mem(sequence, index, &cost_matrix[i][j], sizeof(TYPE));
    }
  }

  return {sequence, n_bytes, n_inputs, n_outputs};
}

concurrent_neural_network::concurrent_neural_network(const concurrent_neural_network &aux) :
  cost_matrix (aux.cost_matrix),
  graph_matrix (aux.graph_matrix),
  n_inputs (aux.n_inputs),
  n_outputs (aux.n_outputs)
{
  build_from_matrixes();
}

void concurrent_neural_network::operator=(const concurrent_neural_network &aux) {
  cost_matrix = aux.get_cost_matrix();
  n_inputs = aux.n_inputs;
  n_outputs = aux.n_outputs;
  graph_matrix = aux.graph_matrix;
  build_from_matrixes();
}

bool concurrent_neural_network::calculate(std::vector<double> &input_values,
                                          std::vector<double> &output_values) {

  propagate_feedback();

  // Check compatibility of the vectors
  unsigned i_size = input_values.size();
  unsigned o_size = n_outputs;
  if (i_size != n_inputs)
    return false;

  // Set the inputs
  for (unsigned i = 0; i < i_size; i++)
    input_axons[i]->set_value(input_values[i]);


  std::vector<std::future<void>> promises (neurons.size());
  auto calculate_neuron = [&](unsigned i) {
    neurons[i]->calculate_value();
    neurons[i]->propagate_value();
  };

  // Calculate concurently
  unsigned last_neuron = 0;
  for (unsigned concurrent_group : concurrent_steps) {
    for (unsigned i = last_neuron; i <= concurrent_group; i++)
      promises[i] = std::async(calculate_neuron, i);

    for (unsigned i = last_neuron; i < concurrent_group; i++)
      promises[i].get();

    last_neuron = concurrent_group + 1;
  }

  // Retrieve the outputs
  output_values.resize(o_size);
  for (unsigned i = 0; i < o_size; i++)
    output_values[i] = output_axons[i]->get_value();

  return true;
}

void concurrent_neural_network::print() const {
  for (auto& line : cost_matrix) {
    for (auto& element : line)
      printf("%5d", element);
    std::cout << std::endl;
  }

  std::cout << "\n" << std::endl;
  std::cout << "Inputs" << std::endl;
  for (unsigned i = 0; i < n_inputs; i++)
    std::cout << i << " ";
  std::cout << "\n\n" << std::endl;
  std::cout << "outputs" << std::endl;
  for (unsigned i = n_inputs; i < n_inputs + n_outputs; i++)
    std::cout << i << " ";
}

void concurrent_neural_network::propagate_feedback() {
  for (auto& feedbacker : feedbackers)
    feedbacker.second->propagate_value();
}

void concurrent_neural_network::add_feedbacker(unsigned int origin_neuron,
                                               unsigned int destiny_neuron,
                                               double w){
  if (feedbackers.find(origin_neuron) == feedbackers.end())
    feedbackers[destiny_neuron] = new feedback_bus(neurons[destiny_neuron]);
  feedbackers[destiny_neuron]->add_connection(neurons[origin_neuron], w);
}


void concurrent_neural_network::delete_unreachable_nodes() {

  unsigned size = graph_matrix.size();

  unsigned j;

  std::stack<unsigned> sucesors;
  for (unsigned i = size - n_outputs - 1; i >= n_inputs; i--)
    sucesors.push(i);

  std::vector<bool> has_predecesor (size);

  while (sucesors.size() != 0) {
    // find next valid candidate in the stack
    do {
      if (sucesors.size() == 0)
        return;
      j = sucesors.top();
      sucesors.pop();
    } while (j < n_inputs || j > size - n_outputs - 1);

    bool empty_column = true;
    for (int i = j - 1; i >= 0; i--) {
      if (graph_matrix[i][j]) {
        empty_column = false;
        i = 0;
      }
    }

    // delete unreachable node and find all sucessors
    if (empty_column) {
      sucesors.push(j);
      for (unsigned k = 0; k < size; k++) {
        if (graph_matrix[j][k]) {
          sucesors.push(j > k ? j - 1 : j);
          graph_matrix[j][k] = 0;
        }
      }
      delete_row_col<bool> (graph_matrix, j);
      delete_row_col<TYPE> (cost_matrix, j);
      size--;
    }
  }
}


void concurrent_neural_network::delete_deathend_nodes() {
  unsigned size = graph_matrix.size();

  std::stack<unsigned> predecesors;
  for (unsigned i = size - n_outputs - 1; i >= n_inputs; i--)
    predecesors.push(i);

  unsigned i;
  while (predecesors.size() != 0) {
    // find next valid candidate in the stack
    do {
      if (predecesors.size() == 0)
        return;
      i = predecesors.top();
      predecesors.pop();
    } while (i < n_inputs || i > size - n_outputs - 1);

    bool empty_row = true;
    for (unsigned j = i + 1; j < size; j++) {
      if (graph_matrix[i][j]) {
        empty_row = false;
        j = size;
      }
    }
    // delete deathend node and find all predecesors
    if (empty_row) {
      predecesors.push(i);
      for (unsigned k = 0; k < size; k++) {
        if (graph_matrix[k][i]) {
          predecesors.push(k > i ? k - 1 : k);
          graph_matrix[k][i] = 0;
        }
      }
      delete_row_col<bool> (graph_matrix, i);
      delete_row_col<TYPE> (cost_matrix, i);
      size--;
    }
  }
}

std::vector< unsigned int > concurrent_neural_network::generate_visited_nodes() {
  unsigned size = graph_matrix.size();
  std::vector<unsigned> visited_nodes (size);

  for (unsigned i = 0; i < size; i++) {
    for (unsigned j = i + 1; j < size; j++) {
      if (graph_matrix[i][j])
        visited_nodes[j]++;
    }
  }
  return visited_nodes;
}

std::vector<unsigned int> concurrent_neural_network::generate_concurrent_steps() {
  unsigned size = graph_matrix.size();
  std::vector<unsigned> visited_nodes = generate_visited_nodes();

  std::vector<unsigned> solve;

  auto aux_visited = visited_nodes;

  unsigned last_node = 0;

  for (unsigned i = 0; i < size; i++) {
    for (unsigned j = i + 1; j < size; j++) {
      if (graph_matrix[i][j]) {
        if (visited_nodes[i] != 0) {
          visited_nodes = aux_visited;
          solve.push_back(last_node);
        }
        aux_visited[j]--;
        last_node = i;
      }
    }
  }
  solve.push_back(last_node);
  solve.push_back(size - 1);
  return solve;
}
