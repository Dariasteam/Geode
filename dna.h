#ifndef DNA_H
#define DNA_H

#include <stdlib.h>
#include <string.h>

struct dna {
  char* sequence;

  // tamaño en bytes
  unsigned byte_sz;
  unsigned input_neurons;
  unsigned output_neurons;

  dna () :
    sequence (nullptr)
  {}

  dna (const dna& DNA) {
    operator =(DNA);
  }

  dna (char* s, unsigned b, unsigned i, unsigned o) :
    sequence (s),
    byte_sz (b),
    input_neurons (i),
    output_neurons (o)
  {}

  ~dna ();
  void operator= (const dna& aux);
};

#endif // DNA_H
