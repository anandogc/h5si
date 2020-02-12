/* H5SI
 *
 * Copyright 2020 Mahendra K. Verma, Anando Gopal Chatterjee
 *
 * Mahendra K. Verma
 * Indian Institute of Technology, Kanpur-208016
 * UP, India
 *
 * mkv@iitk.ac.in
 *
 * This file is part of H5SI.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * \file  h5vector_ops.h
 * @author  A. G. Chatterjee
 * @date feb 2012
 * @bug  No known bugs
 */

#ifndef _H_VECTOR_OPS
#define _H_VECTOR_OPS

#include <vector>
#include <algorithm>
#include <functional>
#include <blitz/array.h>
// #include <iostream>

namespace VecOps {

	template<typename T, int nD>
	static std::vector<T> to_vector(blitz::TinyVector<T,nD> tvec) {
		std::vector<T> vec(nD);

		vec.assign(tvec.data(), tvec.data()+nD);

		return vec;
	}

	template<typename T>
	std::vector<T> assign(std::vector<T> &vec, int val) {
		vec.assign(vec.size(), val);
		
		return vec;
	}

	template<typename T>
	std::vector<T> range_assign(std::vector<T> vec, blitz::Range r, T val) {
		for (typename std::vector<T>::size_type i=r.first(0); i<r.last(vec.size())-1; i++)
			vec[i] = val;
		
		return vec;
	}

	template<typename T>
	std::vector<T> add(std::vector<T> vec, T val) {
		std::transform(vec.begin(), vec.end(), vec.begin(),
				std::bind1st(std::plus<T>(), val));

		return vec;
	}

	template<typename T>
	std::vector<T> add(std::vector<T> vec1, std::vector<T> vec2) {
		std::transform(vec1.begin(), vec1.end(), vec2.begin(), vec1.begin(),
				std::plus<T>());

		return vec1;
	}

	template<typename T>
	std::vector<T> subtract(std::vector<T> vec, T val) {
		std::transform(vec.begin(), vec.end(), vec.begin(),
				std::bind1st(std::minus<T>(), val));

		return vec;
	}

	template<typename T>
	std::vector<T> subtract(std::vector<T> vec1, std::vector<T> vec2) {
		std::transform(vec1.begin(), vec1.end(), vec2.begin(), vec1.begin(),
				std::minus<T>());

		return vec1;
	}

	template<typename T>
	std::vector<T> multiply(std::vector<T> vec, T val) {
		std::transform(vec.begin(), vec.end(), vec.begin(),
				std::bind1st(std::multiplies<T>(), val));

		return vec;
	}

	template<typename T>
	std::vector<T> multiply(std::vector<T> vec1, std::vector<T> vec2) {
		std::transform(vec1.begin(), vec1.end(), vec2.begin(), vec1.begin(),
				std::multiplies<T>());

		return vec1;
	}

	template<typename T>
	std::vector<T> divide(std::vector<T> vec, T val) {
		std::transform(vec.begin(), vec.end(), vec.begin(),
				std::bind1st(std::divides<T>(), val));

		return vec;
	}

	template<typename T>
	std::vector<T> divide(std::vector<T> vec1, std::vector<T> vec2) {
		std::transform(vec1.begin(), vec1.end(), vec2.begin(), vec1.begin(),
				std::divides<T>());

		return vec1;
	}
}

#endif
