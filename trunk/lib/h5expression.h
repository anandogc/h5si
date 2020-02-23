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
 * \file  h5expression.h
 * @author  A. G. Chatterjee
 * @date feb 2012
 * @bug  No known bugs
 */

#ifndef _H_H5EXPRESSION
#define _H_H5EXPRESSION


#include <vector>
#include "h5select.h"

namespace h5 {

class Expression {
    public:
    std::vector<Select> expression;

    typedef std::vector<Select>::size_type size_type;

    Expression() {};
    Expression(Select select) {this->expression.push_back(select);}
    Expression(std::string expression_str);
    
    void Add_select(const Select &select);
    void Subtract_select(const Select &select);

    void Insert(const Select &select, size_t index=-1);
    void Remove(size_t index);

    size_t size() const { return expression.size(); }

    bool isEmpty() const{ return (expression.size()==0); }

    void Pretty_print(std::string &str, bool print_first_positive=false) const;

    Expression& operator=(const Select &select);

    Select& operator [](int i) { return expression[i]; }
};

}

std::ostream& operator<< (std::ostream& stream, const h5::Expression& expression);
#endif
