#include "dna.h"

dna::~dna() {
  if (sequence != nullptr)
    free (sequence);
}

void dna::operator=(const dna &aux) {  
  input_neurons = aux.input_neurons;
  output_neurons = aux.output_neurons;

  sequence = (char*)malloc(aux.byte_sz);
  memcpy(sequence, aux.sequence, aux.byte_sz);

  byte_sz = aux.byte_sz;
}
