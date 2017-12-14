#include "dna.h"

dna::~dna() {
  free (sequence);
}

void dna::operator=(const dna &aux) {
  unsigned sz;
  input_neurons = aux.input_neurons;
  output_neurons = aux.output_neurons;
  memcpy(&sz, aux.sequence, sizeof(sz));
  sequence = (char*)malloc(sz + sizeof(sz));
  memcpy(sequence, aux.sequence, sz + sizeof(sz));
}
