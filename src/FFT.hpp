/*
 *	Copyright (C) 2016  Hannes Haberl
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

#include <fftw3.h>

#include <complex>
#include <memory>

#include "Buffer.hpp"
#include "Magnitudes.hpp"

class FFT {
	public:
		using Input_t = float;
		using Output_t = std::complex<Input_t>;

		FFT(size_t);
		// disable copy construction
		FFT(const FFT&) = delete;

		// allow move construction
		FFT(FFT&&);
		FFT& operator=(FFT&&) = default;
		~FFT();

		template<typename T> void calculate(Buffer<T>&);
		template<typename T> void calculate(std::vector<T>&, int, int);
		void resize(size_t);

		size_t max_bin(size_t, size_t);
		void magnitudes(Magnitudes&, float);
		size_t output_size();

		std::unique_ptr<Output_t[]> output;
	private:
		std::unique_ptr<Input_t[]> input;
		fftwf_plan plan;
		size_t size;

		template <typename Fwindow> void calculate_window(size_t, Fwindow);
		std::vector<float> window;


};
