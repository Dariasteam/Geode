#ifndef WORKABLE_NN_H
#define WORKABLE_NN_H

#include <math.h>

#include "neuron.h"
#include "dna.h"
#include "codified_nn.h"

/* Clase que representa una red neuronal en formato lista de sucesores para
 * su codificación como genoma en binario.
 *
 * Las primeras input_neurons + output_neurons neuronas son fijas y representan
 * respectivamente las neuronas de entrada y de salida en ese orden.
 *
 * La suma de estos dos numeros es por tanto el tamaño mínimo de la red
 * */

class workable_nn {
private:
  std::vector<std::vector<TYPE>> cost_matrix;  // contiene los valores de lso elementos
  std::vector<std::vector<bool>> graph_matrix; // determina qué elementos pertenecen al grafo
  unsigned input_neurons;
  unsigned output_neurons;
public:
  workable_nn () {}

  workable_nn (const workable_nn& aux) :
    cost_matrix (aux.cost_matrix),
    input_neurons (aux.input_neurons),
    output_neurons (aux.output_neurons),
    graph_matrix (aux.graph_matrix)
  {}

  workable_nn (const codified_nn& nn) :
    input_neurons (nn.get_input_neurons()),
    output_neurons (nn.get_output_neurons())
    {

    std::vector<neuron> aux_nn = nn.get_content();
    unsigned size = aux_nn.size();

    cost_matrix.resize (size);
    graph_matrix.resize (size);
    for (unsigned i = 0; i < size; i++) {

      cost_matrix[i].resize (size);
      graph_matrix[i].resize (size);

      graph_matrix[i][i] = true;
      cost_matrix[i][i] = aux_nn[i].umbral;

      for (auto& element : aux_nn[i].list) {
        unsigned connected_n = element.first;
        if (connected_n >= size)
          break;
        cost_matrix[i][connected_n] = element.second;
        graph_matrix[i][connected_n] = true;
      }
    }
  }

  void operator= (const workable_nn& aux) {
    cost_matrix = aux.get_cost_matrix();
    input_neurons = aux.input_neurons;
    output_neurons = aux.output_neurons;
    graph_matrix = aux.graph_matrix;
  }

  std::vector<std::vector<TYPE>> get_cost_matrix () const { return cost_matrix; }

  void calculate (std::vector<double>& inputs, std::vector<double>& outputs) {
    unsigned size = cost_matrix.size();

    // Matrix auxliar para comprobar cuándo puede calcularse la siguiente neurona
    std::vector<std::vector<bool>> used_elements (size);
    for (auto& row : used_elements)
      row.resize (size);

    // Matriz auxiliar para resultados intermedios
    std::vector<std::vector<double>> results (size);
    for (auto& row : results)
      row.resize (size);

    // Inicializar las neuronas de entrada
    for (unsigned i = 0; i < input_neurons; i++) {
      results[i][i] = double(inputs[i]);
      used_elements[i][i] = true;

      unsigned value = inputs[i];
      // Calcular los axones derivados de las neuronas de entrada
      for (unsigned j = 0; j < i; j++) {
        if (graph_matrix[i][j]) {
          results[i][j] = value * double(cost_matrix[i][j]) / 100;
          used_elements[i][j] = true;
        }
      }
      for (unsigned j = i + 1; j < size; j++) {
        if (graph_matrix[i][j]) {
          results[i][j] = value * double(cost_matrix[i][j]) / 100;
          used_elements[i][j] = true;
        }
      }
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
                selected_neuron = -1;
                break;
              }
            }
          }
          for (unsigned j = i + 1; j < size; j++) {
            if (graph_matrix[j][i]) {
              if (used_elements[j][i]) {
                // El axón de entrada está disponible
                value += results[j][i];
                ac++;
              } else if (cost_matrix[j][i] != 0) {
                // El axón de entrada aún no ha sido calculado
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

//   std::cout << "La neurona seleccionada es " << selected_neuron << std::endl;
//      std::cout << "Su valor es " << value << std::endl;

      value = (value != 0) ? std::tanh(value / ac) : 0;

//      std::cout << "Su valor normalizado es " << value << std::endl;

      used_elements[selected_neuron][selected_neuron] = true;
      results[selected_neuron][selected_neuron] = value;

      // Calcular los axones derivados de la neurona calculada
      for (unsigned j = 0; j < selected_neuron; j++) {
        if (graph_matrix[selected_neuron][j]) {
          double aux = value * double(cost_matrix[selected_neuron][j]) / 100;
          results[selected_neuron][j] = aux;
          used_elements[selected_neuron][j] = true;
        }
      }
      for (unsigned j = selected_neuron + 1; j < size; j++) {
        if (graph_matrix[selected_neuron][j]) {
          double aux = value * double(cost_matrix[selected_neuron][j]) / 100;
          results[selected_neuron][j] = aux;
          used_elements[selected_neuron][j] = true;
        }
      }

    }

    // Recoger los valores de las neuronas de salida
    outputs.resize(0);

    for (unsigned i = input_neurons; i < output_neurons + input_neurons; i++)
      outputs.push_back(std::tanh(results[i][i]));
  }

  void print () const {
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
};

#endif // WORKABLE_NN_H
