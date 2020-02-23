/* H5SI
 *
 * Copyright (C) 2020, Mahendra K. Verma, Anando Gopal Chatterjee
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
 * \file  h5select.h
 * @author  A. G. Chatterjee
 * @date feb 2012
 * @bug  No known bugs
 */

#ifndef _H_H5SELECT
#define _H_H5SELECT

#include <string>
#include <algorithm>
#include <sstream>
#include <blitz/array.h>

#include <vector>

namespace h5 {
    
class Expression;

class Select {
    char sign_;               // '+' or '-'
    std::vector<blitz::Range> range_;    // range to select along X, Y, and Z respectively

    public:

    blitz::Range Parse_indices(std::string indices);

    Select(blitz::Range range0) {
        this->sign_ = '+';
        this->range_.reserve(1);
        this->range_.push_back(range0);
    }

    Select(blitz::Range range0, blitz::Range range1) {
        this->sign_ = '+';
        this->range_.reserve(2);
        this->range_.push_back(range0);
        this->range_.push_back(range1);
    }

    Select(blitz::Range range0, blitz::Range range1, blitz::Range range2) {
        this->sign_ = '+';
        this->range_.reserve(3);
        this->range_.push_back(range0);
        this->range_.push_back(range1);
        this->range_.push_back(range2);
    }

    Select(blitz::Range range0, blitz::Range range1, blitz::Range range2, blitz::Range range3) {
        this->sign_ = '+';
        this->range_.reserve(4);
        this->range_.push_back(range0);
        this->range_.push_back(range1);
        this->range_.push_back(range2);
        this->range_.push_back(range3);
    }

    Select(blitz::Range range0, blitz::Range range1, blitz::Range range2, blitz::Range range3, blitz::Range range4) {
        this->sign_ = '+';
        this->range_.reserve(5);
        this->range_.push_back(range0);
        this->range_.push_back(range1);
        this->range_.push_back(range2);
        this->range_.push_back(range3);
        this->range_.push_back(range4);
    }

    Select(std::string select);

    static Select all(int nD);

    std::vector<blitz::Range> get_range() { return range_; }
    char get_sign() { return sign_; }

    void Negate() {this->sign_ = (this->sign_ == '+' ? '-' : '+');}

    void Pretty_print(std::string &str, bool print_positive_sign=false) const;

    blitz::Range operator [](int i) const {return range_[i];}
    blitz::Range& operator [](int i) {return range_[i];}
};

}

std::ostream& operator<< (std::ostream& stream, const h5::Select& select);

h5::Expression operator+(const h5::Select  &s1, const h5::Select  &s2); 
h5::Expression operator+(const h5::Expression &e1, const h5::Select  &s2);
h5::Expression operator+(const h5::Select  &s1, const h5::Expression &e2); 

h5::Expression operator-(const h5::Select  &s1, const h5::Select  &s2); 
h5::Expression operator-(const h5::Expression &e1, const h5::Select  &s2); 
h5::Expression operator-(const h5::Select  &s1, const h5::Expression &e2); 

#endif
