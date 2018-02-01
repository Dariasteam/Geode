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

#include "feedback_bus.h"

void feedback_bus::add_connection(neuron* origin, double weight) {
  size++;

  axon* aux1 = new axon(weight);
  axon* aux2 = new axon(1);

  inputs.push_back(aux1);
  origin->add_output(aux1);

  outputs.push_back(aux2);
  destiny->add_input(aux2);
}

void feedback_bus::propagate_value(){
  for (unsigned i = 0; i < size; i++)
    outputs[i]->set_value (inputs[i]->get_value());
}
