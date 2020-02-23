# H5SI
#
# Copyright (C) 2020, Mahendra K. Verma, Anando Gopal Chatterjee
#
# Mahendra K. Verma
# Indian Institute of Technology, Kanpur-208016
# UP, India
#
# mkv@iitk.ac.in
#
# This file is part of H5SI.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its contributors
#    may be used to endorse or promote products derived from this software without
#    specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
# OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
# OF THE POSSIBILITY OF SUCH DAMAGE.
#
# \file  CMakeLists.txt
# @author  A. G. Chatterjee
# @date feb 2012
# @bug  No known bugs

ADD_LIBRARY(h5si
            h5dataset
            h5datatype
            h5expression
            h5file
            h5group
            h5node
            h5plan
            h5select
            h5shape
            h5si
            vector_ops
)

INSTALL(TARGETS h5si
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

FILE(GLOB headers "${CMAKE_SOURCE_DIR}/lib/*.h")

INSTALL(FILES ${headers}
        DESTINATION include)


#configure_file("pkgconfig/fftk.pc.in" "pkgconfig/fftk${LIB_SUFFIX}.pc" @ONLY)
#install(FILES ${PROJECT_BINARY_DIR}/src/pkgconfig/fftk${LIB_SUFFIX}.pc
#        DESTINATION lib/pkgconfig)
