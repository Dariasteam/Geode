#ifndef DNA_H
#define DNA_H

#include <stdlib.h>
#include <string.h>

struct dna {
  char* sequence;
  unsigned sz;
  unsigned input_neurons;
  unsigned output_neurons;

  ~dna ();
  void operator= (const dna& aux);
};

#endif // DNA_H
