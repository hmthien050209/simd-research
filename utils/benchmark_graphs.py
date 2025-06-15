import sys
import matplotlib.pyplot as plt
import json
from pathlib import Path


def render_benchmarks_in_dir(dir_path: str):
    dir = Path(dir_path)
    for file_path in dir.glob('*.json'):
        render_graphs(file_path)


def render_graphs(file_path: Path):
    file = file_path.open(mode='r')
    file_name = file_path.name
    data = json.load(file)
    benchmarks = data["benchmarks"]

    exam_nums = [100_000, 5_000_000, 10_000_000]
    mcq_nums = [10, 100, 200]

    benchmarks_dict = {}
    for benchmark in benchmarks:
        benchmarks_dict[benchmark["name"]] = benchmark["real_time"]

    for exam_num in exam_nums:
        # The figure size will be 1280x720
        fig, ax = plt.subplots(figsize=(12.8, 7.2), dpi=100)
        naive_val = [
            benchmarks_dict[f'BM_NaiveScorer/{exam_num}/10'],
            benchmarks_dict[f'BM_NaiveScorer/{exam_num}/100'],
            benchmarks_dict[f'BM_NaiveScorer/{exam_num}/200'],
        ]
        boolean_mul_val = [
            benchmarks_dict[f'BM_BooleanMultiplicationScorer/{exam_num}/10'],
            benchmarks_dict[f'BM_BooleanMultiplicationScorer/{exam_num}/100'],
            benchmarks_dict[f'BM_BooleanMultiplicationScorer/{exam_num}/200'],
        ]
        simd_val = [
            benchmarks_dict[f'BM_SimdScorer/{exam_num}/10'],
            benchmarks_dict[f'BM_SimdScorer/{exam_num}/100'],
            benchmarks_dict[f'BM_SimdScorer/{exam_num}/200'],
        ]
        simd_avx512_val = [
            benchmarks_dict[f'BM_SimdAvx512Scorer/{exam_num}/10'],
            benchmarks_dict[f'BM_SimdAvx512Scorer/{exam_num}/100'],
            benchmarks_dict[f'BM_SimdAvx512Scorer/{exam_num}/200'],
        ]

        ax.plot(mcq_nums, naive_val, 'o-', label="Naive")
        ax.plot(mcq_nums, boolean_mul_val, 'o-', label="Boolean Multiplication")
        ax.plot(mcq_nums, simd_val, 'o-', label="SIMD AVX2")
        ax.plot(mcq_nums, simd_avx512_val, 'o-', label="SIMD AVX512")

        # Display the value above every data point
        for x, y in zip(mcq_nums, naive_val):
            ax.text(x, y - 25, f"{int(y)}", ha="center")
        for x, y in zip(mcq_nums, boolean_mul_val):
            ax.text(x, y + 25, f"{int(y)}", ha="center")
        for x, y in zip(mcq_nums, simd_val):
            ax.text(x, y - 25, f"{int(y)}", ha="center")
        for x, y in zip(mcq_nums, simd_avx512_val):
            ax.text(x, y + 25, f"{int(y)}", ha="center")

        ax.set_title(f"Scoring time for {exam_num} exams\nBenchmark: {file_name}")
        ax.set_xlabel("Number of MCQs")
        ax.set_xticks(mcq_nums)
        ax.set_ylabel("Time (ms)")
        ax.legend()
        ax.grid(True)

        plt.savefig(f"graphs/{file_name}_{exam_num}.png")
        plt.clf()


if __name__ == "__main__":
    if len(sys.argv) <= 1:
        print("Usage: benchmark_graphs.py <directory>")
        sys.exit(1)
    render_benchmarks_in_dir(sys.argv[1])
