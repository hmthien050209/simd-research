#ifndef EXAM_H_INCLUDED
#define EXAM_H_INCLUDED

#include <array>
#include <cstdint>
#include <vector>

constexpr int32_t NUMBER_OF_QUESTIONS = 100;
constexpr int32_t NUMBER_OF_EXAMS = 5'000'000;

using Exam = std::vector<char>;

std::vector<Exam> generate_exams();
Exam generate_correct_answers();
std::vector<int8_t> generate_points();

#endif