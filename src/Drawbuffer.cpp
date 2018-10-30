#include "Drawbuffer.hpp"
#include <iostream>

DrawBuffer::DrawBuffer(){
	b_left.bind();
	t_left.bind(GL_TEXTURE_BUFFER);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, b_left.id);
GLDEBUG;

	b_right.bind();
	t_right.bind(GL_TEXTURE_BUFFER);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, b_right.id);
GLDEBUG;

	b_f_left.bind();
	t_f_left.bind(GL_TEXTURE_BUFFER);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, b_f_left.id);
GLDEBUG;
	
	b_f_right.bind();
	t_f_right.bind(GL_TEXTURE_BUFFER);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, b_f_right.id);
GLDEBUG;

	GL::Texture::unbind(GL_TEXTURE_BUFFER);
}

DrawBuffer::~DrawBuffer(){}

void DrawBuffer::update(std::vector<Buffer<float>>& buffers){
	{
		auto lock = buffers[0].lock();
		b_left.bind(GL_TEXTURE_BUFFER);
		glBufferData(GL_TEXTURE_BUFFER, buffers[0].bsize(), 0, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_TEXTURE_BUFFER, 0, buffers[0].bsize(), buffers[0].data());
GLDEBUG;
	}

	if(buffers.size() > 1){
		auto lock = buffers[1].lock();
		b_right.bind(GL_TEXTURE_BUFFER);
		glBufferData(GL_TEXTURE_BUFFER, buffers[1].bsize(), 0, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_TEXTURE_BUFFER, 0, buffers[1].bsize(), buffers[1].data());
GLDEBUG;
	}

	GL::Buffer::unbind(GL_TEXTURE_BUFFER);
}

DrawBuffer::TextureHandles DrawBuffer::get_handles() const{
	TextureHandles ret(t_left, t_right, t_f_left, t_f_right);
	return ret; 
}
