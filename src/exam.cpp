#include "exam.h"

#include <random>

#include "pcg_random.hpp"

// Generate exams of MCQs with the answer in ['A', 'B', 'C', 'D']
std::vector<ByteArray> generate_exams(const int32_t &number_of_exams,
                                      const int32_t &number_of_questions) {
  pcg_extras::seed_seq_from<std::random_device> seed_source;
  pcg32 rng(seed_source);

  // The list of exams
  std::vector exams(number_of_exams, ByteArray(number_of_questions));

  for (auto &exam : exams) {
    for (auto &answer : exam) {
      answer = static_cast<char>(rng(4) + 'A');
    }
  }

  return exams;
}

// Generate an exam with a list of random correct answers
ByteArray generate_correct_answers(const int32_t &number_of_questions) {
  pcg_extras::seed_seq_from<std::random_device> seed_source;
  pcg32 rng(seed_source);

  ByteArray exam(number_of_questions);

  for (auto &answer : exam) {
    answer = static_cast<char>(rng(4) + 'A');
  }

  return exam;
}

// Generate a list of MCQs' points
ByteArray generate_points(const int32_t &number_of_questions) {
  ByteArray points(number_of_questions, 2);
  return points;
}
