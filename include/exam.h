#ifndef EXAM_H_INCLUDED
#define EXAM_H_INCLUDED

#include <cstdint>
#include <vector>

using Exam = std::vector<char>;

std::vector<Exam> generate_exams(const int32_t &number_of_exams,
                                 const int32_t &number_of_questions);
Exam generate_correct_answers(const int32_t &number_of_questions);
std::vector<int8_t> generate_points(const int32_t &number_of_questions);

#endif