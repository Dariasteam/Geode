#ifndef DNA_H
#define DNA_H

#include <memory>

#include <stdlib.h>
#include <string.h>

/**
 * @brief Estructura que representa un genoma de red neuronal
 */
struct dna {
  /**
   * @brief Secuencia *binaria* que contiene toda la información para generar la
   * red neuronal. Codifica dos matrices de tamaño n x n donde n es el número de
   * neuronas y las matrices corresponden a:
   *
   * Matriz a: matriz booleana, indica si existe un determinado axón que ij que
   * conecta la neurona i con la neurona j. En el caso de i = j es indiferente el valor.
   *
   * Matriz b: matriz de costes, indica el peso del axón ij en caso de existir.
   * En el caso de i = j el valor representa el umbral.
   *
   * La codificación es la siguiente, por orden de aparición de los
   * elementos:
   *
   * n    : número de neuronas. 4 bytes
   * a00  : trivial, una neurona siempre conecta consigo misma
   * b00  : valor del umbral de la neurona 0
   * a01  : existencia o no de un axón que conecte la neurona 0 con la 1
   * b01  : peso del axón que conecta la neurona 0 con la 1
   * ...
   * ann  : trivial, una neurona siempre conecta consigo misma
   * bnn  : valor del umbral de la neurona n
   */
  char* sequence;

  /**
   * @brief Tamaño en bytes de la cadena contenida en #sequence
   */
  unsigned byte_sz;

  /**
   * @brief Número de neuronas de entrada, las neuronas desde 0 hasta #input_neurons
   * se consideran neuronas de entrada. Primeras #input_neurons neuronas.
   */
  unsigned input_neurons;

  /**
   * @brief Número de neuronas de salida, las neuronas desde (n - #output_neurons) hasta n
   * se consideran neuronas de entrada. Últimas #output_neurons neuronas.
   */
  unsigned output_neurons;

  explicit dna () :
    sequence (nullptr)
  {}

  dna (const dna& DNA) :
    input_neurons (DNA.input_neurons),
    output_neurons (DNA.output_neurons),
    byte_sz (DNA.byte_sz)
    {
      sequence = new char[DNA.byte_sz];
      memcpy(sequence, DNA.sequence, DNA.byte_sz);
    }

  dna (char* s, unsigned b, unsigned i, unsigned o) :
    byte_sz (b),
    input_neurons (i),
    output_neurons (o)
  {
    sequence = new char[b];
    memcpy(sequence, s, b);
  }

  ~dna ();
  void operator= (const dna& aux);
};

#endif // DNA_H
