deps = []
deps += dependency('gl')
deps += dependency('libconfig++')
deps += dependency('glfw3')

files = ['GL_utils', 'main', 'Renderer', 'Drawbuffer', 'Config', 'xdg', 'Buffer', 'Processing', 'Utils']
src = []
foreach f: files
	src += f + '.cpp'
endforeach

args = []
args += '-DWITH_SIMD'
args += '-fopenmp'

glmvng = executable('glmvng', src, dependencies: deps, cpp_args: args)
