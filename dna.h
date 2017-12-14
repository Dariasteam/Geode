#ifndef DNA_H
#define DNA_H

#include <stdlib.h>
#include <string.h>

struct dna {
  char* sequence;
  unsigned sz;
  unsigned input_neurons;
  unsigned output_neurons;

  ~dna () {
    free (sequence);
  }
  void operator= (const dna& aux) {
    unsigned sz;
    input_neurons = aux.input_neurons;
    output_neurons = aux.output_neurons;
    memcpy(&sz, aux.sequence, sizeof(sz));
    sequence = (char*)malloc(sz + sizeof(sz));
    memcpy(sequence, aux.sequence, sz + sizeof(sz));
  }
};

#endif // DNA_H
