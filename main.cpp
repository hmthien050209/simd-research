#include "exam/exam.h"
#include "exec_timer/exec_timer.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

constexpr std::string FILE_NAME = "./exam.txt";
constexpr int NUM_OF_MCQS = 100;

using Exam = std::array<char, NUM_OF_MCQS>;
std::vector<Exam> exams;

void read_exams(const std::string &file_name) {
  ExecTimer timer("read_exams");
  std::ifstream exam_file(file_name);
  std::string line;
  while (std::getline(exam_file, line)) {
    Exam exam;
    for (int i = 0; i < NUM_OF_MCQS; ++i) {
      exam[i] = line[i];
    }
    exams.emplace_back(exam);
  }
}

int main() {
  generate_exams(FILE_NAME);
  read_exams(FILE_NAME);
  return 0;
}