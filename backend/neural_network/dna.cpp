#include "dna.h"

#include <iostream>

dna::~dna() {    
  if (sequence != nullptr)
    delete[] sequence;  
}

void dna::operator=(const dna &aux) {
  input_neurons = aux.input_neurons;
  output_neurons = aux.output_neurons;
  byte_sz = aux.byte_sz;

  sequence = new char[aux.byte_sz];

  memcpy(sequence, aux.sequence, aux.byte_sz);
}
