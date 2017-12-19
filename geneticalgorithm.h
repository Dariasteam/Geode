#ifndef GENETICALGORITHM_H
#define GENETICALGORITHM_H

#include <functional>
#include <vector>
#include <future>
#include <algorithm>
#include <cmath>

template <class T>
class GeneticAlgorithm {
private:
  std::vector <T> poblation;        // población generada en cada iteración
  std::vector <T> best_candidates;  // mejores candidatos de una generación

  std::function<T(T&, T&)> op_cross;
  std::function<void(T&)> op_mutate;
  std::function<bool(const T&, const T&)> op_evaluate;

  unsigned poblation_size;          // tamaño de las poblaciones generadas
  unsigned candidates_size;         // cantidad de individuos seleccionados
  unsigned ratio_cand_pobl;         // cantidad de copias de cada candidato

  // Genera toda la población de forma concurrente
  void generate_next_poblation () {
    poblation.clear();
    poblation.resize(poblation_size);

    std::vector<std::future<void>> promises (candidates_size);

    // función a aplicar sobre cada elemento
    auto async_function = ([&](unsigned index) {
      for (unsigned j = 0; j < ratio_cand_pobl; j++) {
        poblation[(index * ratio_cand_pobl) + j] = best_candidates[index];
      }
    });

    for (unsigned i = 0; i < candidates_size; i++)
      promises[i] = std::async(async_function, i);

    for (auto& promise : promises)
      promise.get();
  }

  // muta toda la población de forma concurrente
  void mutate_poblation () {
    std::vector<std::future<void>> promises (poblation_size - 1);

    // función a aplicar sobre cada elemento
    auto async_function = ([&](unsigned index){
      op_mutate(poblation[index]);
    });

    for (unsigned i = 0; i < poblation_size - 1; i++)
      promises[i] = std::async(async_function, i);

    for (auto& promise : promises)
      promise.get();
  }

  // evalúa toda la población y la ordena en base a la función de fitness
  void evaluate_poblation () {
    //std::sort (poblation.begin(), poblation.end(), op_evaluate);
  }

  // copia los mejores elementos de la población al vector de candidatos
  void generate_next_candidates () {
    std::copy (poblation.begin(), poblation.begin() + candidates_size, std::back_inserter(best_candidates));
  }

public:

  explicit GeneticAlgorithm(
      std::function<T(T&, T&)> operator_cross,
      std::function<void(T&)> operator_mutate,
      std::function<bool(const T&, const T&)> operator_evaluator,
      unsigned poblation_s,
      unsigned candidates_s
      ) :
        op_cross (operator_cross),
        op_mutate (operator_mutate),
        op_evaluate (operator_evaluator),
        poblation_size (poblation_s),
        candidates_size (candidates_s)
      {
        std::srand(time(nullptr));
        ratio_cand_pobl = std::round(double(poblation_size) / candidates_size);
      }

  bool step () {
    generate_next_poblation();
    mutate_poblation();
    evaluate_poblation();
    generate_next_candidates();
    return true;
  }

  void set_initial_poblation (std::vector<T>& i_poblation) {
    int diff_size = i_poblation.size() - candidates_size;
    if (diff_size > 0) {    // hay más candidatos de los que se requiere
      std::copy(i_poblation.begin(), i_poblation.end() - diff_size, std::back_inserter(best_candidates));
    } else {                // hay menos o igual candidatos de los que se requiere
      best_candidates = i_poblation;
      while (best_candidates.size() < candidates_size)  // introducir candidatos aleatorios
        best_candidates.push_back(i_poblation[std::rand() % i_poblation.size()]);
    }
  }
};

#endif // GENETICALGORITHM_H
