#include "exam/exam.h"
#include "exec_timer/exec_timer.h"

#include <assert.h>
#include <iostream>
#include <vector>

namespace Scorer {
class BaseScorer {
public:
  virtual ~BaseScorer() = default;
  virtual std::vector<uint32_t> score(const std::vector<Exam> &exams,
                                      const Exam &correct_answers,
                                      const std::vector<uint8_t> &points) = 0;
};

class NaiveScorer final : public BaseScorer {
public:
  std::vector<uint32_t> score(const std::vector<Exam> &exams,
                              const Exam &correct_answers,
                              const std::vector<uint8_t> &points) override {
    ExecTimer timer("NaiveScorer");
    std::vector<uint32_t> scored_exams_points(exams.size());
    for (size_t i = 0; i < exams.size(); ++i) {
      for (size_t j = 0; j < exams[i].size(); ++j) {
        if (exams[i][j] == correct_answers[j]) {
          scored_exams_points[i] += points[j];
        }
      }
    }
    return scored_exams_points;
  }
};

class MultithreadedNaiveScorer final : public BaseScorer {
  std::vector<uint32_t> score(const std::vector<Exam> &exams,
                              const Exam &correct_answers,
                              const std::vector<uint8_t> &points) override {
    ExecTimer timer("MultithreadedNaiveScorer");
    std::vector<uint32_t> scored_exams_points(exams.size());
    throw std::runtime_error("Not implemented");
    return scored_exams_points;
  }
};
}; // namespace Scorer

int main() {
  // The synthetic tests
  const std::vector<Exam> exams = generate_exams();
  const Exam correct_answers = generate_correct_answers();
  const std::vector<uint8_t> points = generate_points();

  // The scorers to be tested
  const std::vector<std::shared_ptr<Scorer::BaseScorer>> scorers = {
      std::make_shared<Scorer::NaiveScorer>(),
      // std::make_shared<Scorer::BaseScorer>(Scorer::MultithreadedNaiveScorer()),
  };
  std::vector<std::vector<uint32_t>> results;
  for (const auto &scorer : scorers) {
    results.emplace_back(scorer->score(exams, correct_answers, points));
  }

  // Comparison logic
  assert(scorers.size() == results.size());
  for (auto it = results.begin(); it != results.end() - 1; ++it) {
    auto &result = *it;
    auto &next = *(it + 1);

    assert(result.size() == next.size());
    for (size_t i = 0; i < result.size(); ++i) {
      assert(result[i] == next[i]);
    }
  }
  return 0;
}