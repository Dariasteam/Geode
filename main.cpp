#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <string.h>
#include <random>

#define TYPE int
#define N 14
#define MIN -100
#define MAX  100
#define MAX_DNA_SIZE 1000
#define E 2.71828182845

struct neuron {
  unsigned umbral;
  std::vector<std::pair<unsigned, TYPE>> list;
};

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

void get_mem (void* mem, unsigned& index, const char* seq, size_t size) {
  memcpy(mem, seq + index, size);
  index += size;
}

void copy_mem (char* seq, unsigned& index, const void* mem, size_t size) {
  memcpy(seq + index, mem, size);
  index += size;
}

/* Clase que representa una red neuronal en formato lista de sucesores para
 * su codificación como genoma en binario.
 *
 * Las primeras input_neurons + output_neurons neuronas son fijas y representan
 * respectivamente las neuronas de entrada y de salida en ese orden.
 *
 * La suma de estos dos numeros es por tanto el tamaño mínimo de la red
 * */
class codified_nn {
private:
  unsigned sz;
  unsigned n_axons;

  unsigned input_neurons;
  unsigned output_neurons;

  std::vector <neuron> sucesors_list;
public:
  explicit codified_nn (const std::vector<neuron>& aux_m, unsigned in,
                                                          unsigned on) :
    sucesors_list (aux_m),
    input_neurons (in),
    output_neurons (on),
    n_axons(0)
    {
      sz = aux_m.size();
      if (sz >= input_neurons + output_neurons) {
        for (auto& aux_neuron : sucesors_list)
          n_axons += aux_neuron.list.size();
      } else {
        unsigned n_neurons = input_neurons + output_neurons - sz;
        for (unsigned i = 0; i < n_neurons; i++)
          sucesors_list.push_back(neuron());
        sz += n_neurons;
      }
    }

  explicit codified_nn (dna& DNA) :
    input_neurons  (DNA.input_neurons),
    output_neurons (DNA.output_neurons),
    n_axons (0)
    {
    sucesors_list.clear();       

    unsigned index = 0;
    unsigned aux_sz;
    unsigned dna_sz;

    const char* sequence = DNA.sequence;

    get_mem(&dna_sz, index, sequence, sizeof(dna_sz));

    while (dna_sz - index > sizeof(unsigned) * 2) {
      neuron aux_neuron;
      get_mem(&aux_neuron.umbral, index, sequence, sizeof(aux_neuron.umbral));
      get_mem(&aux_sz, index, sequence, sizeof(aux_sz));

      for (unsigned j = 0; j < aux_sz; j++) {
        if (dna_sz - index <= sizeof(unsigned) * 2)
          break;
        n_axons++;
        std::pair<unsigned, TYPE> aux_p;
        get_mem(&aux_p.first, index, sequence, sizeof(aux_p.first));
        get_mem(&aux_p.second, index, sequence, sizeof(aux_p.second));

        aux_p.first = std::min (aux_p.first, unsigned(100));
        aux_p.second = std::min(aux_p.second, 100);

        aux_neuron.list.push_back(aux_p);
      }
      sucesors_list.push_back(aux_neuron);
    }

    // Añadir listas de sucesores vacías para completar el tamaño
    int minimum_size = DNA.input_neurons + DNA.output_neurons;
    while (minimum_size - int(sucesors_list.size()) >= 0)
      sucesors_list.push_back(neuron());
  }

  void print () {
    printf("Umbral N_Axones [Sucesor Coste] [Sucesor Coste] [Sucesor Coste]\n");
    for (neuron& aux_neuron : sucesors_list) {
      printf("%6u", aux_neuron.umbral);
      printf("%9zu", aux_neuron.list.size());
      for (auto& pr : aux_neuron.list) {
        printf("%9u", pr.first);
        printf("%6d", pr.second);
      }
      std::cout << "\n";
    }
  }

  // [número de bytes] [[umbral] [número de sucesores] (list de sucesores)] [...]
  dna to_dna () {

    unsigned byte_size = (sz * sizeof(unsigned) * 2) +
                        (n_axons * (sizeof(unsigned) +
                                    sizeof(TYPE)
                        ) + sizeof(unsigned));


    char* sequence = (char*)malloc(byte_size);
    unsigned index = 0;
    TYPE aux_t;
    unsigned aux_u;
    unsigned aux_sz;

    copy_mem(sequence, index, &byte_size, sizeof(byte_size));

    for (unsigned i = 0; i < sz; i++) {
      aux_u = sucesors_list[i].umbral;
      copy_mem(sequence, index, &aux_u, sizeof(aux_u));
      aux_sz = sucesors_list[i].list.size();
      copy_mem(sequence, index, &aux_sz, sizeof(aux_sz));
      for (unsigned j = 0; j < aux_sz; j++) {
        aux_u = sucesors_list[i].list[j].first;
        copy_mem(sequence, index, &aux_u, sizeof(aux_u));
        aux_t = sucesors_list[i].list[j].second;
        copy_mem(sequence, index, &aux_t, sizeof(aux_t));
      }
    }
    return {sequence, byte_size, input_neurons, output_neurons};
  }

  const unsigned get_size                () const { return sz; }
  const unsigned get_n_axons             () const { return n_axons; }
  const std::vector<neuron>& get_content () const { return sucesors_list; }
  unsigned get_input_neurons             () const { return input_neurons; }
  unsigned get_output_neurons            () const { return output_neurons; }
};

class workable_nn {
private:
  std::vector<std::vector<TYPE>> cost_matrix;
  unsigned input_neurons;
  unsigned output_neurons;
public:
  workable_nn () {}

  workable_nn (const workable_nn& aux) :
    cost_matrix (aux.cost_matrix),
    input_neurons (aux.input_neurons),
    output_neurons (aux.output_neurons)
  {}

  workable_nn (const codified_nn& nn) :
    input_neurons (nn.get_input_neurons()),
    output_neurons (nn.get_output_neurons())
    {

    std::vector<neuron> aux_nn = nn.get_content();
    unsigned size = aux_nn.size();

    cost_matrix.resize (size);
    for (unsigned i = 0; i < size; i++) {
      cost_matrix[i].resize (size);
      cost_matrix[i][i] = std::min(aux_nn[i].umbral, unsigned(100));

      for (auto& element : aux_nn[i].list) {
        unsigned connected_n = element.first;
        if (connected_n >= size)
          break;
        cost_matrix[i][connected_n] = element.second;       
      }
    }    
  }

  /*
  workable_nn (const std::vector<std::vector<TYPE>>& aux_c) :
    cost_matrix (aux_c)
  {}
  */

  void operator= (const workable_nn& aux) {
    cost_matrix = aux.get_cost_matrix();
    input_neurons = aux.input_neurons;
    output_neurons = aux.output_neurons;
  }

  std::vector<std::vector<TYPE>> get_cost_matrix () const { return cost_matrix; }

  void calculate (std::vector<TYPE>& inputs, std::vector<TYPE>& outputs) {
    unsigned size = cost_matrix.size();

    // Matrix auxliar para comprobar cuándo puede calcularse la siguiente neurona
    std::vector<std::vector<bool>> used_elements (size);
    for (auto& row : used_elements)
      row.resize (size);

    // Matriz auxiliar para resultados intermedios
    std::vector<std::vector<TYPE>> results (size);
    for (auto& row : results)
      row.resize (size);

    // Inicializar las neuronas de entrada
    for (unsigned i = 0; i < input_neurons; i++) {
      results[i][i] = inputs[i];
      used_elements[i][i] = true;

      unsigned value = inputs[i];
      // Calcular los axones derivados de las neuronas de entrada
      for (unsigned j = 0; j < i; j++) {
        results[i][j] = cost_matrix[i][j] * value;
        used_elements[i][j] = true;
      }
      for (unsigned j = i; j < size; j++) {
        results[i][j] = cost_matrix[i][j] * value;
        used_elements[i][j] = true;
      }
    }


    // Propagar valores por las siguientes neuronas disponibles
    unsigned i = input_neurons;
    while (true) {
      int selected_neuron = -1;
      // Seleccionar la siguiente neurona cuyos predecesores estén calculados
      double value (0);
      for (unsigned j = i; j < size; j++) {
        value = 0;
        if (!used_elements[j][j]) {
          selected_neuron = j;
          for (unsigned k = 0; k < size; k++) {
            if (results[k][j] != 0 && !used_elements[k][j] && k!=j) {
              selected_neuron = -1;
              break;
            } else if (k!=j){
              value += results[k][j];
            }
          }
          if (selected_neuron != -1)
            break;
        }
      }

      if (selected_neuron == -1)
        break;

      used_elements[selected_neuron][selected_neuron] = true;
/*
      std::cout << "La neurona seleccionada es " << selected_neuron << std::endl;
      std::cout << "Su valor es " << value << std::endl;
*/
      value = 1/ (1 + std::pow (E,-value)) - 0.1;

//      std::cout << "Su valor normalizado es " << value << std::endl;

      // Calcular los axones derivados de la neurona calculada
      for (unsigned j = 0; j < selected_neuron; j++) {
        results[selected_neuron][j] = cost_matrix[selected_neuron][j] * std::round(value);
        used_elements[selected_neuron][j] = true;
      }
      for (unsigned j = selected_neuron; j < size; j++) {
        results[selected_neuron][j] = cost_matrix[selected_neuron][j] * std::round(value);
        used_elements[selected_neuron][j] = true;
      }
    }

    // Recoger los valores de las neuronas de salida
    outputs.resize(0);

    for (unsigned i = input_neurons; i < output_neurons + input_neurons; i++) {
      outputs.push_back(1/ (1 + std::pow (E,-results[i][i])));
    }
  }

  void print () const {
    for (auto& line : cost_matrix) {
      for (auto& element : line)
        printf("%3d", element);
      std::cout << std::endl;
    }

    if (input_neurons > 200 || output_neurons > 200)
      int a = 2;

    std::cout << "\n" << std::endl;
    std::cout << "Inputs" << std::endl;
    for (unsigned i = 0; i < input_neurons; i++)
      std::cout << i << " ";
    std::cout << "\n\n" << std::endl;
    std::cout << "outputs" << std::endl;
    for (unsigned i = input_neurons; i < input_neurons + output_neurons; i++)
      std::cout << i << " ";
  }
};

void random_values_generator (std::vector<neuron>& suc_list) {
  suc_list.resize (N);
  for (unsigned i = 0; i < N; i++) {
    unsigned n_connections = std::rand() % N ;
    suc_list[i].umbral = 0;
    for (unsigned j = 0; j < n_connections; j++) {
      TYPE cost = 100 - std::rand() % 20;
      unsigned next = std::rand() % N;
      suc_list[i].list.push_back(std::make_pair(next, cost));
    }
  }
}

dna cross_workable_nn (workable_nn& A, workable_nn& B) {
  std::vector<std::vector<TYPE>> A_cost = A.get_cost_matrix();
  std::vector<std::vector<TYPE>> B_cost = B.get_cost_matrix();

  unsigned S_size = (A_cost.size() / 2) + (B_cost.size() / 2);

  std::vector<std::vector<TYPE>> s_cost (S_size);
}

dna cross_dna (dna& A, dna& B) {
  unsigned a_size;
  unsigned b_size;

  memcpy(&a_size, A.sequence, sizeof(a_size));
  memcpy(&b_size, B.sequence, sizeof(b_size));

  unsigned final_size = (a_size / 2) + (b_size / 2);

  dna S;
  S.sequence = (char*)malloc(final_size);
  memcpy(S.sequence, A.sequence, (a_size / 2));
  memcpy(S.sequence + (a_size / 2), B.sequence + (b_size / 2), (b_size / 2));

  memcpy(S.sequence, &final_size, sizeof(final_size));

  S.input_neurons = A.input_neurons;
  S.output_neurons = A.output_neurons;

  return S;
}

void add_mutations (dna& DNA) {
  unsigned old_sz;

  memcpy(&old_sz, DNA.sequence, sizeof(old_sz));
  unsigned new_sz = old_sz;

  if (rand() % 15 < 1) {
    new_sz += (rand() % 10 - 5);

    // ajusta el mínimo de neuronas a al menos las de entrada y salida
    if (new_sz < (DNA.input_neurons + DNA.output_neurons))
      new_sz += (DNA.input_neurons + DNA.output_neurons) - new_sz + 1;

    char* sequence = (char*)malloc(new_sz);
    memcpy(sequence, DNA.sequence, std::min(new_sz, old_sz));

    free (DNA.sequence);
    DNA.sequence = sequence;

    memcpy(DNA.sequence, &new_sz, sizeof(new_sz));
  }

  unsigned first_index;
  unsigned second_index;

  if (new_sz > old_sz)
    first_index = old_sz;
  else
    first_index = new_sz;

  second_index = new_sz;
  // TODO Aparentemente los mejores resultados los da el método a nivel de bit

/*
  // Genera mutaciones para el genoma ya existente
  // Método numérico
  for (unsigned i = sizeof(new_sz); i < first_index; i+=sizeof(old_sz)) {
    int a = std::rand() % 14 - 7;
    int b;
    memcpy(&b, DNA.sequence + i, sizeof(old_sz));
    b += a;
    if (b < 0)
      b = 0;
    else if (b > 100)
      b = 100;
    memcpy(DNA.sequence + i, &b, sizeof(old_sz));
  }
*/
  // Método a nivel de bit
  for (unsigned i = sizeof(new_sz); i < first_index; i+=sizeof(old_sz)) {
    for (unsigned j = 0; j < 3; j++) {
      if (rand() % 50 < 1)
        DNA.sequence[i] ^= 1 << j;
    }
  }

  // Genera la nueva información
  // Método numético
/*
  for (unsigned i = first_index; i < second_index; i+=sizeof(old_sz)) {
    int a = std::rand() % 14 - 7;
    int b;
    memcpy(&b, DNA.sequence + i, sizeof(old_sz));
    b += a;
    if (b < 0)
      b = 0;
    else if (b > 100)
      b = 100;
    memcpy(DNA.sequence + i, &b, sizeof(old_sz));
  }
*/
  // Método a nivel de bit

  for (unsigned i = first_index; i < second_index; i+=sizeof(old_sz)) {
    for (unsigned j = 0; j < 3; j++) {
      DNA.sequence[i] ^= (rand() % 5 < 1) << j;
    }
  }
}

unsigned evaluate (workable_nn& w, std::vector<TYPE> input) {
  std::vector<std::vector<TYPE>> matrix = w.get_cost_matrix();
  unsigned ac = 0;

  std::vector<TYPE> output;

  w.calculate(input, output);

  unsigned v = 0;

  unsigned i = 1;
  for (auto& e : output) {
    v += e * i;
    i++;
  }

  return v;
}

int main(int argc, char **argv) {

  srand(time(nullptr));

  std::vector<neuron> v1;
  std::vector<neuron> v2;

  random_values_generator(v1);
  random_values_generator(v2);

  codified_nn parent_1 (v1, 4, 8);
  codified_nn parent_2 (v2, 4, 8);

  dna serialized_nn_1 = parent_1.to_dna();
  dna serialized_nn_2 = parent_2.to_dna();

  std::vector<dna> sons_dna (12);
  std::vector<workable_nn> sons_wrk (12);

  char user_input = 1;
  while (user_input != 'q') {
    sons_dna[0] = cross_dna(serialized_nn_1, serialized_nn_2);
    sons_wrk[0] = workable_nn (codified_nn(sons_dna[0]));

    sons_dna[10] = serialized_nn_1;
    sons_dna[11] = serialized_nn_2;

    sons_wrk[10] = workable_nn (codified_nn(sons_dna[10]));
    sons_wrk[11] = workable_nn (codified_nn(sons_dna[11]));

    // Generar 10 clones y aplicar mutaciones
    for (unsigned i = 1; i < 10; i++) {
      sons_dna[i] = sons_dna[0];
      add_mutations(sons_dna[i]);
      sons_wrk[i] = workable_nn (codified_nn(sons_dna[i]));
    }

    int best_index = -1;
    int best_value = 1000;

    unsigned i = 0;
    for (auto& nn : sons_wrk) {
        int eval_value = evaluate(nn, {1, 1, 1, 1});
      if (abs(eval_value - 37) < best_value) {
        std::cout << "Distancia del candidato " << abs(eval_value - 37) << std::endl;
        best_value = abs(eval_value - 37);
        best_index = i;
      }
      i++;
    }

    std::cout << "\n\n\nFormato matricial\n\n\n" << std::flush;   
    std::cout << "Índice " << best_index << std::endl;
    sons_wrk[best_index].print();
    std::cout << "Con un valor de " << evaluate(sons_wrk[best_index], {1,1,1,1}) << std::endl;


    std::cout << evaluate(sons_wrk[best_index], {0, 0, 0, 0}) << std::endl;
    std::cout << evaluate(sons_wrk[best_index], {0, 0, 0, 1}) << std::endl;
    std::cout << evaluate(sons_wrk[best_index], {0, 0, 1, 0}) << std::endl;
    std::cout << evaluate(sons_wrk[best_index], {0, 0, 1, 1}) << std::endl;
    std::cout << evaluate(sons_wrk[best_index], {0, 1, 0, 0}) << std::endl;
    std::cout << evaluate(sons_wrk[best_index], {0, 1, 0, 1}) << std::endl;
    std::cout << evaluate(sons_wrk[best_index], {0, 1, 1, 0}) << std::endl;
    std::cout << evaluate(sons_wrk[best_index], {0, 1, 1, 1}) << std::endl;

    std::cout << evaluate(sons_wrk[best_index], {1, 0, 0, 0}) << std::endl;
    std::cout << evaluate(sons_wrk[best_index], {1, 0, 0, 1}) << std::endl;
    std::cout << evaluate(sons_wrk[best_index], {1, 0, 1, 0}) << std::endl;
    std::cout << evaluate(sons_wrk[best_index], {1, 0, 1, 1}) << std::endl;
    std::cout << evaluate(sons_wrk[best_index], {1, 1, 0, 0}) << std::endl;
    std::cout << evaluate(sons_wrk[best_index], {1, 1, 0, 1}) << std::endl;
    std::cout << evaluate(sons_wrk[best_index], {1, 1, 1, 0}) << std::endl;
    std::cout << evaluate(sons_wrk[best_index], {1, 1, 1, 1}) << std::endl;


    scanf("%c", &user_input);

    free(serialized_nn_2.sequence);
    free(serialized_nn_1.sequence);

    serialized_nn_1 = sons_dna[std::rand() % 12];
    serialized_nn_2 = sons_dna[best_index];
  }
  return 0;
}
