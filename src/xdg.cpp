/*
 *	Copyright (C) 2017  Hannes Haberl
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

#include "xdg.hpp"

#include <pwd.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <cstdlib>

#include <filesystem>

using namespace std;
using filesystem::path;
using filesystem::exists;

constexpr string_view xdg_config_dirs_default = "/etc/xdg";

namespace xdg{
	path default_config_home(){
		string config_home;
		// get default config directory
		const char* env_home = getenv("HOME");
		if(env_home != nullptr){
			config_home = env_home;
		}else{
			struct passwd* pw = ::getpwuid(::geteuid());
			if(pw){
				config_home = pw->pw_dir;
			}else{
				return "";
			}
		}
		config_home += "/.config";

		return config_home;
	}

	path config_home(){
		path config_home;
		const char* cxdg_cfg_home = getenv("XDG_CONFIG_HOME");
		if(cxdg_cfg_home != nullptr){
			config_home = cxdg_cfg_home;
		}

		if(config_home.empty())
			config_home = default_config_home();

		return config_home;
	}

	vector<path> config_dirs(){
		string config_dirs;

		const char* cxdg_cfg_dirs = getenv("XDG_CONFIG_DIRS");
		if(cxdg_cfg_dirs != nullptr)
			config_dirs = cxdg_cfg_dirs;

		if(config_dirs.empty())
			config_dirs = xdg_config_dirs_default;

		vector<path> dirs;
		istringstream ss(config_dirs);
		string dir;

		while(getline(ss, dir, ':')){
			path directory = dir;

			if(directory.is_absolute())
				dirs.push_back(directory);
		}

		return dirs;
	}

	path find_config(const path& cfg_file){
		path config;
		// check XDG_CONFIG_HOME
		config = config_home() / cfg_file;
		if(exists(config)) return config;

		// check XDG_CONFIG_DIRS
		for(auto& dir : config_dirs()){
			config = dir / cfg_file;
			if(exists(config)) return config;
		}

		// return empty string if no config file was found
		return "";
	}
}
