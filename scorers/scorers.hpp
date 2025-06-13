#ifndef SCORERS_HPP
#define SCORERS_HPP

#include<immintrin.h>

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
            ExecTimer timer("NaiveScorer");

            std::vector<int32_t> scored_exams_points(exams.size());

            for (size_t i = 0; i < exams.size(); ++i) {
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
            ExecTimer timer("BooleanMultiplicationScorer");

            std::vector<int32_t> scored_exams_points(exams.size());

            for (size_t i = 0; i < exams.size(); ++i) {
                for (size_t j = 0; j < exams[i].size(); ++j) {
                    // Idea: to reduce false branch predictions
                    scored_exams_points[i] +=
                            (exams[i][j] == correct_answers[j]) * static_cast<int32_t>(points[j]);
                }
            }

            return scored_exams_points;
        }
    };

    class SimdScorer final : public BaseScorer {
        std::vector<int32_t> score(const std::vector<Exam> &exams,
                                   const Exam &correct_answers,
                                   const std::vector<int8_t> &points) override {
            if (!__builtin_cpu_supports("avx2")) {
                throw std::runtime_error("SIMD scorer not supported because the CPU lacks AVX2 support.");
            }

            ExecTimer timer("SimdScorer");
            std::vector<int32_t> scored_exams_points(exams.size());
            // We are targeting AVX2, so we have at most 256-bit registers,
            // which means that we can pack 32 MCQs to score at a time
            constexpr int32_t BATCH_SIZE = 32;

            // Process each exam
            for (size_t i = 0; i < exams.size(); ++i) {
                auto &exam = exams[i];
                int32_t exam_scored_points = 0;

                size_t j = 0;

                for (; j + BATCH_SIZE < correct_answers.size(); j += BATCH_SIZE) {
                    // Vectorize
                    // Because we're using std::vector (as we're unaware of the correct sizes in the reality), we can't
                    // use _mm256_load_si256
                    const __m256i vec_answers = _mm256_set_epi8(exam[j + 31], exam[j + 30], exam[j + 29], exam[j + 28],
                                                                exam[j + 27], exam[j + 26], exam[j + 25], exam[j + 24],
                                                                exam[j + 23], exam[j + 22], exam[j + 21], exam[j + 20],
                                                                exam[j + 19], exam[j + 18], exam[j + 17], exam[j + 16],
                                                                exam[j + 15], exam[j + 14], exam[j + 13], exam[j + 12],
                                                                exam[j + 11], exam[j + 10], exam[j + 9], exam[j + 8],
                                                                exam[j + 7], exam[j + 6], exam[j + 5], exam[j + 4],
                                                                exam[j + 3], exam[j + 2], exam[j + 1], exam[j]);
                    const __m256i vec_corrects = _mm256_set_epi8(correct_answers[j + 31], correct_answers[j + 30],
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
                    const __m256i vec_points = _mm256_set_epi8(points[j + 31], points[j + 30], points[j + 29],
                                                               points[j + 28],
                                                               points[j + 27], points[j + 26], points[j + 25],
                                                               points[j + 24],
                                                               points[j + 23], points[j + 22], points[j + 21],
                                                               points[j + 20],
                                                               points[j + 19], points[j + 18], points[j + 17],
                                                               points[j + 16],
                                                               points[j + 15], points[j + 14], points[j + 13],
                                                               points[j + 12],
                                                               points[j + 11], points[j + 10], points[j + 9],
                                                               points[j + 8],
                                                               points[j + 7], points[j + 6], points[j + 5],
                                                               points[j + 4],
                                                               points[j + 3], points[j + 2], points[j + 1], points[j]);
                    // Mark the correct answers with 0xff, otherwise 0
                    const __m256i vec_marks = _mm256_cmpeq_epi8(vec_answers, vec_corrects);

                    // Imagine that we have a mark vector of 3 elements 0xff00ff, and the points are 0x010101,
                    // 0xff00ff & 0x010101 = 0x010001, which is correct. Hence, the use of bitwise AND.
                    const __m256i vec_final_points = _mm256_and_si256(vec_marks, vec_points);
                    // Reference:
                    // https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#text=_mm256_sad_epu8&ig_expand=5674
                    // This will produce 4 sums, stored in the first 16 bits of every 64-bit block inside the 256-bit vector
                    const __m256i vec_final_points_4_sum = _mm256_sad_epu8(vec_final_points, _mm256_setzero_si256());
                    exam_scored_points += _mm256_extract_epi16(vec_final_points_4_sum, 0)
                            + _mm256_extract_epi16(vec_final_points_4_sum, 4)
                            + _mm256_extract_epi16(vec_final_points_4_sum, 8)
                            + _mm256_extract_epi16(vec_final_points_4_sum, 12);
                }

                // Process the remaining batch (if any)
                for (; j < correct_answers.size(); ++j) {
                    exam_scored_points += (exam[j] == correct_answers[j]) * points[j];
                }

                scored_exams_points[i] = exam_scored_points;
            }

            return scored_exams_points;
        }
    };
} // namespace Scorer
#endif
