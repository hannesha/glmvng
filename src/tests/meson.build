b_test_src = ['buffertest.cpp', buffer_src]
threads = dependency('threads')
b_test_exe = executable('b_test', b_test_src, include_directories: src_dir, dependencies: threads)
test('buffer test', b_test_exe)
