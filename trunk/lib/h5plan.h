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
 * \file  h5plan.h
 * @author  A. G. Chatterjee
 * @date feb 2012
 * @bug  No known bugs
 */

#ifndef _H_H5PLAN
#define _H_H5PLAN


#include <vector>
#include "vector_ops.h"
#include "h5expression.h"
#include "hdf5.h"

#ifdef H5SI_ENABLE_MPI

#include <mpi.h>

#endif

namespace h5 {

    class Plan {

        hid_t filespace_;
        hid_t memoryspace_;

        std::vector<int> local_N_;  //number of points that this process will have along each dimension.
        int nD_;
        std::vector<int> my_id_;
        std::vector<int> numprocs_;
        std::vector<hsize_t> memoryspace_dimension_;
        Expression memoryspace_expression_;
        std::vector<hsize_t> filespace_dimension_;
        Expression filespace_expression_;
        hid_t dtype_;

        struct FilterBlock_
        {
            hsize_t start_index;
            hsize_t blocklength;

            FilterBlock_(hsize_t start_index, hsize_t blocklength): start_index(start_index), blocklength(blocklength){}
        };

        struct HyperspaceBlock_{
            blitz::Array<hsize_t, 1> dimension;
            blitz::Array<hsize_t, 1> start;
            blitz::Array<hsize_t, 1> blockdim;
            blitz::Array<hsize_t, 1> blockstride;
            blitz::Array<hsize_t, 1> blockcount;

            HyperspaceBlock_(hsize_t nD);
        };

        bool master();

        void Get_next_start(blitz::Array<int,1> filter, hsize_t begin_at, int my_id, int numprocs, hsize_t &start, hsize_t &blocklength);

        std::vector<HyperspaceBlock_> Get_intersections_memoryspace(int nD, int* my_id, int* numprocs, blitz::Array<int,1>* memoryspace_filter);

        std::vector<HyperspaceBlock_> Get_intersections_filespace(int nD, int* my_id, int* numprocs, blitz::Array<int,1>* filespace_filter);

        void Modify_memoryspace(int nD, int *my_id, int *numprocs, hsize_t *memoryspace_dimension, Select select);

        void Modify_filespace(int nD, int *my_id, int *numprocs, hsize_t *filespace_dimension, Select select, hsize_t *my_start_index_filespace, hsize_t *my_end_index_filespace);

        template<typename T, int size>
        bool isTinyVectorEqual(blitz::TinyVector<T, size> v1, blitz::TinyVector<T, size> v2) {
            for (int i=0; i<size; i++)
                if (v1[i] != v2[i])
                    return false;

            return true;
        }



    public:
        Plan() {}

        void set_plan(std::vector<hsize_t> memoryspace_dimension, Expression memoryspace_expression, std::vector<hsize_t> filespace_dimension, Expression filespace_expression, hid_t dtype=0);

        void set_plan(std::vector<int> my_id, std::vector<int> numprocs,
                      std::vector<hsize_t> memoryspace_dimension,
                      Expression memoryspace_expression,
                      std::vector<hsize_t> filespace_dimension,
                      Expression filespace_expression, hid_t dtype=0);

#ifdef H5SI_ENABLE_MPI
        void set_plan(MPI_Comm MPI_COMMUNICATOR, std::vector<hsize_t> memoryspace_dimension, Expression memoryspace_expression, std::vector<hsize_t> filespace_dimension, Expression filespace_expression, hid_t dtype=0);


        template<int nD>
        void set_plan(MPI_Comm MPI_COMMUNICATOR, blitz::TinyVector<hsize_t, nD> memoryspace_dimension, Expression memoryspace_expression, blitz::TinyVector<hsize_t, nD> filespace_dimension, Expression filespace_expression, hid_t dtype=0) {

            std::vector<int> my_id(memoryspace_dimension.size());
            std::vector<int> numprocs(memoryspace_dimension.size());

            VecOps::assign(my_id, 0);
            VecOps::assign(numprocs, 1);

            MPI_Comm_rank(MPI_COMM_WORLD, &my_id[0]);
            MPI_Comm_size(MPI_COMM_WORLD, &numprocs[0]);

            set_plan(my_id, numprocs, VecOps::to_vector(memoryspace_dimension), memoryspace_expression, VecOps::to_vector(filespace_dimension), filespace_expression, dtype);
        }
#endif

        void set_plan(int rank, int* my_id, int* numprocs, blitz::Array<int,1>* dataspace_filter, blitz::Array<int,1>* memspace_filter, hid_t dtype);

        template<int nD>
        void set_plan(blitz::TinyVector<int, nD> my_id, blitz::TinyVector<int, nD> numprocs, blitz::TinyVector<hsize_t, nD> memoryspace_dimension,
                      Expression memoryspace_expression, blitz::TinyVector<hsize_t, nD> filespace_dimension, Expression filespace_expression, hid_t dtype=0) {

            set_plan(VecOps::to_vector(my_id), VecOps::to_vector(numprocs), VecOps::to_vector(memoryspace_dimension), memoryspace_expression, VecOps::to_vector(filespace_dimension), filespace_expression, dtype);
        }


        hid_t filespace() const;
        hid_t memoryspace() const;

        std::vector<int> my_id() const;
        std::vector<int> numprocs() const;
        std::vector<hsize_t> memoryspace_dimension() const;
        Expression memoryspace_expression() const;
        std::vector<hsize_t> filespace_dimension() const;
        Expression filespace_expression() const;
        hid_t dtype() const;
    };
}
#endif
