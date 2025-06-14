#ifndef SCORERS_HPP
#define SCORERS_HPP

#include <immintrin.h>

#include "exam.h"

namespace Scorer {
class BaseScorer {
 public:
  virtual ~BaseScorer() = default;

  virtual std::vector<int32_t> score(const std::vector<Exam> &exams,
                                     const Exam &correct_answers,
                                     const std::vector<int8_t> &points) = 0;
};

class NaiveScorer final : public BaseScorer {
 public:
  std::vector<int32_t> score(const std::vector<Exam> &exams,
                             const Exam &correct_answers,
                             const std::vector<int8_t> &points) override {
    if (exams.empty()) {
      return {};
    }

    if (correct_answers.size() != points.size()) {
      throw std::runtime_error(
          "The size of correct answers and points must be the same.");
    }

    std::vector<int32_t> scored_exams_points(exams.size());

    for (size_t i = 0; i < exams.size(); ++i) {
      if (exams[i].size() != correct_answers.size()) {
        throw std::runtime_error(
            "The size of exams' questions and correct answers must be the "
            "same.");
      }

      for (size_t j = 0; j < exams[i].size(); ++j) {
        if (exams[i][j] == correct_answers[j]) {
          scored_exams_points[i] += static_cast<int32_t>(points[j]);
        }
      }
    }

    return scored_exams_points;
  }
};

class BooleanMultiplicationScorer final : public BaseScorer {
 public:
  std::vector<int32_t> score(const std::vector<Exam> &exams,
                             const Exam &correct_answers,
                             const std::vector<int8_t> &points) override {
    if (exams.empty()) {
      return {};
    }

    if (correct_answers.size() != points.size()) {
      throw std::runtime_error(
          "The size of correct answers and points must be the same.");
    }

    std::vector<int32_t> scored_exams_points(exams.size());

    for (size_t i = 0; i < exams.size(); ++i) {
      if (exams[i].size() != correct_answers.size()) {
        throw std::runtime_error(
            "The size of exams' questions and correct answers must be the "
            "same.");
      }

      for (size_t j = 0; j < exams[i].size(); ++j) {
        // Idea: to reduce false branch predictions
        scored_exams_points[i] += (exams[i][j] == correct_answers[j]) *
                                  static_cast<int32_t>(points[j]);
      }
    }

    return scored_exams_points;
  }
};

class SimdScorer final : public BaseScorer {
 public:
  std::vector<int32_t> score(const std::vector<Exam> &exams,
                             const Exam &correct_answers,
                             const std::vector<int8_t> &points) override {
    if (!__builtin_cpu_supports("avx2")) {
      throw std::runtime_error(
          "SIMD checker not supported because the CPU lacks AVX2 support.");
    }

    if (exams.empty()) {
      return {};
    }

    const auto answers_size = correct_answers.size();

    if (answers_size != points.size()) {
      throw std::runtime_error(
          "The size of correct answers and points must be the same.");
    }

    std::vector<int32_t> scored_exams_points(exams.size(), 0);
    // We are targeting AVX2, so we have at most 256-bit registers,
    // which means that we can pack 32 MCQs to score at a time.
    // ReSharper disable once CppTooWideScopeInitStatement
    constexpr int32_t BATCH_SIZE = 32;

    // Process each exam
    for (size_t i = 0; i < exams.size(); ++i) {
      auto &exam = exams[i];

      // Prefetch the next exam
      // https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html
      if (i + 1 < exams.size()) {
        __builtin_prefetch(&exams[i + 1]);
      }

      if (exam.size() != answers_size) {
        throw std::runtime_error(
            "The size of exams' questions and correct answers must be the "
            "same.");
      }

      size_t j = 0;

      for (; j + BATCH_SIZE < answers_size; j += BATCH_SIZE) {
        // Because we're using std::vector (as we're unaware of the correct
        // sizes in the reality), we can't use _mm256_load_si256
        // Vectorize exam's MCQs
        __m256i v1 = _mm256_set_epi8(
            exam[j + 31], exam[j + 30], exam[j + 29], exam[j + 28],
            exam[j + 27], exam[j + 26], exam[j + 25], exam[j + 24],
            exam[j + 23], exam[j + 22], exam[j + 21], exam[j + 20],
            exam[j + 19], exam[j + 18], exam[j + 17], exam[j + 16],
            exam[j + 15], exam[j + 14], exam[j + 13], exam[j + 12],
            exam[j + 11], exam[j + 10], exam[j + 9], exam[j + 8], exam[j + 7],
            exam[j + 6], exam[j + 5], exam[j + 4], exam[j + 3], exam[j + 2],
            exam[j + 1], exam[j]);
        // Vectorize correct MCQs
        __m256i v2 =
            _mm256_set_epi8(correct_answers[j + 31], correct_answers[j + 30],
                            correct_answers[j + 29], correct_answers[j + 28],
                            correct_answers[j + 27], correct_answers[j + 26],
                            correct_answers[j + 25], correct_answers[j + 24],
                            correct_answers[j + 23], correct_answers[j + 22],
                            correct_answers[j + 21], correct_answers[j + 20],
                            correct_answers[j + 19], correct_answers[j + 18],
                            correct_answers[j + 17], correct_answers[j + 16],
                            correct_answers[j + 15], correct_answers[j + 14],
                            correct_answers[j + 13], correct_answers[j + 12],
                            correct_answers[j + 11], correct_answers[j + 10],
                            correct_answers[j + 9], correct_answers[j + 8],
                            correct_answers[j + 7], correct_answers[j + 6],
                            correct_answers[j + 5], correct_answers[j + 4],
                            correct_answers[j + 3], correct_answers[j + 2],
                            correct_answers[j + 1], correct_answers[j]);
        // Mark the correct answers with 0xff, otherwise 0
        v1 = _mm256_cmpeq_epi8(v1, v2);
        // Vectorize points
        v2 = _mm256_set_epi8(
            points[j + 31], points[j + 30], points[j + 29], points[j + 28],
            points[j + 27], points[j + 26], points[j + 25], points[j + 24],
            points[j + 23], points[j + 22], points[j + 21], points[j + 20],
            points[j + 19], points[j + 18], points[j + 17], points[j + 16],
            points[j + 15], points[j + 14], points[j + 13], points[j + 12],
            points[j + 11], points[j + 10], points[j + 9], points[j + 8],
            points[j + 7], points[j + 6], points[j + 5], points[j + 4],
            points[j + 3], points[j + 2], points[j + 1], points[j]);

        // Imagine that we have a mark vector of 3 elements 0xff00ff, and the
        // points are 0x010101, 0xff00ff & 0x010101 = 0x010001, which is
        // correct. Hence, the use of bitwise AND.
        v1 = _mm256_and_si256(v1, v2);
        // Reference:
        // https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#text=_mm256_sad_epu8&ig_expand=5674
        // This will produce 4 sums, stored in the first 16 bits of every 64-bit
        // block inside the 256-bit vector
        v1 = _mm256_sad_epu8(v1, _mm256_setzero_si256());
        scored_exams_points[i] +=
            _mm256_extract_epi16(v1, 0) + _mm256_extract_epi16(v1, 4) +
            _mm256_extract_epi16(v1, 8) + _mm256_extract_epi16(v1, 12);
      }

      // Process the remaining batch (if any)
      for (; j < answers_size; ++j) {
        scored_exams_points[i] += (exam[j] == correct_answers[j]) * points[j];
      }
    }

    return scored_exams_points;
  }
};

class SimdAvx512Scorer final : public BaseScorer {
 public:
  std::vector<int32_t> score(const std::vector<Exam> &exams,
                             const Exam &correct_answers,
                             const std::vector<int8_t> &points) override {
    // The idea is the same as the AVX2 functions, the only difference is that
    // we have double the registers' size (512 bits instead of 256 bits)
    if (!__builtin_cpu_supports("avx512bw") ||
        !__builtin_cpu_supports("avx512vl") ||
        !__builtin_cpu_supports("avx512f") ||
        !__builtin_cpu_supports("avx512dq")) {
      throw std::runtime_error(
          "SIMD checker not supported because the CPU lacks AVX512{BW,VL,F,DQ} "
          "support.");
    }

    if (exams.empty()) {
      return {};
    }

    const auto answers_size = correct_answers.size();

    if (answers_size != points.size()) {
      throw std::runtime_error(
          "The size of correct answers and points must be the same.");
    }

    std::vector<int32_t> scored_exams_points(exams.size());
    // ReSharper disable once CppTooWideScopeInitStatement
    constexpr int32_t BATCH_SIZE = 64;

    // Process each exam
    for (size_t i = 0; i < exams.size(); ++i) {
      auto &exam = exams[i];

      // Prefetch the next exam
      if (i + 1 < exams.size()) {
        __builtin_prefetch(&exams[i + 1]);
      }

      if (exam.size() != answers_size) {
        throw std::runtime_error(
            "The size of exams' questions and correct answers must be the "
            "same.");
      }

      size_t j = 0;

      for (; j + BATCH_SIZE < answers_size; j += BATCH_SIZE) {
        __m512i v1 = _mm512_set_epi8(
            exam[j + 63], exam[j + 62], exam[j + 61], exam[j + 60],
            exam[j + 59], exam[j + 58], exam[j + 57], exam[j + 56],
            exam[j + 55], exam[j + 54], exam[j + 53], exam[j + 52],
            exam[j + 51], exam[j + 50], exam[j + 49], exam[j + 48],
            exam[j + 47], exam[j + 46], exam[j + 45], exam[j + 44],
            exam[j + 43], exam[j + 42], exam[j + 41], exam[j + 40],
            exam[j + 39], exam[j + 38], exam[j + 37], exam[j + 36],
            exam[j + 35], exam[j + 34], exam[j + 33], exam[j + 32],
            exam[j + 31], exam[j + 30], exam[j + 29], exam[j + 28],
            exam[j + 27], exam[j + 26], exam[j + 25], exam[j + 24],
            exam[j + 23], exam[j + 22], exam[j + 21], exam[j + 20],
            exam[j + 19], exam[j + 18], exam[j + 17], exam[j + 16],
            exam[j + 15], exam[j + 14], exam[j + 13], exam[j + 12],
            exam[j + 11], exam[j + 10], exam[j + 9], exam[j + 8], exam[j + 7],
            exam[j + 6], exam[j + 5], exam[j + 4], exam[j + 3], exam[j + 2],
            exam[j + 1], exam[j]);
        __m512i v2 =
            _mm512_set_epi8(correct_answers[j + 63], correct_answers[j + 62],
                            correct_answers[j + 61], correct_answers[j + 60],
                            correct_answers[j + 59], correct_answers[j + 58],
                            correct_answers[j + 57], correct_answers[j + 56],
                            correct_answers[j + 55], correct_answers[j + 54],
                            correct_answers[j + 53], correct_answers[j + 52],
                            correct_answers[j + 51], correct_answers[j + 50],
                            correct_answers[j + 49], correct_answers[j + 48],
                            correct_answers[j + 47], correct_answers[j + 46],
                            correct_answers[j + 45], correct_answers[j + 44],
                            correct_answers[j + 43], correct_answers[j + 42],
                            correct_answers[j + 41], correct_answers[j + 42],
                            correct_answers[j + 39], correct_answers[j + 38],
                            correct_answers[j + 37], correct_answers[j + 36],
                            correct_answers[j + 35], correct_answers[j + 34],
                            correct_answers[j + 33], correct_answers[j + 32],
                            correct_answers[j + 31], correct_answers[j + 30],
                            correct_answers[j + 29], correct_answers[j + 28],
                            correct_answers[j + 27], correct_answers[j + 26],
                            correct_answers[j + 25], correct_answers[j + 24],
                            correct_answers[j + 23], correct_answers[j + 22],
                            correct_answers[j + 21], correct_answers[j + 20],
                            correct_answers[j + 19], correct_answers[j + 18],
                            correct_answers[j + 17], correct_answers[j + 16],
                            correct_answers[j + 15], correct_answers[j + 14],
                            correct_answers[j + 13], correct_answers[j + 12],
                            correct_answers[j + 11], correct_answers[j + 10],
                            correct_answers[j + 9], correct_answers[j + 8],
                            correct_answers[j + 7], correct_answers[j + 6],
                            correct_answers[j + 5], correct_answers[j + 4],
                            correct_answers[j + 3], correct_answers[j + 2],
                            correct_answers[j + 1], correct_answers[j]);
        // -1 = 0b1111'1111'1111...1111: full 1s
        v1 = _mm512_mask_mov_epi8(v1, _mm512_cmpeq_epi8_mask(v1, v2),
                                  _mm512_set1_epi64(-1));
        v2 = _mm512_set_epi8(
            points[j + 63], points[j + 62], points[j + 61], points[j + 60],
            points[j + 59], points[j + 58], points[j + 57], points[j + 56],
            points[j + 55], points[j + 54], points[j + 53], points[j + 52],
            points[j + 51], points[j + 50], points[j + 49], points[j + 48],
            points[j + 47], points[j + 46], points[j + 45], points[j + 44],
            points[j + 43], points[j + 42], points[j + 41], points[j + 42],
            points[j + 39], points[j + 38], points[j + 37], points[j + 36],
            points[j + 35], points[j + 34], points[j + 33], points[j + 32],
            points[j + 31], points[j + 30], points[j + 29], points[j + 28],
            points[j + 27], points[j + 26], points[j + 25], points[j + 24],
            points[j + 23], points[j + 22], points[j + 21], points[j + 20],
            points[j + 19], points[j + 18], points[j + 17], points[j + 16],
            points[j + 15], points[j + 14], points[j + 13], points[j + 12],
            points[j + 11], points[j + 10], points[j + 9], points[j + 8],
            points[j + 7], points[j + 6], points[j + 5], points[j + 4],
            points[j + 3], points[j + 2], points[j + 1], points[j]);

        v1 = _mm512_and_si512(v1, v2);
        v1 = _mm512_sad_epu8(v1, _mm512_setzero_si512());

        // Split the 512-bit vector into two 256-bit vectors and then sum them
        // up like we did in AVX2
        const auto lo = _mm512_extracti32x8_epi32(v1, 0);
        const auto hi = _mm512_extracti32x8_epi32(v1, 1);
        scored_exams_points[i] +=
            _mm256_extract_epi16(lo, 0) + _mm256_extract_epi16(lo, 4) +
            _mm256_extract_epi16(lo, 8) + _mm256_extract_epi16(lo, 12) +
            _mm256_extract_epi16(hi, 0) + _mm256_extract_epi16(hi, 4) +
            _mm256_extract_epi16(hi, 8) + _mm256_extract_epi16(hi, 12);
      }

      // Process the remaining batch (if any)
      for (; j < answers_size; ++j) {
        scored_exams_points[i] += (exam[j] == correct_answers[j]) * points[j];
      }
    }

    return scored_exams_points;
  }
};
}  // namespace Scorer
#endif
