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
#include <mutex>
#include <memory>

template<typename T>
class Buffer {
	std::vector<T> v_buffer;
	size_t size_;
	unsigned offset = 0;
	std::shared_mutex m;

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
		T* wdata_ = nullptr;
		Buffer<T>& buffer_;
		bool write_ = true;
		bool locked_ = true;

	public:
		Handle(Buffer<T>& buffer): buffer_(buffer){
			buffer_.read_lock();
			data_ = buffer.data();
		}

		Handle(Buffer<T>& buffer, unsigned size): buffer_(buffer){
			buffer_.write_lock();
			wdata_ = buffer.allocate(size);
			data_ = buffer.data();
			write_ = true;
		}

		Handle(const Handle& h) = delete;

		Handle(Handle&& h) noexcept: data_(h.data_), wdata_(h.wdata_),
			buffer_(h.buffer_), write_(h.write_), locked_(true){
			// don't unlock old object
			h.locked_ = false;
		}

		~Handle() {
			if(!locked_){
				return;
			}
			if(write_){
				buffer_.write_unlock();
			}else{
				buffer_.read_unlock();
			}
		}

		const T* data(){
			return data_;
		}

		T* wdata(){
			return wdata_;
		}

		//operator[]
		//operator*
	};

	friend class Handle;

	Buffer(const size_t);
	Buffer(const Buffer& b) = delete;
	Buffer(Buffer&& b): v_buffer(std::move(b.v_buffer)), size_(std::move(b.size_)), offset(std::move(b.offset)){};

	bool new_data;

	std::unique_lock<std::shared_mutex> lock();

	void write(const T buf[], const size_t);
	void write(const std::vector<T>& buf);
	void resize(const size_t);

	T* allocate(const unsigned);

	size_t bsize() const {
		return size_ * sizeof(T);
	}

	size_t size() const {
		return size_;
	}

	const T* data() const {
		return v_buffer.data() + offset;
	}

	Handle map_read() {
		return Handle(*this);
	}

	Handle map_write(unsigned size) {
		return Handle(*this, size);
	}
};


template<typename T>
struct Buffers{
	using Ptr = std::shared_ptr<Buffers>;
	std::vector<Buffer<T>> bufs;
	std::mutex mut;

	Buffers():bufs(), mut(){};

	void write(T buf[], size_t data_size);
};

template class Buffer<int16_t>;
template class Buffer<float>;
template class Buffers<int16_t>;
template class Buffers<float>;
