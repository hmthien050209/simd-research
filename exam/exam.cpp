#include "exam/exam.h"
#include "exec_timer/exec_timer.h"
#include "pcg_random.hpp"
#include <random>

// Generate `NUMBER_OF_EXAMS` exams, and for each exam, there are
// `NUMBER_OF_QUESTIONS` MCQs with the answer in ['A', 'B', 'C', 'D']
std::vector<Exam> generate_exams() {
  ExecTimer timer("generate_exams");

  pcg_extras::seed_seq_from<std::random_device> seed_source;
  pcg32 rng(seed_source);

  // The list of exams
  std::vector<Exam> exams(NUMBER_OF_EXAMS);
  for (auto &exam : exams) {
    for (auto &answer : exam) {
      answer = static_cast<char>(rng(4) + 'A');
    }
  }

  return exams;
}

// Generate an exam with a list of random correct answers
Exam generate_correct_answers() {
  ExecTimer timer("generate_correct_answers");
  pcg_extras::seed_seq_from<std::random_device> seed_source;
  pcg32 rng(seed_source);

  Exam exam;
  for (auto &answer : exam) {
    answer = static_cast<char>(rng(4) + 'A');
  }
  return exam;
}

// Generate a list of MCQs' points
std::array<uint8_t, NUMBER_OF_QUESTIONS> generate_points() {
  ExecTimer timer("generate_points");
  std::array<uint8_t, NUMBER_OF_QUESTIONS> points;
  for (auto &x : points) {
    x = 2;
  }
  return points;
}