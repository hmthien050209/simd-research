#include <benchmark/benchmark.h>

#include "exam.h"
#include "scorers.hpp"

static void BM_NaiveScorer(benchmark::State& state) {
  for (auto _ : state) {
    state.PauseTiming();
    auto exams = generate_exams(state.range(0), state.range(1));
    auto correct_answers = generate_correct_answers(state.range(1));
    auto points = generate_points(state.range(1));
    auto naive_scorer = std::make_shared<Scorer::NaiveScorer>();
    benchmark::ClobberMemory();
    state.ResumeTiming();

    auto result = naive_scorer->score(exams, correct_answers, points);
    benchmark::DoNotOptimize(result);
  }
}

BENCHMARK(BM_NaiveScorer)
    ->ArgsProduct({{100'000, 5'000'000, 10'000'000}, {10, 100, 200}})
    ->Unit(benchmark::kMillisecond);

static void BM_BooleanMultiplicationScorer(benchmark::State& state) {
  for (auto _ : state) {
    state.PauseTiming();
    auto exams = generate_exams(state.range(0), state.range(1));
    auto correct_answers = generate_correct_answers(state.range(1));
    auto points = generate_points(state.range(1));
    auto boolean_multiplication_scorer =
        std::make_shared<Scorer::BooleanMultiplicationScorer>();
    benchmark::ClobberMemory();
    state.ResumeTiming();

    auto result =
        boolean_multiplication_scorer->score(exams, correct_answers, points);
    benchmark::DoNotOptimize(result);
  }
}

BENCHMARK(BM_BooleanMultiplicationScorer)
    ->ArgsProduct({{100'000, 5'000'000, 10'000'000}, {10, 100, 200}})
    ->Unit(benchmark::kMillisecond);

static void BM_SimdScorer(benchmark::State& state) {
  for (auto _ : state) {
    state.PauseTiming();
    auto exams = generate_exams(state.range(0), state.range(1));
    auto correct_answers = generate_correct_answers(state.range(1));
    auto points = generate_points(state.range(1));
    auto simd_scorer = std::make_shared<Scorer::SimdScorer>();
    benchmark::ClobberMemory();
    state.ResumeTiming();

    auto result = simd_scorer->score(exams, correct_answers, points);
    benchmark::DoNotOptimize(result);
  }
}

BENCHMARK(BM_SimdScorer)
    ->ArgsProduct({{100'000, 5'000'000, 10'000'000}, {10, 100, 200}})
    ->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();