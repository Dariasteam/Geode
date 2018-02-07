#ifndef GENETICALGORITHM_H
#define GENETICALGORITHM_H

#include <functional>
#include <vector>
#include <future>
#include <algorithm>
#include <cmath>
#include <iostream>


template <class T>
/**
 * @brief Framework parametrizado para el manejo de algoritmos genéticos.
 * Este sistema trata de abstraer al programador de los procesos relacionados
 * con el algoritmo realizando las operaciones de cruce, mutación y evaluación
 * por él según sea conveniente.
 *
 * El programador debe proporcionar no obstante 3 funciones que definan
 * estos operadores para los individuos de tipo #T con los que se emplearán.
 *
 * La generación de población y su mutación se realiza de forma concurrente,
 * asignándole un hilo a cada individuo.
 */
class GeneticAlgorithm {
private:
  std::vector <T> population;       // population generated on each simualtion
  std::vector <T> best_candidates;  // best candidates in a simulation

  std::function<T(T&, T&)> op_cross;
  std::function<void(T&, unsigned)> op_mutate;
  std::function<double(const T&)> op_evaluate;

  unsigned mutation_rate;

  unsigned population_size;
  unsigned candidates_size;
  unsigned ratio_cand_pobl;      // number of copies generated of each candidate

  // Concurrently generates the population
  void generate_next_population () {
    population.clear();
    population.resize(population_size);

    std::vector<std::future<void>> promises (candidates_size);

    ratio_cand_pobl = std::floor(double(population_size) / candidates_size);

    auto async_function = ([&](unsigned index) {
      for (unsigned j = 0; j < ratio_cand_pobl; j++)
        population[(index * ratio_cand_pobl) + j] = op_cross(best_candidates[index], best_candidates[rand() % candidates_size]);
    });

    for (unsigned i = 0; i < candidates_size; i++)
      promises[i] = std::async(async_function, i);

    for (auto& promise : promises)
      promise.get();

    // conpensates the odd cases
    unsigned rest = population_size % candidates_size;

    if (rest > 0) {
      unsigned index = candidates_size * ratio_cand_pobl;

      for (unsigned i = 0; i < rest; i++)
        population[index + i] = best_candidates[i];
    }
  }

  // Concurrently mutates all population
  // Last best 2 candidates are also pushed without mutations to guarantee
  // actual optimal solutions are not lost
  void mutate_population () {
    std::vector<std::future<void>> promises (population_size - 2);

    // función a aplicar sobre cada elemento
    auto async_function = ([&](unsigned index) {
      op_mutate(population[index], mutation_rate);
    });    

    for (unsigned i = 0; i < population_size - 2; i++)
      promises[i] = std::async(async_function, i);

    for (auto& promise : promises)
      promise.get();

    population[population_size - 2] = best_candidates[0];
    population[population_size - 1] = best_candidates[1];
  }

  void sort_population (std::vector<double>& evaluations) {
    for (unsigned i = 0; i < population_size; i++) {
      double best = evaluations[i];
      unsigned best_index = i;
      for (unsigned j = i + 1; j < population_size; j++) {
        if (evaluations[j] > best) {
          best_index = j;
          best = evaluations[j];
        }
      }
      if (best_index != i) {
        // exchange individuals
        auto aux = population[i];
        population[i] = population[best_index];
        population[best_index] = aux;

        // exchange scores
        auto aux2 = evaluations[i];
        evaluations[i] = evaluations[best_index];
        evaluations[best_index] = aux2;
      }
    }

    double old_value = evaluations[0];
    double value;
  }

  // Evaluates all population and orders it based on the fitness function
  void evaluate_population () {
    std::vector<double> evaluations (population_size);

    for (unsigned i = 0; i < population_size; i++)
      evaluations[i] = op_evaluate(population[i]);

    sort_population (evaluations);
  }

  // Copy best individuals of the current population to the candidates vector
  void generate_next_candidates () {
    best_candidates.clear();
    std::copy (population.begin(), population.begin() + candidates_size, std::back_inserter(best_candidates));    
    // TODO Cross them!!!
  }

public:
  inline std::vector<T> get_best_candidates () { return best_candidates; }

  /**
   * @brief Constructor del algoritmo genético
   * @param operator_cross Función lambda que expecifica de qué manera han de cruzarse los genomas de individuos #T
   * @param operator_mutate Función lambda que expecifica de qué manera han de mutarse los genomas de individuos #T
   * @param operator_evalautor Función lambda que expecifica de qué manera han de compararse (función fitness) los genomas de individuos #T
   * @param population_s Tamaño de la población generada en cada iteración por medio de cruces
   * @param candidates_s Tamaño de la muestra seleccionada por la función evaluadora.
   */
  explicit GeneticAlgorithm(
      std::function<T(T&, T&)> operator_cross,
      std::function<void(T&, unsigned)> operator_mutate,
      std::function<double(const T&)> operator_evaluator,
      unsigned population_s,
      unsigned candidates_s,
      unsigned mutation_r
      ) :

        op_cross (operator_cross),
        op_mutate (operator_mutate),
        op_evaluate (operator_evaluator),

        mutation_rate (mutation_r),
        population_size (population_s),
        candidates_size (candidates_s)
      {
        std::srand(time(nullptr));
        ratio_cand_pobl = std::round(double(population_size) / candidates_size);
      }

  /**
  * @brief Adjust parameters of the simulation any time
  *
  * @param population_s p_population_s: population size
  * @param candidates_s p_candidates_s: candidates size
  * @param mutation_r p_mutation_r: mutation rate
  */
  void set_population_parameters (unsigned population_s, unsigned candidates_s,
                                  unsigned mutation_r) {
    mutation_rate = mutation_r;
    if (population_s > candidates_s) {
      population_size = population_s;
      candidates_size = candidates_s;
      ratio_cand_pobl = std::round(double(population_size) / candidates_size);
    }
  }

  /**
  * @brief Simulates a complete generation. (Create population, mutate it, evaluate it, generate new one)
  *
  *
  * @return currently always true
  */
  bool step () {
    generate_next_population();
    mutate_population();
    evaluate_population();
    generate_next_candidates();
    return true;
  }

  /**
  * @brief Makes the simulation advance half step by generatin a new population
  * and mutate it. #set_external_evaluations should be used to complete the
  * iteration.
  *
  */
  void semi_step () {
    generate_next_population();
    mutate_population();
  }

  std::vector<T> get_population () {
    return population;
  }

  /**
  * @brief Sets a external evaluation for each individiual. Completes the second
  * half of the iteration, initiated by #semi_step.
  *
  * @param evaluations p_evaluations: Should have the same size as the population.
  * Each i element contains the score of the i individual from population.
  */
  void set_external_evaluations (std::vector<double> evaluations) {
    /*
    std::vector<std::pair<T, double>> aux (population_size);
    for (unsigned i = 0; i < population_size; i++)
      aux[i] = {population[i], evaluations[i]};

    auto comparator = [&](const std::pair<T, double>& A, const std::pair<T, double>& B) {
      return A.second > B.second;
    };
    std::sort (aux.begin(), aux.end(), comparator);

    population.clear();
    population.resize(population_size);

    for (unsigned i = 0; i < population_size; i++)
      population[i] = aux[i].first;
    */

    sort_population (evaluations);
    generate_next_candidates();
  }

  /**
  * @brief Sets the initial population of the simulation. Automatically adjust
  * the size to accomplish with the one seted in #GeneticAlgorithm or
  * #set_poblation_parameters by copy / delete elements.
  *
  * @param i_population: population
  */
  void set_initial_population (const std::vector<T>& i_population) {
    int diff_size = i_population.size() - candidates_size;
    if (diff_size > 0) {    // hay más candidatos de los que se requiere
      best_candidates = i_population;
      best_candidates.resize(candidates_size);
    } else {                // hay menos o igual candidatos de los que se requiere
      best_candidates = i_population;
      unsigned size = i_population.size();
      best_candidates.resize(candidates_size);
      for (unsigned i = size - 1; i < candidates_size; i++)
        best_candidates[i] = i_population[std::rand() % i_population.size()];
    }
  }

  /**
  * @brief Print the best candidates and its score
  */
  void print_best () {
    std::cout << "Best candidates and score \n";

    for (auto& candidate : best_candidates) {
      std::cout << op_evaluate(candidate) << "\n";
    }
    std::cout << std::flush;

  }
};

#endif // GENETICALGORITHM_H
