#ifndef NEURON_H
#define NEURON_H

#define TYPE short

#include <vector>

struct neuron {
  unsigned umbral;
  std::vector<std::pair<unsigned, TYPE>> list;
};

#endif // NEURON_H
