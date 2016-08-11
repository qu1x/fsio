// This file is part of fsio, see <https://qu1x.org/fsio>.
// 
// Copyright (c) 2016 Rouven Spreckels <n3vu0r@qu1x.org>
// 
// fsio is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License version 3
// as published by the Free Software Foundation on 19 November 2007.
// 
// fsio is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
// 
// You should have received a copy of the GNU Affero General Public License
// along with fsio. If not, see <https://www.gnu.org/licenses>.

#ifndef _FSIO_MAP_H
#define _FSIO_MAP_H

#include "fsio.hxx"

#include <string>
#include <cstring>
#include <cerrno>

#include <sys/mman.h>

template<typename T>
class fsio_map final: public fsio::map {
	static const int prot = PROT_READ | PROT_WRITE;
	static const int flag = MAP_SHARED;
	std::size_t poll;
	volatile const T* i;
	volatile T* o;
	T data;
	bool dual;
public:
	fsio_map(int fd, std::size_t i = 0, std::size_t o = 0, bool dual = false) {
		poll = 0;
		this->dual = dual;
		this->i = (volatile T*)mmap(nullptr, sizeof (T), prot, flag, fd, i);
		if (this->i == MAP_FAILED)
			throw std::string("Cannot map base memory: ")
				+ std::strerror(errno);
		if (dual)
			this->o = (volatile T*)((volatile const char*)this->i + o);
		else {
			this->o = (volatile T*)mmap(nullptr, sizeof (T), prot, flag, fd, o);
			if (this->o == MAP_FAILED)
				throw std::string("Cannot map dual memory: ")
					+ std::strerror(errno);
		}
		own();
	}
	void free() {
		if (i != MAP_FAILED && munmap((T*)i, sizeof (T)) == -1)
			throw std::string("Cannot unmap base memory: ")
				+ std::strerror(errno);
		if (!dual)
			if (o != MAP_FAILED && munmap((T*)o, sizeof (T)) == -1)
				throw std::string("Cannot unmap dual memory: ")
					+ std::strerror(errno);
		i = o = (volatile T*)MAP_FAILED;
	}
	~fsio_map() override { try { free(); } catch (...) {} }
	auto size() const -> std::size_t override { return sizeof (T); }
	void wave() override { *o = data = !data; }
#if FSIO_STAT
	void feed() override { ++poll; *o = data = *i; }
	auto back() -> bool override { ++poll; return data == *i; }
#else
	void feed() override { *o = data = *i; }
	auto back() -> bool override { return data == *i; }
#endif
	auto stat() -> std::size_t {
		auto poll = this->poll;
		this->poll = 0;
		return poll;
	}
	void own() override { data = *i; }
	auto get(void* data) const -> std::size_t override {
		*((T*)data) = this->data;
		return sizeof (T);
	}
	auto put(const void* data) -> std::size_t override {
		*o = this->data = *((const T*)data);
		return sizeof (T);
	}
};

#endif
