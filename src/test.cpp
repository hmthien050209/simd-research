#include <gtest/gtest.h>

#include "exam.h"
#include "scorers.hpp"

class ScorerTestFixture : public testing::Test {
 protected:
  const std::shared_ptr<Scorer::BaseScorer> naive_scorer =
      std::make_shared<Scorer::NaiveScorer>();
  const std::shared_ptr<Scorer::BaseScorer> boolean_multiplication_scorer =
      std::make_shared<Scorer::BooleanMultiplicationScorer>();
  const std::shared_ptr<Scorer::BaseScorer> simd_scorer =
      std::make_shared<Scorer::SimdScorer>();

  std::vector<Exam> exams_size_mismatch;
  Exam correct_answers_size_mismatch;
  std::vector<int8_t> points_size_mismatch;
  std::vector<Exam> exams;
  Exam correct_answers;
  std::vector<int8_t> points;

  void SetUp() override {
    exams_size_mismatch = std::vector(1, Exam(2, 'B'));
    correct_answers_size_mismatch = {'A'};
    points_size_mismatch = {1, 2, 3};

    exams = {{
                 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
                 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
                 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
             },
             {
                 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B',
                 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B',
                 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B',
             }};
    correct_answers = {
        'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
        'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
        'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
    };
    points = std::vector<int8_t>(32, 2);
  }
};

TEST_F(ScorerTestFixture, HandleVectorSizeMismatch) {
  EXPECT_THROW(
      naive_scorer->score(exams_size_mismatch, correct_answers_size_mismatch,
                          points_size_mismatch),
      std::runtime_error);
  EXPECT_THROW(boolean_multiplication_scorer->score(
                   exams_size_mismatch, correct_answers_size_mismatch,
                   points_size_mismatch),
               std::runtime_error);
  EXPECT_THROW(
      simd_scorer->score(exams_size_mismatch, correct_answers_size_mismatch,
                         points_size_mismatch),
      std::runtime_error);
}

TEST_F(ScorerTestFixture, ScorerWorks) {
  EXPECT_EQ(
      naive_scorer->score(exams, correct_answers, points),
      boolean_multiplication_scorer->score(exams, correct_answers, points));
  EXPECT_EQ(
      boolean_multiplication_scorer->score(exams, correct_answers, points),
      simd_scorer->score(exams, correct_answers, points));
}