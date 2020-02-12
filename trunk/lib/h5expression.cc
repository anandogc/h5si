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
 * \file  h5expression.cc
 * @author  A. G. Chatterjee
 * @date feb 2012
 * @bug  No known bugs
 */

#include "h5expression.h"

namespace h5 {

    Expression::Expression(std::string expression_str) {
        std::string select_str;

        size_t current_op_index=0;
        size_t next_op_index=0;

        expression_str.erase(std::remove(expression_str.begin(), expression_str.end(), ' '), expression_str.end());
        if (expression_str.length()<=0)
            return;

        if (expression_str.find_first_of("+-")!=0)
            expression_str = "+" + expression_str;

        current_op_index = expression_str.find_first_of("+-", 0);

        //extract each range with their signs.
        while (next_op_index != std::string::npos) {
            next_op_index = expression_str.find_first_of("+-", current_op_index+1);

            if (next_op_index!=std::string::npos) {
                select_str = expression_str.substr(current_op_index, next_op_index-current_op_index);
            }
            else {
                select_str = expression_str.substr(current_op_index);
            }

            current_op_index = next_op_index;

            this->expression.push_back(Select(select_str));
        }
    }


    void Expression::Add_select(const Select  &select) {
        this->expression.push_back(select);
    }

    void Expression::Subtract_select(const Select  &select) {
        Select select_neg = select;
        select_neg.Negate();

        this->expression.push_back(select_neg);
    }

    void Expression::Insert(const Select &select, size_t index) {
        if (index < 0)
            this->expression.push_back(select);
        else
            this->expression.insert(this->expression.begin()+index, select);
    }

    void Expression::Remove(size_t index) {
        if (index >= 0)
            this->expression.erase(this->expression.begin()+index);
    }


    void Expression::Pretty_print(std::string &str, bool print_first_positive) const {
        std::ostringstream oss;

        std::string select_str;

        for (size_type i=0; i<expression.size(); i++) {
            if(i != 0)
                oss << " ";
            
            select_str="";
            if(i != 0)
                expression[i].Pretty_print(select_str, true);
            else
                expression[i].Pretty_print(select_str, print_first_positive);

            oss << select_str;
        }

        str += oss.str();
    }


    Expression& Expression::operator=(const Select &select)
    {
        this->expression.erase(this->expression.begin(), this->expression.end());
        this->expression.push_back(select);
        return *this;
    }

}

std::ostream& operator<< (std::ostream& stream, const h5::Expression& expression) {
    std::string select_str;
    expression.Pretty_print(select_str, true);
    stream << select_str;

    return stream;
}


h5::Expression operator+(const h5::Select &s1, const h5::Select &s2) {
    h5::Expression expression;

    expression.Add_select(s1);
    expression.Add_select(s2);

    return expression;
}

h5::Expression operator+(const h5::Expression &e1, const h5::Select  &s2) {
    h5::Expression expression = e1;

    expression.Add_select(s2);

    return expression;
}

h5::Expression operator+(const h5::Select  &s1, const h5::Expression &e2) {
    h5::Expression expression = e2;

    expression.Insert(s1, 0);

    return expression;
}


h5::Expression operator-(const h5::Select &s1, const h5::Select &s2) {
    h5::Expression expression;

    expression.Add_select(s1);
    expression.Subtract_select(s2);

    return expression;
}

h5::Expression operator-(const h5::Expression &e1, const h5::Select  &s2) {
    h5::Expression expression = e1;

    expression.Subtract_select(s2);

    return expression;
}

h5::Expression operator-(const h5::Select  &s1, const h5::Expression &e2) {
    h5::Expression expression = e2;

    expression.Insert(s1, 0);
    for (h5::Expression::size_type i=1; i<expression.expression.size(); i++)
        expression.expression[i].Negate();

    return expression;
}
