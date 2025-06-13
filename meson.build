project('simd-research', 'cpp',
        default_options : [
            'cpp_std=c++20',
            'buildtype=release'
        ]
)

src = [
    'main.cpp',
    'exec_timer/exec_timer.cpp',
    'exam/exam.cpp'
]

# for threading
thread_dep = dependency('threads')

# exec_timer
exec_timer_dep = declare_dependency(
    sources : 'exec_timer/exec_timer.h'
)

# exam
exam_dep = declare_dependency(
    sources : 'exam/exam.h'
)

# scorers
scorers_dep = declare_dependency(
    sources : 'scorers/scorers.hpp'
)

# pcg
pcg_dep = dependency('pcg', fallback : ['pcg', 'pcg_cpp_dep'])

extra_args = ['-mavx2']

executable('main', src, dependencies : [thread_dep, exec_timer_dep, exam_dep, scorers_dep, pcg_dep], cpp_args : extra_args)
