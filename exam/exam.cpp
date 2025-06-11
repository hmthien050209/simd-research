#include "exam/exam.h"
#include "exec_timer/exec_timer.h"
#include <fstream>
#include <random>

#define FAST_MOD4(x) (x & ((1u << 2) - 1))

constexpr int NUMBER_OF_QUESTIONS = 100;
constexpr int NUMBER_OF_EXAMS = 5'000'000;

void generate_exams(const std::string &file_name) {
  ExecTimer timer("generate_exams");
  std::ofstream exam_file(file_name);

  std::mt19937 mt;
  std::random_device rd;
  mt.seed(rd());

  // Output `NUMBER_OF_EXAMS` exams, and for each exam, there are
  // `NUMBER_OF_QUESTIONS` MCQs with the answer in ['A', 'B', 'C', 'D']
  for (int i = 0; i < NUMBER_OF_EXAMS; ++i) {
    for (int j = 0; j < NUMBER_OF_QUESTIONS; ++j) {
      exam_file << static_cast<char>(FAST_MOD4(mt()) + 'A');
    }
    exam_file << '\n';
  }
  exam_file.flush();
  exam_file.close();
}