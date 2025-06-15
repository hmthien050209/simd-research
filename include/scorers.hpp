#ifndef SCORERS_HPP
#define SCORERS_HPP

#include <immintrin.h>

#include "exam.h"

namespace Scorer {
class BaseScorer {
 public:
  virtual ~BaseScorer() = default;

  virtual std::vector<int32_t> score(const std::vector<ByteArray> &exams,
                                     const ByteArray &correct_answers,
                                     const ByteArray &points) = 0;
};

class NaiveScorer final : public BaseScorer {
 public:
  std::vector<int32_t> score(const std::vector<ByteArray> &exams,
                             const ByteArray &correct_answers,
                             const ByteArray &points) override {
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
  std::vector<int32_t> score(const std::vector<ByteArray> &exams,
                             const ByteArray &correct_answers,
                             const ByteArray &points) override {
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
  std::vector<int32_t> score(const std::vector<ByteArray> &exams,
                             const ByteArray &correct_answers,
                             const ByteArray &points) override {
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

      for (size_t j = 0, _j = 0; j < correct_answers.block_count_avx2();
           ++j, _j = j << 5) {
        // Vectorize exam's MCQs
        __m256i v1 = _mm256_loadu_si256(
            reinterpret_cast<const __m256i *>(exam.data() + _j));
        // Vectorize correct MCQs
        __m256i v2 = _mm256_loadu_si256(
            reinterpret_cast<const __m256i *>(correct_answers.data() + _j));

        // Mark the correct answers with 0xff, otherwise 0
        v1 = _mm256_cmpeq_epi8(v1, v2);
        // Vectorize points
        v2 = _mm256_loadu_si256(
            reinterpret_cast<const __m256i *>(points.data() + _j));

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
    }

    return scored_exams_points;
  }
};

#if defined(__AVX512BW__) && defined(__AVX512VL__) && defined(__AVX512F__) && \
    defined(__AVX512DQ__)
class SimdAvx512Scorer final : public BaseScorer {
 public:
  std::vector<int32_t> score(const std::vector<ByteArray> &exams,
                             const ByteArray &correct_answers,
                             const ByteArray &points) override {
    // The idea is the same as the AVX2 functions, the only difference is that
    // we have double the registers' size (512 bits instead of 256 bits)
    if (!__builtin_cpu_supports("avx512bw") ||
        !__builtin_cpu_supports("avx512vl") ||
        !__builtin_cpu_supports("avx512f") ||
        !__builtin_cpu_supports("avx512dq")) {
      throw std::runtime_error(
          "SIMD AVX512 checker not supported because the CPU lacks "
          "AVX512{BW,VL,F,DQ} "
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

      for (size_t j = 0, _j = 0; j < correct_answers.block_count_avx512();
           ++j, _j = j << 6) {
        // Load the exam and the correct answers
        __m512i v1 = _mm512_loadu_si512(exam.data() + _j);
        const __m512i v2 = _mm512_loadu_si512(correct_answers.data() + _j);

        // Compute the mask
        const __mmask64 mask = _mm512_cmpeq_epi8_mask(v1, v2);

        // Load the points
        v1 = _mm512_loadu_si512(points.data() + _j);

        // The masked points
        v1 = _mm512_maskz_mov_epi8(mask, v1);

        // Final sum calculation
        v1 = _mm512_sad_epu8(v1, _mm512_setzero_si512());
        uint64_t sum[8];
        _mm512_storeu_si512(sum, v1);

        for (const auto &k : sum) {
          // In fact, the results are stored at the lower 16-bit of each 64-bit
          // group, so we can safely cast here
          scored_exams_points[i] += static_cast<int32_t>(k);
        }
      }
    }

    return scored_exams_points;
  }
};
#endif
}  // namespace Scorer
#endif
