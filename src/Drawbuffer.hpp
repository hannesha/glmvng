#pragma once

#include "GL_utils.hpp"
#include "Buffer.hpp"
// buffer textures
// update(buffer&)
//

class DrawBuffer{
public:
	DrawBuffer();
	~DrawBuffer();

	void update(std::vector<Buffer<float>>& );
//	void update_fft(const std::vector<FFT>& ffts);

	struct TextureHandles{
		const GL::Texture &t_left, &t_right, &t_f_left, &t_f_right;
		TextureHandles(const GL::Texture &t_left_, const GL::Texture &t_right_, const GL::Texture &t_f_left_, const GL::Texture &t_f_right_): t_left(t_left_), t_right(t_right_), t_f_left(t_f_left_), t_f_right(t_f_right_){};
	};

	TextureHandles get_handles() const;

private:
	GL::Texture t_left, t_right, t_f_left, t_f_right;
	GL::Buffer b_left, b_right, b_f_left, b_f_right;
}; 
