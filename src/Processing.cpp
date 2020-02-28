#include "Processing.hpp"
#include <numeric>
#include <cmath>

#pragma omp declare simd
inline float square(float x){
	return x*x;
}

// calculate the rms value of all the audio data in the buffer
template <typename T>
float Processing::rms(Buffer<T>& buffer){
	auto lock = buffer.lock();
	auto data = buffer.data();
	auto size = buffer.size();
	float rms = 0;
	//unsigned i = 0;

	//#pragma omp simd reduction(+:rms)
	//for(i = 0; i < size; i++){
	//	rms += square(data[i]);
	//}
	rms = std::transform_reduce(data, data + size, data, 0);

	return std::sqrt(rms / size);
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
