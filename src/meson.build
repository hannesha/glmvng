deps = []
deps += dependency('gl')
deps += dependency('libconfig++')
deps += dependency('glfw3')
deps += dependency('libpulse')
deps += dependency('x11')
deps += dependency('fftw3f')
deps += dependency('threads')

files = ['GL_utils', 'main', 'Renderer', 'Drawbuffer', 'Config', 'xdg', 'Buffer', 'Processing', 'Utils', 'Pulse_Async', 'GLFW', 'GLXwindow', 'FFT']
src = []
foreach f: files
	src += f + '.cpp'
endforeach


args = []
args += '-DWITH_SIMD'
args += '-fopenmp'

glmvng = executable('glmvng', src, dependencies: deps, cpp_args: args)

buffer_src = files('Buffer.cpp')
src_dir = include_directories('.')
subdir('tests')
