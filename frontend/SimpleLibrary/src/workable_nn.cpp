#include "workable_nn.h"

/* Construye una red a partir de una matriz de costes
 */
workable_nn::workable_nn(std::vector<std::vector<std::pair<bool, TYPE> > > cost,
unsigned input, unsigned output) :
input_neurons (input),
output_neurons (output)
{
  unsigned size = cost.size();

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
}

/* Construye una red a partir de una adn
* */
workable_nn::workable_nn (const dna& DNA)  :
input_neurons (DNA.input_neurons),
output_neurons (DNA.output_neurons)
{
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
}

void workable_nn::get_mem (void* mem, unsigned& index, const char* seq, size_t size) {
  memcpy(mem, seq + index, size);
  index += size;
}

void workable_nn::copy_mem (char* seq, unsigned& index, const void* mem, size_t size) {
  memcpy(seq + index, mem, size);
  index += size;
}

dna workable_nn::to_dna() {
  unsigned n_bytes = (std::pow(cost_matrix.size(), 2)  * (sizeof(TYPE) + sizeof(bool))) +
  sizeof(unsigned);

  char* sequence = (char*)malloc(n_bytes);
  unsigned index = 0;

  unsigned c = cost_matrix.size();
  copy_mem(sequence, index, &c, sizeof(c));

  for (unsigned i = 0; i < cost_matrix.size(); i++) {
    for (unsigned j = 0; j < cost_matrix.size(); j++) {
      bool b = graph_matrix[i][j];
      copy_mem(sequence, index, &b, sizeof(bool));
      copy_mem(sequence, index, &cost_matrix[i][j], sizeof(TYPE));
    }
  }

  return {sequence, n_bytes, input_neurons, output_neurons};
}

workable_nn::workable_nn(const workable_nn &aux) :
cost_matrix (aux.cost_matrix),
input_neurons (aux.input_neurons),
output_neurons (aux.output_neurons),
graph_matrix (aux.graph_matrix)
{}

void workable_nn::operator=(const workable_nn &aux) {
  cost_matrix = aux.get_cost_matrix();
  input_neurons = aux.input_neurons;
  output_neurons = aux.output_neurons;
  graph_matrix = aux.graph_matrix;
}

void workable_nn::calculate(std::vector<double> &inputs, std::vector<double> &outputs) {
  unsigned size = cost_matrix.size();

  // Matriz auxiliar para comprobar cuándo puede calcularse la siguiente neurona
  std::vector<std::vector<bool>> used_elements (size);
  for (auto& row : used_elements)
    row.resize (size);

  // Matriz auxiliar para resultados intermedios
  std::vector<std::vector<double>> results (size);
  for (auto& row : results)
    row.resize (size);

  // Inicializar las neuronas de entrada
  for (unsigned i = 0; i < input_neurons; i++) {
    results[i][i] = inputs[i];
    used_elements[i][i] = true;

    double value = inputs[i];

    // Calcular los axones derivados de las neuronas de entrada
    iterate_avoiding_index([&](unsigned index) -> void {
      if (graph_matrix[i][index]) {
        results[i][index] = value * saturate(cost_matrix[i][index]);
        used_elements[i][index] = true;
      }
    }, 0, i, size);
  }

  // Propagar valores por las siguientes neuronas disponibles
  while (true) {
    int selected_neuron = -1;
    // Seleccionar la siguiente neurona cuyos predecesores estén calculados
    double value (0);
    unsigned ac (0);

    for (unsigned i = input_neurons; i < size; i++) {
      value = 0;
      ac = 0;
      if (!used_elements[i][i]) {
        selected_neuron = i;
        for (unsigned j = 0; j < i; j++) {
          if (graph_matrix[j][i]) {
            if (used_elements[j][i]) {
              // El axón de entrada está disponible
              value += results[j][i];
              ac++;
            } else if (cost_matrix[j][i] != 0) {
              // El axón de entrada aún no ha sido calculado
              value = 0;
              selected_neuron = -1;
              break;
            }
          }
        }
        // Tenemos una neurona con todos sus predecesores calculados
        if (selected_neuron != -1)
          break;
      }
    }

    // No hay más neuronas calculables
    if (selected_neuron == -1)
      break;

    double threshold = saturate(cost_matrix[selected_neuron][selected_neuron]);
    value = (ac != 0) ? std::tanh(value + threshold / ac) : 0;

    used_elements[selected_neuron][selected_neuron] = true;
    results[selected_neuron][selected_neuron] = value;

    // Calcular los axones derivados de la neurona calculada
    iterate_avoiding_index([&](unsigned index) -> void {
      if (graph_matrix[selected_neuron][index]) {
        double aux = value * saturate(cost_matrix[selected_neuron][index]);
        results[selected_neuron][index] = aux;
        used_elements[selected_neuron][index] = true;
      }
    }, 0, selected_neuron, size);
  }

  // Recoger los valores de las neuronas de salida
  outputs.resize(0);

  for (unsigned i = cost_matrix.size() - output_neurons; i < cost_matrix.size(); i++)
    outputs.push_back(std::tanh(results[i][i]));
}

void workable_nn::print() const {
  for (auto& line : cost_matrix) {
    for (auto& element : line)
      printf("%5d", element);
    std::cout << std::endl;
  }

  std::cout << "\n" << std::endl;
  std::cout << "Inputs" << std::endl;
  for (unsigned i = 0; i < input_neurons; i++)
    std::cout << i << " ";
  std::cout << "\n\n" << std::endl;
  std::cout << "outputs" << std::endl;
  for (unsigned i = input_neurons; i < input_neurons + output_neurons; i++)
    std::cout << i << " ";
}
