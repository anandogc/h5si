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
 * \file  h5select.cc
 * @author  A. G. Chatterjee
 * @date feb 2012
 * @bug  No known bugs
 */

#include "h5select.h"

namespace h5 {

//////////////////////////
//Parse indices and return an equivalent blitz::Range object
//e.g.
//"1"      -> Range(1)         # index 1
//"0:10"   -> Range(0,10)      # Range 0 to 10, inclusive of both ends
//"0:10:2" -> Range(0,10,2)    # Range 0 to 10, in steps of 2 inclusive of both ends
///////////////////////////
blitz::Range Select::Parse_indices(std::string indices) {

    int colon_count = std::count(indices.begin(), indices.end(), ':');

    int first_colon_index=indices.find(':');
    int second_colon_index=indices.find(':', first_colon_index+1);

    blitz::Range range;

    if (colon_count==0)
        range = blitz::Range(atoi(indices.c_str()));
    else if (colon_count==1) {
        if (indices == ":")
            range = blitz::Range(blitz::Range::all());
        else {
            int start = atoi(indices.substr(0,first_colon_index).c_str());
            int stop = atoi(indices.substr(first_colon_index+1).c_str());
            range = blitz::Range(start, stop);
        }
    }
    else if (colon_count==2) {
        if  (indices == "::")
            range = blitz::Range(blitz::Range::all());
        else {
            int start = atoi(indices.substr(0,first_colon_index).c_str());
            int stop = atoi(indices.substr(first_colon_index+1, second_colon_index - first_colon_index-1).c_str());
            int step = atoi(indices.substr(second_colon_index+1).c_str());

            range = blitz::Range(start, stop, step);
        }
    }

    return range;
}

//////////////////////////
//Parse ranges and return an equivalent Select object
//e.g.
// "[0:10:2, 4:20, 5]" -> Select('+',Range(0,10,2),Range(4,20),Range(5))
// "+[0:10:2, 4:20, 5]" -> Select('+',Range(0,10,2),Range(4,20),Range(5))
// "-[0:10:2, 4:20, 5]" -> Select('-',Range(0,10,2),Range(4,20),Range(5))
//////////////////////////
Select::Select(std::string select_str) {

    //remove all spaces
    select_str.erase(std::remove(select_str.begin(), select_str.end(), ' '), select_str.end());
    if (select_str.length()<=0)
        return;

    if (select_str.find_first_of("+-")!=0)
        select_str = "+" + select_str;

    this->sign_ = select_str[0];

    //Check Range is surrounded by a pair of brackets
    if ( select_str[1]!='[' || select_str[select_str.length()-1]!=']' ) {
            // if (myid_==0)
                std::cerr << "Invalid Range: '" << select_str << "'. Each select must be surrounded by a pair of square brackets." << std::endl;
            // MPI_Abort(MPI_COMM_WORLD, 1);
    }

    select_str[select_str.length()-1] = ',';

    int comma_count = std::count(select_str.begin(), select_str.end(), ',');
    int nD = comma_count;

    this->range_.reserve(nD);

    size_t comma_index = 1;
    size_t next_comma_index;


    for (int d=0; d<nD; d++) {
        next_comma_index = select_str.find(",", comma_index+1);
        this->range_.push_back(Parse_indices(select_str.substr(comma_index+1, next_comma_index - comma_index - 1)));
        comma_index = next_comma_index;
    }

/*  //If it has three parts
    if (std::count(select_str.begin(), select_str.end(), ',')==2) {
        int index_of_first_comma = select_str.find_first_of(',',1);
        int index_of_second_comma = select_str.find_first_of(',', index_of_first_comma+1);

        //Range along X-axis
        std::string range_x_str = select_str.substr(2,index_of_first_comma-2);
        this->range[0] = Parse_indices(range_x_str);

        //Range along Y-axis
        std::string range_y_str = select_str.substr(index_of_first_comma+1,index_of_second_comma - index_of_first_comma-1);
        this->range[1] = Parse_indices(range_y_str);

        //Range along Z-axis
        std::string range_z_str = select_str.substr(index_of_second_comma+1, select_str.length()-index_of_second_comma-2);
        this->range[2] = Parse_indices(range_z_str);

    }
    else {
        // if (myid_==0)
            std::cerr << "Range '" << select_str << "' must have three parts separated by comma." << std::endl;
        // MPI_Abort(MPI_COMM_WORLD, 1);
    }*/
}

Select Select::all(int nD) {
    return Select(blitz::Range::all(), blitz::Range::all(), blitz::Range::all());
}


bool isRangeEqual(blitz::Range ra1, blitz::Range ra2) {
    if ( ( ra1.first(blitz::fromStart) == ra2.first(blitz::fromStart) ) && ( ra1.last(blitz::toEnd) == ra2.last(blitz::toEnd) ) && ( ra1.stride() == ra2.stride() ) )
        return true;

    return false;
}


void Select::Pretty_print(std::string &str, bool print_positive_sign) const {

    std::ostringstream oss;
    blitz::Range all = blitz::Range::all();
    
    if ( (this->sign_ == '+' && print_positive_sign) || (this->sign_ == '-') )
        oss << this->sign_ + std::string(" ");

    oss << "[";

    for (std::vector<blitz::Range>::size_type i=0; i<this->range_.size(); i++) {
        if(i != 0)
            oss << ",";

        if (isRangeEqual(this->range_[i], all)) {
            oss << ":";
        }
        else {
            oss << this->range_[i].first(blitz::fromStart);

            if (this->range_[i].last() != this->range_[i].first(blitz::fromStart))
                oss << ":" << this->range_[i].last(blitz::toEnd);

            if (this->range_[i].stride() != 1)
                oss << ":" << this->range_[i].stride();
        }

    }
    oss << "]";

    str += oss.str();

}

}

std::ostream& operator<< (std::ostream& stream, const h5::Select& select) {
    std::string select_str;
    select.Pretty_print(select_str, true);
    stream << select_str;

    return stream;
}
