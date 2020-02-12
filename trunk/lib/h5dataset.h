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
 * \file  h5dataset.h
 * @author  A. G. Chatterjee
 * @date feb 2012
 * @bug  No known bugs
 */

#ifndef _H_H5DATASET
#define _H_H5DATASET

#include <hdf5.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include <map>

#include <complex>

#include "h5plan.h"
#include "h5datatype.h"

namespace h5 {

    class Group;

    class Dataset
    {

    protected:
        hid_t id_;
        std::string name_;

        std::vector<hsize_t> shape_;

        hid_t filespace_dtype_;
        
        Group *parent_;

        std::string driver_;

    public:

#ifdef H5SI_ENABLE_MPI
        MPI_Comm MPI_COMMUNICATOR;
#endif
        Plan plan;

        Dataset();
        Dataset(const Dataset& ds);
        Dataset(Group *parent, std::string name);
        Dataset(Group *parent, std::string name, std::vector<hsize_t> shape, std::string data_type);
        Dataset(Group *parent, std::string name, Plan plan, std::string data_type="");

        ~Dataset();

        void close();

        void create();

        void mpiioInit();

        hid_t id() const;
        hid_t dtype() const;
        std::string name() const;
        Group *parent() const;
        std::string driver() const;
        std::vector<hsize_t> shape() const;

        Dataset &set_plan(Plan plan);

        const Dataset &operator=(const Dataset &dataset);
    };

    const Dataset &operator>>(const Dataset &ds, void *data);
    const Dataset &operator<<(const Dataset &ds, const void *data);
    const Dataset &operator>>(const Dataset &ds, char *data);
    const Dataset &operator<<(const Dataset &ds, const char *data);
    const Dataset &operator>>(const Dataset &ds, signed char *data);
    const Dataset &operator<<(const Dataset &ds, const signed char *data);
    const Dataset &operator>>(const Dataset &ds, unsigned char *data);
    const Dataset &operator<<(const Dataset &ds, const unsigned char *data);
    const Dataset &operator>>(const Dataset &ds, short *data);
    const Dataset &operator<<(const Dataset &ds, const short *data);
    const Dataset &operator>>(const Dataset &ds, unsigned short *data);
    const Dataset &operator<<(const Dataset &ds, const unsigned short *data);
    const Dataset &operator>>(const Dataset &ds, int *data);
    const Dataset &operator<<(const Dataset &ds, const int *data);
    const Dataset &operator>>(const Dataset &ds, unsigned int *data);
    const Dataset &operator<<(const Dataset &ds, const unsigned int *data);
    const Dataset &operator>>(const Dataset &ds, long *data);
    const Dataset &operator<<(const Dataset &ds, const long *data);
    const Dataset &operator>>(const Dataset &ds, unsigned long *data);
    const Dataset &operator<<(const Dataset &ds, const unsigned long *data);
    const Dataset &operator>>(const Dataset &ds, long long *data);
    const Dataset &operator<<(const Dataset &ds, const long long *data);
    const Dataset &operator>>(const Dataset &ds, unsigned long long *data);
    const Dataset &operator<<(const Dataset &ds, const unsigned long long *data);
    const Dataset &operator>>(const Dataset &ds, float *data);
    const Dataset &operator<<(const Dataset &ds, const float *data);
    const Dataset &operator>>(const Dataset &ds, std::complex<float> *data);
    const Dataset &operator<<(const Dataset &ds, const std::complex<float> *data);
    const Dataset &operator>>(const Dataset &ds, double *data);
    const Dataset &operator<<(const Dataset &ds, const double *data);
    const Dataset &operator>>(const Dataset &ds, std::complex<double> *data);
    const Dataset &operator<<(const Dataset &ds, const std::complex<double> *data);
    const Dataset &operator>>(const Dataset &ds, long double *data);
    const Dataset &operator<<(const Dataset &ds, const long double *data);

}


#endif
