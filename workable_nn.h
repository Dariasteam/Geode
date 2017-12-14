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

  workable_nn (const workable_nn& aux);
  workable_nn (const codified_nn& nn);

  void operator= (const workable_nn& aux);

  std::vector<std::vector<TYPE>> get_cost_matrix () const { return cost_matrix; }

  void calculate (std::vector<double>& inputs, std::vector<double>& outputs);

  void print () const;
};

#endif // WORKABLE_NN_H
