#pragma once

#include <map>
#include <string>
#include "Scalar.hpp"

using Vec4 = std::array<float, 4>;

using ShaderConfig = std::map<std::string, Scalar>;
using ShaderVectors = std::map<std::string, Vec4>;
