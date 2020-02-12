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
 * \file  h5datatype.cc
 * @author  A. G. Chatterjee
 * @date feb 2012
 * @bug  No known bugs
 */

#include "h5datatype.h"

#include <iostream>

namespace h5 {

    std::map<std::string, hid_t> Dtype::dtype_;
    hid_t Dtype::compound_type_native_complex_float_;
    hid_t Dtype::compound_type_native_complex_double_;

    Dtype::Dtype(std::string dtype_str) {
        this->selected_dtype_str_ = dtype_str;
    }

    void Dtype::finalize() {
        H5Tclose(compound_type_native_complex_float_);
        H5Tclose(compound_type_native_complex_double_);
    }

    void Dtype::init() {
        compound_type_native_complex_float_ = H5Tcreate(H5T_COMPOUND, 2*H5Tget_size(H5T_NATIVE_FLOAT));
        H5Tinsert(compound_type_native_complex_float_, "real", 0, H5T_NATIVE_FLOAT);
        H5Tinsert(compound_type_native_complex_float_, "imag", H5Tget_size(H5T_NATIVE_FLOAT), H5T_NATIVE_FLOAT);
        // H5Tlock(compound_type_native_complex_float_);

        compound_type_native_complex_double_ = H5Tcreate(H5T_COMPOUND, 2*H5Tget_size(H5T_NATIVE_DOUBLE));
        H5Tinsert(compound_type_native_complex_double_, "real", 0, H5T_NATIVE_DOUBLE);
        H5Tinsert(compound_type_native_complex_double_, "imag", H5Tget_size(H5T_NATIVE_DOUBLE), H5T_NATIVE_DOUBLE);
        // H5Tlock(compound_type_native_complex_double_);

        dtype_[">f32"] =  H5T_IEEE_F32BE;
        dtype_["<f32"] =  H5T_IEEE_F32LE;
        dtype_[">f64"] =  H5T_IEEE_F64BE;
        dtype_["<f64"] =  H5T_IEEE_F64LE;

        dtype_[">i8"] = H5T_STD_I8BE;
        dtype_["<i8"] = H5T_STD_I8LE;
        dtype_[">i16"] =  H5T_STD_I16BE;
        dtype_["<i16"] =  H5T_STD_I16LE;
        dtype_[">i32"] =  H5T_STD_I32BE;
        dtype_["<i32"] =  H5T_STD_I32LE;
        dtype_[">i64"] =  H5T_STD_I64BE;
        dtype_["<i64"] =  H5T_STD_I64LE;
        dtype_[">u8"] = H5T_STD_U8BE;
        dtype_["<u8"] = H5T_STD_U8LE;
        dtype_[">u16"] =  H5T_STD_U16BE;
        dtype_["<u16"] =  H5T_STD_U16LE;
        dtype_[">u32"] =  H5T_STD_U32BE;
        dtype_["<u32"] =  H5T_STD_U32LE;
        dtype_[">u64"] =  H5T_STD_U64BE;
        dtype_[">b8"] = H5T_STD_B8BE;
        dtype_["<b8"] = H5T_STD_B8LE;
        dtype_[">b16"] =  H5T_STD_B16BE;
        dtype_["<b16"] =  H5T_STD_B16LE;
        dtype_[">b32"] =  H5T_STD_B32BE;
        dtype_["<b32"] =  H5T_STD_B32LE;
        dtype_[">b64"] =  H5T_STD_B64BE;
        dtype_["<b64"] =  H5T_STD_B64LE;

        dtype_["S"] = H5T_C_S1;
        dtype_["a"] = H5T_C_S1;

        dtype_["char"] =  H5T_NATIVE_CHAR;
        dtype_["schar"] =  H5T_NATIVE_SCHAR;
        dtype_["uchar"] =  H5T_NATIVE_UCHAR;
        dtype_["short"] =  H5T_NATIVE_SHORT;
        dtype_["ushort"] =  H5T_NATIVE_USHORT;
        dtype_["int"] = H5T_NATIVE_INT;
        dtype_["uint"] =  H5T_NATIVE_UINT;
        dtype_["long"] =  H5T_NATIVE_LONG;
        dtype_["ulong"] =  H5T_NATIVE_ULONG;
        dtype_["llong"] =  H5T_NATIVE_LLONG;
        dtype_["ullong"] =  H5T_NATIVE_ULLONG;
        dtype_["float"] =  H5T_NATIVE_FLOAT;
        dtype_["double"] =  H5T_NATIVE_DOUBLE;
        dtype_["ldouble"] =  H5T_NATIVE_LDOUBLE;
        dtype_["b8"] = H5T_NATIVE_B8;
        dtype_["b16"] = H5T_NATIVE_B16;
        dtype_["b32"] = H5T_NATIVE_B32;
        dtype_["b64"] = H5T_NATIVE_B64;
        dtype_["opaque"] =  H5T_NATIVE_OPAQUE;
        dtype_["haddr"] =  H5T_NATIVE_HADDR;
        dtype_["hsize"] =  H5T_NATIVE_HSIZE;
        dtype_["hssize"] =  H5T_NATIVE_HSSIZE;
        dtype_["herr"] =  H5T_NATIVE_HERR;
        dtype_["hbool"] =  H5T_NATIVE_HBOOL;

        dtype_["cfloat"] =  compound_type_native_complex_float_;
        dtype_["cdouble"] =  compound_type_native_complex_double_;
    }
}
