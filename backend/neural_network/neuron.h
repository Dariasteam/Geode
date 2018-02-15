/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2018  Daniel Darias Sánchez <dariasteam94@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NEURON_H
#define NEURON_H

#include <vector>
#include <cmath>

#include "axon.h"

/**
 * @brief Perceptron like neuron
 *
 */
class neuron {
protected:
  double value;
  double threshold;

  unsigned n_inputs;

  std::vector<axon*> inputs;
  std::vector<axon*> outputs;
public:
  neuron () : n_inputs(0) {}

  void set_threshold (double t) { threshold = t - 0.2; }

  void add_input (axon* input);
  void add_output (axon* output);

  virtual void calculate_value ();
  void propagate_value ();

  double get_value () const { return value; }
};

/**
 * @brief Neuron that check if have any input axon before trying to calculate
 * its output. If have none it outputs 0.
 *
 */
class output_neuron : public neuron {
public:
  /**
   * @brief Check if have any input axon before trying to calculate
   * its output. If have none outputs 0.
   *
   */
  void calculate_value() override;
};

#endif // NEURON_H
