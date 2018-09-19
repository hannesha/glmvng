deps = []
deps += dependency('gl')
deps += dependency('libconfig++')
deps += dependency('glfw3')

files = ['GL_utils', 'main', 'Renderer', 'Drawbuffer', 'Config', 'xdg', 'Buffer', 'Processing']
src = []
foreach f: files
	src += f + '.cpp'
endforeach

glmvng = executable('glmvng', src, dependencies: deps)
