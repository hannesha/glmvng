#pragma once

#include "Buffer.hpp"
#include "Magnitudes.hpp"

namespace Processing {
	template <typename T>
	float rms(Buffer<T>&);

	struct BinGravity{
		float bin_mag;
		float bin_time;
	};

	using GravityInfo = std::vector<BinGravity>;
	void calculate_gravity(Magnitudes& mag, GravityInfo& gravity_info, const float gravity, const float dt);
}
