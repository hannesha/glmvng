#include "Renderer.hpp"
#include "Drawbuffer.hpp"

Renderer::Renderer(const RenderConfig& renderconf, std::shared_ptr<DrawBuffer>& draw_buf): renderconfig(renderconf), drawbuffer(draw_buf)
{
	link_shaders(renderconfig.shaders);

	configure();
}

void Renderer::link_shaders(const RenderConfig::Shaders& shaders){
	for(auto sh : shaders){
		shader.attach(*sh);
	}
	glLinkProgram(shader.get_id());
	for(auto sh : shaders){
		shader.detach(*sh);
	}
	shader.check_link_status();
GLDEBUG;
}

void Renderer::draw(){
	shader();
	vao();
GLDEBUG;
	
	DrawBuffer::TextureHandles textures = drawbuffer->get_handles();
	glActiveTexture(GL_TEXTURE0);
	textures.t_left(GL_TEXTURE_BUFFER);
GLDEBUG;

	glActiveTexture(GL_TEXTURE1);
	textures.t_right(GL_TEXTURE_BUFFER);
GLDEBUG;

	glActiveTexture(GL_TEXTURE2);
	textures.t_f_left(GL_TEXTURE_BUFFER);
GLDEBUG;

	glActiveTexture(GL_TEXTURE3);
	textures.t_f_right(GL_TEXTURE_BUFFER);
GLDEBUG;

	glDrawArrays(renderconfig.drawtype, 0, renderconfig.output_size);
GLDEBUG;

	GL::VAO::unbind();
	GL::Texture::unbind(GL_TEXTURE_BUFFER);
}

void Renderer::set_uniform(const ShaderConfig::value_type& val, GL::Program& sh) {
	GLint loc = sh.get_uniform(val.first.c_str());
	//std::cout << loc << std::endl;
	if(loc < 0) {
		// log unused value
		//std::cout << "Ignoring: " << val.first << std::endl;
		return;
	}

	switch(val.second.type) {
	case Scalar::Type::INT:
		//std::cout << "loc: " << loc << " setting int: " << val.first << std::endl;
		glUniform1i(loc, val.second.value.i);
		GL::get_error("set int");
		break;

	case Scalar::Type::FLOAT:
		//std::cout << "loc: " << loc << " setting float: " << val.first << std::endl;
		glUniform1f(loc, val.second.value.f);
		GL::get_error("set float");
		break;

	default:
		std::cout << "ignoring value: " << val.first << std::endl;
	}
}

void Renderer::configure(){
	shader();
	// set texture buffer locations
	glUniform1i(shader.get_uniform("t_left"), 0);
	glUniform1i(shader.get_uniform("t_right"), 1);
	glUniform1i(shader.get_uniform("t_f_left"), 2);
	glUniform1i(shader.get_uniform("t_f_right"), 3);

	for(auto& uniform : renderconfig.uniforms){
		set_uniform(uniform, shader);
	}
GLDEBUG;
}
