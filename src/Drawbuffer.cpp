#include "Drawbuffer.hpp"
#include "Utils.hpp"
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
		auto readhandle = buffers[0].map_read();
		b_left.bind(GL_TEXTURE_BUFFER);
		glBufferData(GL_TEXTURE_BUFFER, buffers[0].bsize(), 0, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_TEXTURE_BUFFER, 0, buffers[0].bsize(), readhandle.data());
GLDEBUG;
	}

	if(buffers.size() > 1){
		auto readhandle2 = buffers[1].map_read();
		b_right.bind(GL_TEXTURE_BUFFER);
		glBufferData(GL_TEXTURE_BUFFER, buffers[1].bsize(), 0, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_TEXTURE_BUFFER, 0, buffers[1].bsize(), readhandle2.data());
GLDEBUG;
	}

	GL::Buffer::unbind(GL_TEXTURE_BUFFER);
}

void DrawBuffer::update_fft(const std::vector<Magnitudes>& ffts){
	b_f_left.bind(GL_TEXTURE_BUFFER);
	glBufferData(GL_TEXTURE_BUFFER, (GLsizeiptr)Utils::csizeof(ffts.at(0)), 0, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_TEXTURE_BUFFER, 0, (GLsizeiptr)Utils::csizeof(ffts.at(0)), ffts.at(0).data());

	if(ffts.size() > 1){
		b_f_right.bind(GL_TEXTURE_BUFFER);
		glBufferData(GL_TEXTURE_BUFFER, (GLsizeiptr)Utils::csizeof(ffts[1]), 0, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_TEXTURE_BUFFER, 0, (GLsizeiptr)Utils::csizeof(ffts[1]), ffts[1].data());
	}

	GL::Buffer::unbind(GL_TEXTURE_BUFFER);
}

DrawBuffer::TextureHandles DrawBuffer::get_handles() const{
	TextureHandles ret(t_left, t_right, t_f_left, t_f_right);
	return ret; 
}
