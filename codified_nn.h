#ifndef CODIFIED_NN_H
#define CODIFIED_NN_H

#include "neuron.h"
#include "dna.h"

#include <vector>
#include <string.h>
#include <iostream>

class codified_nn {
private:
  unsigned sz;          // número de neuronas
  unsigned n_axons;

  unsigned input_neurons;
  unsigned output_neurons;

  std::vector <neuron> sucesors_list;

  void get_mem (void* mem, unsigned& index, const char* seq, size_t size);
  void copy_mem (char* seq, unsigned& index, const void* mem, size_t size);

public:
  explicit codified_nn (const std::vector<neuron>& aux_m, unsigned in,
                                                          unsigned on);

  explicit codified_nn (dna& DNA);
  void print ();

  // [número de bytes] [[umbral] [número de sucesores] (list de sucesores)] [...]
  dna to_dna ();

  const unsigned get_size                () const { return sz; }
  const unsigned get_n_axons             () const { return n_axons; }
  const std::vector<neuron>& get_content () const { return sucesors_list; }
  unsigned get_input_neurons             () const { return input_neurons; }
  unsigned get_output_neurons            () const { return output_neurons; }
};

#endif // CODIFIED_NN_H
