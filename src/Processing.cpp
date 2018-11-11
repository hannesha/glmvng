#include "Processing.hpp"

// calculate the rms value of all the audio data in the buffer
template <typename T>
float Processing::rms(Buffer<T>& buffer){
	auto lock = buffer.lock();
	auto data = buffer.data();
	auto size = buffer.size_();
	float rms = 0;
	unsigned i = 0;

	constexpr unsigned N = 4;
	// temp sum vector
	alignas(16) std::array<float,N> vrms;
	for(unsigned k=0; k<N; k++) vrms[k] = 0;

	for(; i < size/N; i++){
		// load 4 integers and convert them to float
		alignas(16) std::array<float, N> vdata;
		for(unsigned k=0; k<N; k++) vdata[k] = static_cast<float>(data[i*N + k]);

		#pragma omp simd
		for(unsigned k=0; k<N; k++) vrms[k] += vdata[k] * vdata[k];
	}
	// calculate sum
	for(unsigned k=0; k<N; k++) rms += vrms[k];

	// calculate remaining values
	i *= N;
	for(; i < size; i++){
		 rms += data[i] * data[i];
	}
	return rms;
}

template float Processing::rms(Buffer<int16_t>&);
template float Processing::rms<float>(Buffer<float>&);
