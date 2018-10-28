#include "Utils.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

std::string Utils::load_source(const std::string& path) {
	try {
		std::ifstream file(path);
		file.exceptions(std::ifstream::failbit);
		std::ostringstream content;
		content << file.rdbuf();
		return content.str();
	}
	catch(std::ios_base::failure& err) {
		std::stringstream msg;
		msg << "failed to load source: " << path << std::endl
		<< "ifstream message: " << err.what() << std::endl
		<< "error code: " << err.code() << std::endl;
		throw std::runtime_error(msg.str());
	}
}
