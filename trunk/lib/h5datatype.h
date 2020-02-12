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
 * \file  h5datatype.h
 * @author  A. G. Chatterjee
 * @date feb 2012
 * @bug  No known bugs
 */

#include <map>
#include <string>
#include <hdf5.h>


#ifndef _H_H5DTYPE
#define _H_H5DTYPE


/**
 * addind new datatype:
 * To add a basic datatype
 *       1) Open h5datatype.cc
 *       2) add a line like dtype_["..."] = ... in Dtype::Dtype correspondint to it
 *
 * To add a compound datatype
 *       1) Open h5datatype.cc
 *       2) define it on top of Dtype::Dtype
 *       3) add a line like dtype_["..."] = ... correspondint to it
 *
 */
namespace h5 {
    class Dtype {

        static std::map<std::string, hid_t> dtype_;

        static hid_t compound_type_native_complex_float_;
        static hid_t compound_type_native_complex_double_;

        static unsigned int reference_count_;    /**<  Keeps a count of number of instances of this class.
                                                       Last instance closes compound types. */

        std::string selected_dtype_str_;


    public:
        Dtype(std::string dtype_str);

        static void init();
        static void finalize();

        operator hid_t() const { return this->dtype_[this->selected_dtype_str_]; }
    };
}

#endif
