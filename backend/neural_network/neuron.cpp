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

#include "neuron.h"

// REGULAR NEURON

void neuron::add_input(axon* input){
  inputs.push_back(input);
  n_inputs++;
}

void neuron::add_output(axon* output){
  outputs.push_back(output);
}

void neuron::calculate_value(){
  for (axon* aux : inputs)
    value += aux->get_value();
}

void neuron::propagate_value() {
  value = std::tanh((value / n_inputs) + threshold);
  for (axon* aux : outputs)
    aux->set_value(value);
  value = 0;
}

// OUTPUT NEURON

void output_neuron::propagate_value(){
   value = (n_inputs == 0) ? 0 : std::tanh((value / n_inputs) + threshold);
   for (axon* aux : outputs)
     aux->set_value(value);
   value = 0;
 }
