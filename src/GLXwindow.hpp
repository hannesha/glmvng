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

#pragma once

#include <string>
#include <vector>
#include <functional>

#include <GL/glx.h>

class GLXwindow {
	public:
		GLXwindow();
		~GLXwindow();

		using CBFunction = std::function<void(int, int)>;

		// extension string helper
		static bool hasExt(const std::string& exts, const std::string& ext){
			return exts.find(ext) != std::string::npos;
		};

		void swapBuffers();
		void pollEvents();
		bool shouldClose();

		void setTitle(const std::string&);
		void setResizeCallback(const CBFunction& cb){
			callback = cb;
		}

		const int getWidth(){ return width; }
		const int getHeight(){ return height; }

private:
		int width, height;
		Atom wm_delete_window;
		bool should_close = false;

		CBFunction callback;

		
		Display* display;
		Colormap cmap;
		Window win;
		GLXContext ctx;

		// glXCreateContextAttribsARB function pointer alias
		using glXCreateContextAttribsARBProc = GLXContext (*)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
		glXCreateContextAttribsARBProc glXCreateContextAttribsARB = nullptr;
};
