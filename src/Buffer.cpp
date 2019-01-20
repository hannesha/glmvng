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

#include "Buffer.hpp"
#include <type_traits>
#include <algorithm>

template<typename T>
Buffer<T>::Buffer(const size_t size){
	static_assert(std::is_arithmetic<T>::value, "Buffer<T> only supports arithmetic types!");

	size_ = 0;
	resize(size);

	offset = 0;
}

template<typename T>
std::unique_lock<std::shared_mutex> Buffer<T>::lock(){
	return std::unique_lock<std::shared_mutex>(m);
}

template<typename T>
void Buffer<T>::write(const T buf[], const size_t n){
	Buffer<T>::Handle handle  = map_write(n);
	new_data = true;
	std::copy(buf, buf + n, handle.wdata());
}

template<typename T>
void Buffer<T>::write(const std::vector<T>& buf){
	write(buf.data(), buf.size());
}

template<typename T>
void Buffer<T>::resize(const size_t n){
	// allocate larger chunk
	const unsigned factor = 3;
	auto lock = this->lock();
	if(size() != n){
		size_ = n;
		v_buffer.resize(n * factor);
		new_data = true;
	}
}


template<typename T>
T* Buffer<T>::allocate(const unsigned data_size){
	if(data_size > size()){
		throw std::runtime_error("Requested data size is too large");
	}

	offset = offset + data_size;

	unsigned diff = size() - data_size;
	if(offset > (v_buffer.size() - size())){
		// move current data to the front
		std::copy(v_buffer.begin() + offset, v_buffer.begin() + offset + diff, v_buffer.begin());
		offset = 0;
	}

	return &(v_buffer[offset]) + diff;
}

template<typename T>
void Buffers<T>::write(T buf[], size_t data_size){
	// limit data size
	auto &buffer = bufs[0];
	int n = bufs.size();
	if(data_size > buffer.size() * n){
		auto diff = data_size - buffer.size() * n;
		data_size = buffer.size()*n;
		buf += diff;
	}
	if(bufs.size() > 1){
		// acquire handles and map buffers
		std::vector<typename Buffer<T>::Handle> handles;
		std::vector<T*> data_ptrs;
		for(auto& buffer : bufs){
			handles.push_back(std::move(buffer.map_write(data_size/n)));
			data_ptrs.push_back(handles.back().wdata());
		}

		// interleaved write
		for(unsigned i = 0; i < data_size; i++){
			data_ptrs[i%n][i/n] = buf[i];
		}
	}else{
		//use direct write

		buffer.write(buf, data_size);
	}
}
