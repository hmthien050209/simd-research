project('simd-research', 'cpp', default_options: ['cpp_std=c++20'])

thread_dep = dependency('threads')

src = [
    'main.cpp',
    'exec_timer/exec_timer.cpp',
    'exam/exam.cpp'
]

# exec_timer
exec_timer_dep = declare_dependency(
    sources: 'exec_timer/exec_timer.h'
)

# exam
exam_dep = declare_dependency(
    sources: 'exam/exam.h'
)

extra_args = ['-mavx']

executable('main', src, dependencies : [thread_dep, exec_timer_dep, exam_dep])
