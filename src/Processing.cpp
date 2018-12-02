#include "Processing.hpp"

// calculate the rms value of all the audio data in the buffer
template <typename T>
float Processing::rms(Buffer<T>& buffer){
	auto lock = buffer.lock();
	auto data = buffer.data();
	auto size = buffer.size();
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

//TODO add gravity calculation, add magnitude calculation, add slope functions
void Processing::calculate_gravity(Magnitudes& mag, GravityInfo& gravity_info, const float gravity, const float dt){
	if(mag.size() > gravity_info.size()){
		gravity_info.resize(mag.size());
	}

	for(unsigned i = 0; i < mag.size(); i++){
		float y_old = gravity_info[i].bin_mag - gravity *  gravity_info[i].bin_time;
		float y = mag[i];

		if(y_old > y){
			// apply gravity, keep old value
			mag[i] = y_old;
			gravity_info[i].bin_mag = y_old;
			gravity_info[i].bin_time += dt;
		}else{
			// use new value, reset time
			gravity_info[i].bin_mag = y;
			gravity_info[i].bin_time = 0;
		}
	}
}

template float Processing::rms(Buffer<int16_t>&);
template float Processing::rms<float>(Buffer<float>&);
