/*
 *	Copyright (C) 2017  Hannes Haberl
 *
 *	This file is part of GLMViz.
 *
 *	GLMViz is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	GLMViz is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with GLMViz.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "GL_utils.hpp"
#include <stdexcept>

namespace GL{
	class Multisampler{
		public:
			Multisampler(const int nsamples, const int w, const int h){
				resize(nsamples, w, h);
				};

			inline void bind(){ fbms.bind(); };
			inline void blit(const int w1, const int h1, const int w2, const int h2){
				fbms(GL_READ_FRAMEBUFFER);
				GLDEBUG;
				FBO::unbind(GL_DRAW_FRAMEBUFFER);
				GLDEBUG;
				glBlitFramebuffer(0, 0, w1, h1, 0, 0, w2, h2, GL_COLOR_BUFFER_BIT, GL_LINEAR);
				GLDEBUG;

				// bind default FBO
				FBO::unbind();
				};
			inline void blit(const int w, const int h){
				blit(w, h, w, h);
				GLDEBUG;
			};

			void resize(const int nsamples, const int w, const int h){
				if((w < fb_w && h < fb_h) && (w + margin >= fb_w && h + margin >= fb_h)){
					std::cout << "skipping resize" << std::endl;
					return;
				}

				fb_w = w + margin/2;
				fb_h = h + margin/2;
				std::cout << "Resizing framebuffer to "  << fb_h << "x" << fb_w << std::endl;
				fbms = FBO();
				tex_fb = Texture();
				if(nsamples > 0){
					tex_fb.bind(GL_TEXTURE_2D_MULTISAMPLE);
					glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, nsamples, GL_RGBA, fb_w, fb_h, GL_TRUE);
					fbms.bind();
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, tex_fb.id, 0);
					if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
						GLDEBUG;
						throw std::invalid_argument("Wrong number of Samples");
					}
					Texture::unbind(GL_TEXTURE_2D_MULTISAMPLE);
				}else{
					tex_fb.bind(GL_TEXTURE_2D);
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fb_w, fb_h, 0, GL_RGBA, GL_BYTE, nullptr);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_fb.id, 0);
					Texture::unbind(GL_TEXTURE_2D);
				}
				samples = nsamples;
				FBO::unbind();
			};

			FBO fbms;
			Texture tex_fb;
			int samples = 0, fb_w = 0, fb_h = 0;

			static constexpr int margin = 20;
	};
}
