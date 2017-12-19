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

  ~dna ();
  void operator= (const dna& aux);

  //dna () {}
/*
  dna (char* seq, unsigned s, unsigned i, unsigned o) :
    sequence (seq),
    byte_sz (s),
    input_neurons (i),
    output_neurons (o)
  {}
  */
};

#endif // DNA_H
