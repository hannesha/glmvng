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

#include "FFT.hpp"
#include <algorithm>
#include <cmath>

namespace Window{
	struct Blackman{
		float operator()(float x){
			// compensated Blackman window constants
			const float a1 = 4620.f / 3969.f;
			const float a2 = 715.f / 3969.f;
			const float pi2 = std::acos(-1.f) * 2.f;
			const float pi4 = std::acos(-1.f) * 4.f;

			return 1.f - a1 * std::cos(pi2 * x) + a2 * std::cos(pi4 * x);
		}
	};

	struct Hann{
		float operator()(float x){
			const float pi2 = std::acos(-1.f) * 2.f;

			return 1.f - std::cos(pi2 * x);
		}
	};
}

template <typename T>
constexpr size_t ptr_sizeof(T t){
	return sizeof(typename std::remove_pointer<decltype(t)>::type);
}

FFT::FFT(const size_t fft_size): size(fft_size){
	input = reinterpret_cast<decltype(input)>(fftwf_malloc(ptr_sizeof(input) * size));
	output = reinterpret_cast<decltype(output)>(fftwf_malloc(ptr_sizeof(output) * output_size()));
	plan = fftwf_plan_dft_r2c_1d(size, input, reinterpret_cast<fftwf_complex*>(output), FFTW_ESTIMATE);
}

FFT::FFT(FFT&& f){
	input = f.input;
	output = f.output;
	plan = f.plan;
	window = std::move(f.window);
	size = f.size;

	// invalidate pointers
	f.input = nullptr;
	f.output = nullptr;
	f.plan = nullptr;
	f.size = 0;
}

FFT::~FFT(){
	if (output != nullptr) fftwf_free(output);
	if (input != nullptr) fftwf_free(input);
	if (plan != nullptr) fftwf_destroy_plan(plan);
}

void FFT::resize(const size_t nsize){
	if(size != nsize){
		size = nsize;
		// destroy old plan and free memory
		fftwf_destroy_plan(plan);
		fftwf_free(input);
		fftwf_free(output);

		// create new plan and allocate memory
		input = reinterpret_cast<decltype(input)>(fftwf_malloc(ptr_sizeof(input) * size));
		output = reinterpret_cast<decltype(output)>(fftwf_malloc(ptr_sizeof(output) * output_size()));
		plan = fftwf_plan_dft_r2c_1d(size, input, reinterpret_cast<fftwf_complex*>(output), FFTW_ESTIMATE);
	}
}


template<typename T>
void FFT::calculate(Buffer<T>& buffer){
	// find smallest value for window function
	size_t window_size = std::min(size, buffer.size());
	// use the latest buffer samples
	unsigned buffer_start = std::max(0, static_cast<int>(buffer.size()) - static_cast<int>(window_size));

	if (window.size() != window_size){
		calculate_window(window_size, Window::Blackman());
	}

	auto lock = buffer.lock();
	if(buffer.new_data){
		buffer.new_data = false;

		const T* data = buffer.data();

		unsigned int i;
		for(i = 0; i < window_size; i++){
			// apply hann window with corrected factors (a * 2)
			input[i] = static_cast<float>(data[i + buffer_start]) * window[i];
		}

		lock.unlock();

		// pad remainig values
		for(; i < size; i++){
			input[i] = 0;
		}

		// execute fft
		fftwf_execute(plan);
	}
}

template<typename T>
void FFT::calculate(std::vector<T>& buffer, int start, int len){
	// find smallest value for window function
	size_t window_size = std::min(size, static_cast<size_t>(len));

	if (window.size() != window_size){
		calculate_window(window_size, Window::Blackman());
	}

	unsigned int i;
	for(i = 0; i < window_size; i++){
		// apply hann window with corrected factors (a * 2)
		input[i] = static_cast<float>(buffer[i + start]) * window[i];
	}

	// pad remainig values
	for(; i < size; i++){
		input[i] = 0;
	}

	// execute fft
	fftwf_execute(plan);
}

// return the index of the bin with the highest magnitude
size_t FFT::max_bin(const size_t start, const size_t stop){
	size_t startl = std::min(size, start);
	size_t stopl = std::min(size, stop);
	if(startl > stopl) return stopl;

	size_t ret = startl;
	float max = 0;
	for(size_t i = startl; i < stopl; i++){
		float mag = std::abs(output[i]);
		if(mag > max){
			max = mag;
			ret = i;
		}
	}

	return ret;
}

// calculate the magnitude(in dB) of the fft output
// max_amplitude specified the maximum value of the fft input (32768 for a 16 bit audio signal)
void FFT::magnitudes(Magnitudes& mag, const float max_amplitude){
	if(mag.size() != output_size()){
		mag.resize(output_size());
	}

	// scale depends on window size, due to possible padded data
	float scale = 1.f/ ((float)(window.size()) * max_amplitude);
	float scale_l = std::log10(scale);

	for(unsigned i = 0; i < size/2+1; i++){
		//mag[i] = 20.f * (std::log10(std::abs(output[i])) + scale_l);
		mag[i] = std::log10(std::abs(output[i])) + scale_l; //apply scaling later
	}
}


template <typename Fwindow>
void FFT::calculate_window(const size_t w_size, Fwindow fwindow){
	window.resize(w_size);
	float N_1 = 1.f / (w_size-1);

	for(unsigned int i = 0; i < w_size; i++){
		window[i] = fwindow(i * N_1);
	}
}

size_t FFT::output_size(){
	return size/2+1;
}

template void FFT::calculate(Buffer<int16_t>&);
template void FFT::calculate(Buffer<float>&);
template void FFT::calculate(std::vector<float>&, int, int);
