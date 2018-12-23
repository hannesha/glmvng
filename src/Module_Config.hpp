/*
 *	Copyright (C) 2016,2017  Hannes Haberl
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

#include <string>
#include <tuple>

namespace Module_Config {
	enum class Source {FIFO, PULSE};

	struct Input {
		Source source = Source::PULSE;
		std::string file = "/tmp/mpd.fifo";
		std::string device = "";
		bool stereo = false;
		int f_sample = 48000;
		int fft_size = 4096;
		int latency = 1100; // f_sample * s_latency(0.025 s)
		int buffer_len = 4096;

		inline bool operator==(const Input& rhs) const{
			return std::tie(source, stereo, f_sample)
				== std::tie(rhs.source, rhs.stereo, rhs.f_sample);
		}
	};

	struct Transformation {
		float Xmin = -1, Xmax = 1, Ymin = -1, Ymax = 1;
	};

	struct Color {
		float rgba[4];

		inline void normalize(const Color& c){
			std::copy(c.rgba, c.rgba + 4, rgba);
			normalize();
		}
		inline void normalize(){
			// convert screen color(CRT gamma) to sRGB
			for(int i = 0; i < 3; i ++){
				rgba[i] = rgba[i] / 255;
			}
		}
	};
}
