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

  void get_mem (void* mem, unsigned& index, const char* seq, size_t size) {
    memcpy(mem, seq + index, size);
    index += size;
  }

  void copy_mem (char* seq, unsigned& index, const void* mem, size_t size) {
    memcpy(seq + index, mem, size);
    index += size;
  }

public:
  explicit codified_nn (const std::vector<neuron>& aux_m, unsigned in,
                                                          unsigned on) :
    sucesors_list (aux_m),
    input_neurons (in),
    output_neurons (on),
    n_axons(0)
    {
      sz = aux_m.size();
      if (sz >= input_neurons + output_neurons) {
        for (auto& aux_neuron : sucesors_list)
          n_axons += aux_neuron.list.size();
      } else {
        unsigned n_neurons = input_neurons + output_neurons - sz;
        for (unsigned i = 0; i < n_neurons; i++)
          sucesors_list.push_back(neuron());
        sz += n_neurons;
      }
    }

  explicit codified_nn (dna& DNA) :
    input_neurons  (DNA.input_neurons),
    output_neurons (DNA.output_neurons),
    n_axons (0)
    {
    sucesors_list.clear();

    unsigned index = 0;
    unsigned aux_sz;
    unsigned dna_sz;

    const char* sequence = DNA.sequence;

    get_mem(&dna_sz, index, sequence, sizeof(dna_sz));

    while (dna_sz - index > sizeof(unsigned) * 2) {
      neuron aux_neuron;
      get_mem(&aux_neuron.umbral, index, sequence, sizeof(aux_neuron.umbral));
      get_mem(&aux_sz, index, sequence, sizeof(aux_sz));

      for (unsigned j = 0; j < aux_sz; j++) {
        if (dna_sz - index <= sizeof(unsigned) * 2)
          break;
        n_axons++;
        std::pair<unsigned, TYPE> aux_p;
        get_mem(&aux_p.first, index, sequence, sizeof(aux_p.first));
        get_mem(&aux_p.second, index, sequence, sizeof(aux_p.second));

        aux_p.first = aux_p.first;
        aux_p.second =aux_p.second;

        aux_neuron.list.push_back(aux_p);
      }
      sucesors_list.push_back(aux_neuron);
    }

    // Añadir listas de sucesores vacías para completar el tamaño
    int minimum_size = DNA.input_neurons + DNA.output_neurons;
    while (minimum_size - int(sucesors_list.size()) >= 0)
      sucesors_list.push_back(neuron());

    sz = sucesors_list.size();
  }

  void print () {
    printf("Umbral N_Axones [Sucesor Coste] [Sucesor Coste] [Sucesor Coste]\n");
    for (neuron& aux_neuron : sucesors_list) {
      printf("%6u", aux_neuron.umbral);
      printf("%9zu", aux_neuron.list.size());
      for (auto& pr : aux_neuron.list) {
        printf("%9u", pr.first);
        printf("%6d", pr.second);
      }
      std::cout << "\n";
    }
  }

  // [número de bytes] [[umbral] [número de sucesores] (list de sucesores)] [...]
  dna to_dna () {

    unsigned byte_size = (sz * sizeof(unsigned) * 2) +
                        (n_axons * (sizeof(unsigned) +
                                    sizeof(TYPE)
                        ) + sizeof(unsigned));


    char* sequence = (char*)malloc(byte_size);
    unsigned index = 0;
    TYPE aux_t;
    unsigned aux_u;
    unsigned aux_sz;

    copy_mem(sequence, index, &byte_size, sizeof(byte_size));

    for (unsigned i = 0; i < sz; i++) {
      aux_u = sucesors_list[i].umbral;
      copy_mem(sequence, index, &aux_u, sizeof(aux_u));
      aux_sz = sucesors_list[i].list.size();
      copy_mem(sequence, index, &aux_sz, sizeof(aux_sz));
      for (unsigned j = 0; j < aux_sz; j++) {
        aux_u = sucesors_list[i].list[j].first;
        copy_mem(sequence, index, &aux_u, sizeof(aux_u));
        aux_t = sucesors_list[i].list[j].second;
        copy_mem(sequence, index, &aux_t, sizeof(aux_t));
      }
    }
    return {sequence, byte_size, input_neurons, output_neurons};
  }

  const unsigned get_size                () const { return sz; }
  const unsigned get_n_axons             () const { return n_axons; }
  const std::vector<neuron>& get_content () const { return sucesors_list; }
  unsigned get_input_neurons             () const { return input_neurons; }
  unsigned get_output_neurons            () const { return output_neurons; }
};

#endif // CODIFIED_NN_H
