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

#pragma once

#include <vector>
#include <cstdint>
#include <shared_mutex>
#include <memory>


template<typename T>
class Buffer {
	std::vector<T> v_buffer;
	size_t size_;
	std::shared_mutex m;

	std::vector<T> ibuf; // intermediate buffer for interleaved writes
	void i_write(const T buf[], const size_t);
	//void i_write(const std::vector<T>&, const size_t);

	void read_lock(){
		m.lock_shared();
	};
	void read_unlock(){
		m.unlock_shared();
	};
	void write_lock(){
		m.lock();
	};
	void write_unlock(){
		m.unlock();
	};

public:
	class Handle {
		const T* data_;
		Buffer<T>& buffer_;
		bool write_ = true;

	public:
		Handle(Buffer<T>& buffer): buffer_(buffer){
			buffer_.read_lock();
			data_ = buffer.data();
		}

		Handle(Buffer<T>& buffer, unsigned size): buffer_(buffer){
			buffer_.write_lock();
			//data_ = buffer.allocate(size);
			write_ = true;
		}

		~Handle() {
			if(write_){
				buffer_.write_unlock();
			}else{
				buffer_.read_unlock();
			}
		}

		const T* data(){
			return data_;
		}

		//operator[]
		//operator*
	};

	friend class Handle;

	Buffer(const size_t);
	Buffer(const Buffer& b) = delete;
	Buffer(Buffer&& b): v_buffer(std::move(b.v_buffer)), size_(std::move(b.size_)){};
	//Buffer& operator=(Buffer&& b){ v_buffer = std::move(b.v_buffer); size = std::move(b.size);  return *this; };

	bool new_data;

	std::unique_lock<std::shared_mutex> lock();

	void write(T buf[], const size_t);
	void write(const std::vector<T>& buf);
	void write_offset(T buf[], const size_t, const size_t, const size_t);
	void write_offset(const std::vector<T>& buf, const size_t, const size_t);
	void resize(const size_t);

	size_t bsize() const {
		return size_ * sizeof(T);
	}

	size_t size() const {
		return size_;
	}

	const T* data() const {
		return v_buffer.data();
	}

	const Handle map_read() {
		return Handle(*this);
	}

	const Handle map_write(unsigned size) {
		return Handle(*this, size);
	}


	const decltype(v_buffer)& vector() const {
		return v_buffer;
	}
};


template<typename T>
struct Buffers{
	using Ptr = std::shared_ptr<Buffers>;
	std::vector<Buffer<T>> bufs;
	std::mutex mut;

	Buffers():bufs(), mut(){};
};

template class Buffer<int16_t>;
template class Buffer<float>;
template class Buffers<int16_t>;
template class Buffers<float>;
