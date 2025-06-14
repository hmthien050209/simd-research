import sys
import matplotlib.pyplot as plt
import json


def render_graphs(file_name: str):
    file = open(file_name, "r")
    data = json.load(file)
    benchmarks = data["benchmarks"]

    exam_nums = [100_000, 5_000_000, 10_000_000]
    mcq_nums = [10, 100, 200]

    benchmarks_dict = {}
    for benchmark in benchmarks:
        benchmarks_dict[benchmark["name"]] = benchmark["real_time"]

    for exam_num in exam_nums:
        fig, ax = plt.subplots()
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

        ax.plot(mcq_nums, naive_val, 'o-', label="Naive")
        ax.plot(mcq_nums, boolean_mul_val, 'o-', label="Boolean Multiplication")
        ax.plot(mcq_nums, simd_val, 'o-', label="SIMD")

        # Display the value above every data point
        for x, y in zip(mcq_nums, naive_val):
            ax.text(x, y - 25, f"{int(y)}", ha="center")
        for x, y in zip(mcq_nums, boolean_mul_val):
            ax.text(x, y + 25, f"{int(y)}", ha="center")
        for x, y in zip(mcq_nums, simd_val):
            ax.text(x, y - 25, f"{int(y)}", ha="center")

        ax.set_title(f"Scoring time for {exam_num} exams")
        ax.set_xlabel("Number of MCQs")
        ax.set_xticks(mcq_nums)
        ax.set_ylabel("Time (ms)")
        ax.legend()
        ax.grid(True)

        plt.savefig(f"graphs/scoring_time_{exam_num}.png")
        plt.clf()


if __name__ == "__main__":
    if len(sys.argv) <= 1:
        print("Usage: benchmark_graphs.py <json file name>")
        sys.exit(1)
    render_graphs(sys.argv[1])
