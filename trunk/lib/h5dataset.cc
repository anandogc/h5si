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
 * \file  h5dataset.cc
 * @author  A. G. Chatterjee
 * @date feb 2012
 * @bug  No known bugs
 */

#include "h5dataset.h"

#include "h5group.h"

#include "h5datatype.h"

namespace h5 {

    //Implement
    //Dataset << Plan << A << Plan << B;
    //
    //Make the Constructors consistent
    //End result of constructors must be the same except the default constructor.
    //
    // &operator<< should use filespace_dtype_

    Dataset::Dataset():id_(-1), filespace_dtype_(-1), parent_(NULL) { }

    //Copy exesting dataset object
    Dataset::Dataset(const Dataset& ds):name_(ds.name_), shape_(ds.shape_), filespace_dtype_(ds.filespace_dtype_), parent_(ds.parent_), driver_(ds.driver_), plan(ds.plan) {

#ifdef H5SI_ENABLE_MPI
        this->MPI_COMMUNICATOR = this->parent_->MPI_COMMUNICATOR;
#endif

        this->id_ = H5Dopen2(this->parent_->id(), this->name_.c_str(), H5P_DEFAULT);

        // this->parent_->register_node(*this);
    }

    //Open an existing dataset from file
    Dataset::Dataset(Group *parent, std::string name) {
        this->id_ = H5Dopen2(parent->id(), name.c_str(), H5P_DEFAULT);

        this->name_ = parent->name() + "/" + name;

        hid_t space = H5Dget_space(this->id_);
        int nD = H5Sget_simple_extent_dims (space, NULL, NULL);
        this->shape_.resize(nD);
        H5Sget_simple_extent_dims (space, shape_.data(), NULL);

        this->filespace_dtype_ = H5Dget_type(this->id_);

        this->parent_ = parent;

        this->driver_ = this->parent_->driver();

#ifdef H5SI_ENABLE_MPI
        this->MPI_COMMUNICATOR = this->parent_->MPI_COMMUNICATOR;

        if (this->driver_ == "mpio")
            this->plan.set_plan(this->MPI_COMMUNICATOR, this->shape_, Select::all(nD), this->shape_, Select::all(nD));  
        else
            this->plan.set_plan(this->shape_, Select::all(nD), this->shape_, Select::all(nD));      
#else
        this->plan.set_plan(this->shape_, Select::all(nD), this->shape_, Select::all(nD));      
#endif

        // this->parent_->register_node(*this);
        
    }

    //Create a new dataset in file
    Dataset::Dataset(Group *parent, std::string name, std::vector<hsize_t> shape, std::string filespace_dtype) {
        this->name_ = parent->name() + "/" + name;

        this->shape_ = shape;

        this->filespace_dtype_ = Dtype(filespace_dtype);

        this->parent_ = parent;

        this->driver_ = this->parent_->driver();

        int nD = shape.size();
#ifdef H5SI_ENABLE_MPI
        this->MPI_COMMUNICATOR = this->parent_->MPI_COMMUNICATOR;


        if (this->driver_ == "mpio") {
            this->plan.set_plan(this->MPI_COMMUNICATOR, this->shape_, Select::all(nD), this->shape_, Select::all(nD));  
        }
        else
            this->plan.set_plan(this->shape_, Select::all(nD), this->shape_, Select::all(nD));      
#else
        this->plan.set_plan(this->shape_, Select::all(nD), this->shape_, Select::all(nD));      
#endif

        this->id_ = H5Dcreate2(this->parent_->id(), name.c_str(), Dtype(filespace_dtype), plan.filespace(), H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

        // this->parent_->register_node(*this);
    }

    //Create a new dataset according to the given plan in file
    Dataset::Dataset(Group *parent, std::string name, Plan plan, std::string filespace_dtype) {
        if (filespace_dtype.length() == 0)
            this->filespace_dtype_ = plan.dtype();
        else
            this->filespace_dtype_ = Dtype(filespace_dtype);

        this->id_ = H5Dcreate2(parent->id(), name.c_str(), this->filespace_dtype_, plan.filespace(), H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

        if (this->id_ > 0) {
            this->name_ = parent->name() + "/" + name;

            hid_t space = H5Dget_space(this->id_);
            int nD = H5Sget_simple_extent_dims (space, NULL, NULL);
            this->shape_.resize(nD);
            H5Sget_simple_extent_dims (space, shape_.data(), NULL);

            // this->parent_->register_node(*this);
        }

        this->plan = plan;

        this->parent_ = parent;

        this->driver_ = this->parent_->driver();
#ifdef H5SI_ENABLE_MPI
        this->MPI_COMMUNICATOR = this->parent_->MPI_COMMUNICATOR;
#endif
    }

    void Dataset::create() {
        this->id_ = H5Dcreate2(this->parent_->id(), this->name_.c_str(), this->filespace_dtype_, this->plan.filespace(), H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    }

    Dataset::~Dataset() {
        this->close();
    }

    void Dataset::close() {
        if (this->id_ > 0)
            H5Dclose(this->id_);
    }

    const Dataset &Dataset::operator=(const Dataset &dataset) {
        if (this->id_ > 0)
            H5Dclose(this->id_);

        this->name_ = dataset.name_;

        this->parent_ = dataset.parent_;

        this->driver_ = this->parent_->driver();

        this->id_ = H5Dopen2(this->parent_->id(), this->name_.c_str(), H5P_DEFAULT);

        this->shape_ = dataset.shape_;

        this->filespace_dtype_ = dataset.filespace_dtype_;

#ifdef H5SI_ENABLE_MPI
        this->MPI_COMMUNICATOR = this->parent_->MPI_COMMUNICATOR;
#endif
        this->plan = dataset.plan;
    }

    hid_t Dataset::id() const { 
        return this->id_; 
    }

    hid_t Dataset::dtype() const { 
        return this->filespace_dtype_; 
    }

    std::string Dataset::name() const { 
        return this->name_; 
    }

    Group *Dataset::parent() const { 
        return this->parent_; 
    }

    std::string Dataset::driver() const { 
        return this->driver_; 
    }

    std::vector<hsize_t> Dataset::shape() const { 
        return this->shape_; 
    }


    Dataset &Dataset::set_plan(Plan plan) {
        this->plan = plan; return *this;
    }

    const Dataset &operator>>(const Dataset &ds, void *data) {
        H5Dread(ds.id(), H5Tget_native_type(ds.dtype(), H5T_DIR_ASCEND), ds.plan.memoryspace(), ds.plan.filespace(), H5P_DEFAULT, data);
        return ds;
    }

    const Dataset &operator<<(const Dataset &ds, const void *data) {
        H5Dwrite(ds.id(), H5Tget_native_type(ds.dtype(), H5T_DIR_ASCEND), ds.plan.memoryspace(), ds.plan.filespace(), H5P_DEFAULT, data);
        return ds;
    }

    const Dataset &operator>>(const Dataset &ds, char *data) {
        H5Dread(ds.id(), Dtype("char"), ds.plan.memoryspace(), ds.plan.filespace(), H5P_DEFAULT, data);
        return ds;
    }

    const Dataset &operator<<(const Dataset &ds, const char *data) {
        H5Dwrite(ds.id(), Dtype("char"), ds.plan.memoryspace(), ds.plan.filespace(), H5P_DEFAULT, data);
        return ds;
    }

    const Dataset &operator>>(const Dataset &ds, signed char *data) {
        H5Dread(ds.id(), Dtype("schar"), ds.plan.memoryspace(), ds.plan.filespace(), H5P_DEFAULT, data);
        return ds;
    }

    const Dataset &operator<<(const Dataset &ds, const signed char *data) {
        H5Dwrite(ds.id(), Dtype("schar"), ds.plan.memoryspace(), ds.plan.filespace(), H5P_DEFAULT, data);
        return ds;
    }

    const Dataset &operator>>(const Dataset &ds, unsigned char *data) {
        H5Dread(ds.id(), Dtype("uchar"), ds.plan.memoryspace(), ds.plan.filespace(), H5P_DEFAULT, data);
        return ds;
    }

    const Dataset &operator<<(const Dataset &ds, const unsigned char *data) {
        H5Dwrite(ds.id(), Dtype("uchar"), ds.plan.memoryspace(), ds.plan.filespace(), H5P_DEFAULT, data);
        return ds;
    }

    const Dataset &operator>>(const Dataset &ds, short *data) {
        H5Dread(ds.id(), Dtype("short"), ds.plan.memoryspace(), ds.plan.filespace(), H5P_DEFAULT, data);
        return ds;
    }

    const Dataset &operator<<(const Dataset &ds, const short *data) {
        H5Dwrite(ds.id(), Dtype("short"), ds.plan.memoryspace(), ds.plan.filespace(), H5P_DEFAULT, data);
        return ds;
    }

    const Dataset &operator>>(const Dataset &ds, unsigned short *data) {
        H5Dread(ds.id(), Dtype("ushort"), ds.plan.memoryspace(), ds.plan.filespace(), H5P_DEFAULT, data);
        return ds;

    }
    const Dataset &operator<<(const Dataset &ds, const unsigned short *data) {
        H5Dwrite(ds.id(), Dtype("ushort"), ds.plan.memoryspace(), ds.plan.filespace(), H5P_DEFAULT, data);
        return ds;
    }

    const Dataset &operator>>(const Dataset &ds, int *data) {
        H5Dread(ds.id(), Dtype("int"), ds.plan.memoryspace(), ds.plan.filespace(), H5P_DEFAULT, data);
        return ds;
    }

    const Dataset &operator<<(const Dataset &ds, const int *data) {
        H5Dwrite(ds.id(), Dtype("int"), ds.plan.memoryspace(), ds.plan.filespace(), H5P_DEFAULT, data);
        return ds;
    }

    const Dataset &operator>>(const Dataset &ds, unsigned int *data) {
        H5Dread(ds.id(), Dtype("uint"), ds.plan.memoryspace(), ds.plan.filespace(), H5P_DEFAULT, data);
        return ds;
    }

    const Dataset &operator<<(const Dataset &ds, const unsigned int *data) {
        H5Dwrite(ds.id(), Dtype("uint"), ds.plan.memoryspace(), ds.plan.filespace(), H5P_DEFAULT, data);
        return ds;
    }

    const Dataset &operator>>(const Dataset &ds, long *data) {
        H5Dread(ds.id(), Dtype("long"), ds.plan.memoryspace(), ds.plan.filespace(), H5P_DEFAULT, data);
        return ds;
    }

    const Dataset &operator<<(const Dataset &ds, const long *data) {
        H5Dwrite(ds.id(), Dtype("long"), ds.plan.memoryspace(), ds.plan.filespace(), H5P_DEFAULT, data);
        return ds;
    }

    const Dataset &operator>>(const Dataset &ds, unsigned long *data) {
        H5Dread(ds.id(), Dtype("ulong"), ds.plan.memoryspace(), ds.plan.filespace(), H5P_DEFAULT, data);
        return ds;
    }

    const Dataset &operator<<(const Dataset &ds, const unsigned long *data) {
        H5Dwrite(ds.id(), Dtype("ulong"), ds.plan.memoryspace(), ds.plan.filespace(), H5P_DEFAULT, data);
        return ds;
    }

    const Dataset &operator>>(const Dataset &ds, long long *data) {
        H5Dread(ds.id(), Dtype("llong"), ds.plan.memoryspace(), ds.plan.filespace(), H5P_DEFAULT, data);
        return ds;
    }

    const Dataset &operator<<(const Dataset &ds, const long long *data) {
        H5Dwrite(ds.id(), Dtype("llong"), ds.plan.memoryspace(), ds.plan.filespace(), H5P_DEFAULT, data);
        return ds;
    }

    const Dataset &operator>>(const Dataset &ds, unsigned long long *data) {
        H5Dread(ds.id(), Dtype("ullong"), ds.plan.memoryspace(), ds.plan.filespace(), H5P_DEFAULT, data);
        return ds;
    }

    const Dataset &operator<<(const Dataset &ds, const unsigned long long *data) {
        H5Dwrite(ds.id(), Dtype("ullong"), ds.plan.memoryspace(), ds.plan.filespace(), H5P_DEFAULT, data);
        return ds;
    }

    const Dataset &operator>>(const Dataset &ds, float *data) {
        H5Dread(ds.id(), Dtype("float"), ds.plan.memoryspace(), ds.plan.filespace(), H5P_DEFAULT, data);
        return ds;
    }

    const Dataset &operator<<(const Dataset &ds, const float *data) {
        H5Dwrite(ds.id(), Dtype("float"), ds.plan.memoryspace(), ds.plan.filespace(), H5P_DEFAULT, data);
        return ds;
    }

    const Dataset &operator>>(const Dataset &ds, std::complex<float> *data) {
        H5Dread(ds.id(), Dtype("cfloat"), ds.plan.memoryspace(), ds.plan.filespace(), H5P_DEFAULT, data);
        return ds;
    }

    const Dataset &operator<<(const Dataset &ds, const std::complex<float> *data) {
        H5Dwrite(ds.id(), Dtype("cfloat"), ds.plan.memoryspace(), ds.plan.filespace(), H5P_DEFAULT, data);
        return ds;
    }

    const Dataset &operator>>(const Dataset &ds, double *data) {
        H5Dread(ds.id(), Dtype("double"), ds.plan.memoryspace(), ds.plan.filespace(), H5P_DEFAULT, data);
        return ds;

    }

    const Dataset &operator<<(const Dataset &ds, const std::complex<double> *data) {
        H5Dwrite(ds.id(), Dtype("cdouble"), ds.plan.memoryspace(), ds.plan.filespace(), H5P_DEFAULT, data);
        return ds;
    }

    const Dataset &operator>>(const Dataset &ds, std::complex<double> *data) {
        H5Dread(ds.id(), Dtype("cdouble"), ds.plan.memoryspace(), ds.plan.filespace(), H5P_DEFAULT, data);
        return ds;

    }

    const Dataset &operator<<(const Dataset &ds, const double *data) {
        H5Dwrite(ds.id(), Dtype("double"), ds.plan.memoryspace(), ds.plan.filespace(), H5P_DEFAULT, data);
        return ds;
    }

    const Dataset &operator>>(const Dataset &ds, long double *data) {
        H5Dread(ds.id(), Dtype("ldouble"), ds.plan.memoryspace(), ds.plan.filespace(), H5P_DEFAULT, data);
        return ds;

    }
    const Dataset &operator<<(const Dataset &ds, const long double *data) {
        H5Dwrite(ds.id(), Dtype("ldouble"), ds.plan.memoryspace(), ds.plan.filespace(), H5P_DEFAULT, data);
        return ds;
    }

}
