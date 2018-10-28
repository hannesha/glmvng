#pragma once

#include <string>
#include <map>
#include <iostream>

namespace Utils {
	std::string load_source(const std::string& path);

	template<typename K, typename V>
	void dump_map(const std::map<K, V>& m) {
		for(const auto& i : m) {
			std::cout << "[" << i.first << "]: " << i.second << std::endl;
		}
	}
}
