#include <iostream>
#include <vector>

#include "exam.h"
#include "exec_timer.h"
#include "scorers.hpp"

int main() {
  // The synthetic tests
  const std::vector<Exam> exams = generate_exams();
  const Exam correct_answers = generate_correct_answers();
  const std::vector<int8_t> points = generate_points();

  // The scorers to be tested
  const std::vector<std::shared_ptr<Scorer::BaseScorer>> scorers = {
      std::make_shared<Scorer::NaiveScorer>(),
      std::make_shared<Scorer::BooleanMultiplicationScorer>(),
      std::make_shared<Scorer::SimdScorer>(),
  };
  std::vector<std::vector<int32_t>> scored_results;
  for (const auto &scorer : scorers) {
    scored_results.emplace_back(scorer->score(exams, correct_answers, points));
  }

  // Comparison logic
  if (scorers.size() != scored_results.size()) {
    std::cout << "Error: the number of scorers and results is different."
              << std::endl;
    return 1;
  }

  for (size_t i = 0; i < scored_results.size() - 1; ++i) {
    if (scored_results[i].size() != scored_results[i + 1].size()) {
      const std::string message = std::format(
          "Error: the number of exams between "
          "scorers is different: scorer {0}: {1} "
          "elements, scorer {2}: {3} elements.",
          i, scored_results[i].size(), i + 1, scored_results[i + 1].size());
      std::cout << message << std::endl;
      return 1;
    }

    for (size_t j = 0; j < scored_results[i].size(); ++j) {
      if (scored_results[i][j] != scored_results[i + 1][j]) {
        const std::string message = std::format(
            "Error: the score of exam {0} is different: scorer {1}: {2}, "
            "scorer {3}: {4}",
            j, i, scored_results[i][j], i + 1, scored_results[i + 1][j]);
        std::cout << message << std::endl;
        return 1;
      }
    }
  }

  std::cout << "All tests passed." << std::endl;
  return 0;
}
