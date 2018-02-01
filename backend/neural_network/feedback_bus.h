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

#ifndef FEEDBACK_BUS_H
#define FEEDBACK_BUS_H

#include "neuron.h"
#include "axon.h"

/**
 * @todo write docs
 */
/**
 * @brief Similar to #neuron, it allow cycles (feedback) between neurons by capturing de feedback
 * data and propagate it after regular forward data propagation have ended.
 *
 */
class feedback_bus {
private:
  unsigned size;
  std::vector<axon*> inputs;
  std::vector<axon*> outputs;
  neuron* destiny;
public:
  feedback_bus (neuron* des) : size(0), destiny (des) {}
  void add_connection (neuron* origin, double weight);
  void propagate_value ();
};


#endif // FEEDBACK_BUS_H
