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

#include "h5plan.h"
#include "vector_ops.h"

namespace h5 {

    Plan::HyperspaceBlock_::HyperspaceBlock_(hsize_t rank){
        dimension.resize(rank);
        start.resize(rank);
        blockdim.resize(rank);
        blockstride.resize(rank);
        blockcount.resize(rank);

        dimension=0;
        start=0;
        blockdim=0;
        blockstride=1;
        blockcount=0;
    }

    bool Plan::master() {
        for (std::vector<int>::size_type i=0; i<this->my_id_.size(); ++i)
            if (this->my_id_[i] != 0)
                return false;
        return true;
    }

    //Get the start index and length of a block in memoryspace_, search starts at 'begin_at'
    //e.g.
    //Filter: 0001100111000
    //Input: begin_at=0 (start search from index 0)
    //Caller sets all non-local indices to zero
    //Output:
    //start=3
    //blocklength=2
    void Plan::Get_next_start(blitz::Array<int,1> filter, hsize_t begin_at, int my_id, int numprocs, hsize_t &start, hsize_t &blocklength)
    {
        hsize_t end = -1;

        start = begin_at + first(filter(blitz::Range(begin_at, filter.size()))==1);

        if (start>=0 && start<filter.size())
            end = start + first(filter(blitz::Range(start, filter.size()))==0);

        if (start<0 || start>=filter.size()) {      // start is invalid, i.e. not within local index range
            blocklength = 0;
            start = 0;
            return;
        }

        if (end<0 || end>=filter.size()) {      // No zeros were found after start
            blocklength = filter.size() - start;
            return;
        }
    
        // start and end both are valid
        blocklength = end - start;
    }



    std::vector<Plan::HyperspaceBlock_> Plan::Get_intersections_memoryspace(int rank, int* my_id, int* numprocs, blitz::Array<int,1>* memoryspace_filter) {

        std::vector<HyperspaceBlock_> memoryspace_blocks;
        hsize_t start_index, blocklength;
        hsize_t begin_at;

        std::vector<FilterBlock_> memoryspace_filter_blocks[rank];


        for (int r=0; r<rank; r++) {
            begin_at=0;

            do {
                Get_next_start(memoryspace_filter[r], begin_at, my_id[r], numprocs[r], start_index, blocklength);
                begin_at = start_index + blocklength;

                if (begin_at>0)
                    memoryspace_filter_blocks[r].push_back(FilterBlock_(start_index-my_id[r]*memoryspace_filter[r].size()/numprocs[r], blocklength));
            }
            while(begin_at>0);
        }

        hsize_t memoryspace_num_combinations=1;
        for (int r=0; r<rank; r++)
            memoryspace_num_combinations*=memoryspace_filter_blocks[r].size();

        memoryspace_blocks.reserve(memoryspace_num_combinations);

        for (hsize_t i=0; i<memoryspace_num_combinations; i++)
            memoryspace_blocks.push_back(HyperspaceBlock_(rank));


        if (memoryspace_num_combinations>0) {
            //Perform all the combination, works for any rank
            hsize_t repeat_rth_rank=memoryspace_num_combinations;
            hsize_t index;

            for (int r=0; r<rank; r++) {
                index=-1;
                repeat_rth_rank = repeat_rth_rank/memoryspace_filter_blocks[r].size();

                for (hsize_t j=0; j<memoryspace_num_combinations;) {
                    index = (index+1)%memoryspace_filter_blocks[r].size();

                    for (hsize_t k=0; k<repeat_rth_rank; j++, k++){
                        memoryspace_blocks[j].dimension(r)=memoryspace_filter[r].size()/numprocs[r];
                        memoryspace_blocks[j].start(r)=memoryspace_filter_blocks[r][index].start_index;
                        memoryspace_blocks[j].blockstride(r)=1;
                        memoryspace_blocks[j].blockcount(r)=1;
                        memoryspace_blocks[j].blockdim(r)=memoryspace_filter_blocks[r][index].blocklength;
                    }
                }
            }
        }

        return memoryspace_blocks;
    }

    std::vector<Plan::HyperspaceBlock_> Plan::Get_intersections_filespace(int nD, int* my_id, int* numprocs, blitz::Array<int,1>* filespace_filter) {

        std::vector<HyperspaceBlock_> filespace_blocks;

        hsize_t start_index, blocklength;
        int begin_at;

        std::vector<FilterBlock_> filespace_filter_blocks[nD];

        for (int d=0; d<nD; d++) {
            begin_at=0;
        
            do {
                Get_next_start(filespace_filter[d], begin_at, my_id[d], numprocs[d], start_index, blocklength);
                begin_at = start_index + blocklength;

                if (begin_at>0)
                    filespace_filter_blocks[d].push_back(FilterBlock_(start_index, blocklength));
            }
            while(begin_at>0);
        }

        hsize_t filespace_num_combinations=1;
        for (int d=0; d<nD; d++)
            filespace_num_combinations*=filespace_filter_blocks[d].size();

        filespace_blocks.reserve(filespace_num_combinations);
        for (hsize_t i=0; i<filespace_num_combinations; i++)
            filespace_blocks.push_back(HyperspaceBlock_(nD));

        if (filespace_num_combinations>0) {
            //Perform all the combination, works for any nD
            hsize_t repeat_rth_rank=filespace_num_combinations;
            hsize_t index;

            for (int d=0; d<nD; d++) {
                index=-1;
                repeat_rth_rank = repeat_rth_rank/filespace_filter_blocks[d].size();

                for (hsize_t j=0; j<filespace_num_combinations;) {
                    index = (index+1)%filespace_filter_blocks[d].size();

                    for (hsize_t k=0; k<repeat_rth_rank; j++, k++){
                        filespace_blocks[j].dimension(d)=filespace_filter[d].size();
                        filespace_blocks[j].start(d)=filespace_filter_blocks[d][index].start_index;
                        filespace_blocks[j].blockstride(d)=1;
                        filespace_blocks[j].blockcount(d)=1;
                        filespace_blocks[j].blockdim(d)=filespace_filter_blocks[d][index].blocklength;
                    }
                }
            }
        }

        return filespace_blocks;
    }

    //Returns count list
    //[2,3,4] -> 1st proc has 2 elems in select, 2nd has 3 and 3rd has 4.
    void Plan::Modify_memoryspace(int nD, int *my_id, int *numprocs, hsize_t *memoryspace_dimension, Select select) {

        blitz::Array<int,1> memoryspace_filter[nD];
        std::vector<HyperspaceBlock_> memoryspace_blocks;

        for (int d=0; d<nD; d++) {
            memoryspace_filter[d].resize(memoryspace_dimension[d]);
            memoryspace_filter[d]=0;
            memoryspace_filter[d](select.get_range()[d])=1;
            memoryspace_filter[d](blitz::Range(0, my_id[d]*memoryspace_dimension[d]/numprocs[d]-1)) = 0;
            memoryspace_filter[d](blitz::Range((my_id[d]+1)*memoryspace_dimension[d]/numprocs[d], blitz::toEnd)) = 0;
        }



        memoryspace_blocks = Get_intersections_memoryspace(nD, my_id, numprocs, memoryspace_filter);

        // for (int d=0; d<nD; d++)
            // memoryspace_dimension[d]=memoryspace_filter[d].size()/numprocs[d];

        H5S_seloper_t H5S_SELECT_OPERATOR;

        switch (select.get_sign()) {
            case '+':
                H5S_SELECT_OPERATOR = H5S_SELECT_OR;
                break;

            case '-':
                H5S_SELECT_OPERATOR = H5S_SELECT_NOTB;
                break;
        }

        //Select required region in memoryspace_blocks
        for (std::vector<HyperspaceBlock_>::size_type i=0; i<memoryspace_blocks.size(); i++){
             /*if (my_id[0]==0){
                std::cerr << "Memspace: " << my_id[0] << " " << select << " " << memoryspace_blocks[i].dimension << " " << memoryspace_blocks[i].start << " " << memoryspace_blocks[i].blockstride << " " << memoryspace_blocks[i].blockcount << " " << memoryspace_blocks[i].blockdim << std::endl;
             }*/
             

             H5Sselect_hyperslab(this->memoryspace_, H5S_SELECT_OPERATOR, memoryspace_blocks[i].start.data(), memoryspace_blocks[i].blockstride.data(), memoryspace_blocks[i].blockcount.data(), memoryspace_blocks[i].blockdim.data());
        }
    }

    //Check that they have proper number of data counts.
    /*bool Plan::Validate_expressions()
    {
            if ( (filespace_expression.size() == 0) or (memoryspace_expression.size() not_eq filespace_expression.size() )
                return false;

            blitz::Array<int,1> memoryspace_filter[nD];
            blitz::Array<int,1> filespace_filter[nD];

            for (int d=0; d<nD; d++) {
                memoryspace_filter[d].resize(this->memoryspace_dimension_[d]);
                memoryspace_filter[d] = 0;
                memoryspace_filter[d](this->memoryspace_expression_[d]) = 1;

                filespace_filter[d].resize(this->filespace_dimension_[d]);
                filespace_filter[d] = 0;
                filespace_filter[d](this->filespace_expression_[d]) = 1;

                for (int p=0; p<this->numprocs_[d]; p++)
                    if (count(memoryspace_filter[d](Range(this->my_id_[d]*))))
                        ;
            }
            
    }*/

    //Returns count list
    //[2,3,4] -> 1st proc has 3 elems, 2nd has 3 and 3rd has 4.
    void Plan::Modify_filespace(int nD, int *my_id, int *numprocs, hsize_t *filespace_dimension, Select select, hsize_t *my_start_index_filespace, hsize_t *my_end_index_filespace) {

        blitz::Array<int,1> filespace_filter[nD];
        std::vector<HyperspaceBlock_> filespace_blocks;

        for (int d=0; d<nD; d++) {
            filespace_filter[d].resize(filespace_dimension[d]);
            filespace_filter[d] = 0;
            filespace_filter[d](select.get_range()[d]) = 1;
            filespace_filter[d](blitz::Range(0, my_start_index_filespace[d]-1)) = 0;
            filespace_filter[d](blitz::Range(my_end_index_filespace[d], blitz::toEnd)) = 0;
        }

        filespace_blocks = Get_intersections_filespace(nD, my_id, numprocs, filespace_filter);

        H5S_seloper_t H5S_SELECT_OPERATOR;
        switch (select.get_sign()) {
            case '+':
                H5S_SELECT_OPERATOR = H5S_SELECT_OR;
                break;

            case '-':
                H5S_SELECT_OPERATOR = H5S_SELECT_NOTB;
                break;
        }

                        
        //Select required region in the filespace_
        for (std::vector<HyperspaceBlock_>::size_type i=0; i<filespace_blocks.size(); i++){
/*              if (my_id[0]==0)
                    std::cerr << "filespace_: " << my_id[0] << " " << filespace_blocks[i].dimension << " " << filespace_blocks[i].start << " " << filespace_blocks[i].blockstride << " " << filespace_blocks[i].blockcount << " " << filespace_blocks[i].blockdim << std::endl;
*/
             H5Sselect_hyperslab(this->filespace_, H5S_SELECT_OPERATOR, filespace_blocks[i].start.data(), filespace_blocks[i].blockstride.data(), filespace_blocks[i].blockcount.data(), filespace_blocks[i].blockdim.data());

        }
    }

    void Plan::set_plan(std::vector<hsize_t> memoryspace_dimension, Expression memoryspace_expression, std::vector<hsize_t> filespace_dimension, Expression filespace_expression, hid_t dtype) {

        std::vector<int> my_id(memoryspace_dimension.size());
        std::vector<int> numprocs(memoryspace_dimension.size());

        VecOps::assign(my_id, 0);
        VecOps::assign(numprocs, 1);

        set_plan(my_id, numprocs, memoryspace_dimension, memoryspace_expression, filespace_dimension, filespace_expression, dtype);
    }

#ifdef H5SI_ENABLE_MPI

    void Plan::set_plan(MPI_Comm MPI_COMMUNICATOR, std::vector<hsize_t> memoryspace_dimension, Expression memoryspace_expression, std::vector<hsize_t> filespace_dimension, Expression filespace_expression, hid_t dtype) {

        std::vector<int> my_id(memoryspace_dimension.size());
        std::vector<int> numprocs(memoryspace_dimension.size());

        VecOps::assign(my_id, 0);
        VecOps::assign(numprocs, 1);

        MPI_Comm_rank(MPI_COMM_WORLD, &my_id[0]);
        MPI_Comm_size(MPI_COMM_WORLD, &numprocs[0]);

        set_plan(my_id, numprocs, memoryspace_dimension, memoryspace_expression, filespace_dimension, filespace_expression, dtype);
    }

#endif

    void Plan::set_plan(std::vector<int> my_id, std::vector<int> numprocs, std::vector<hsize_t> memoryspace_dimension, Expression memoryspace_expression, std::vector<hsize_t> filespace_dimension, Expression filespace_expression, hid_t dtype) {

        hsize_t nD = my_id.size();

        blitz::Array<hsize_t,1> memoryspace_filter[nD];
        blitz::Array<hsize_t,1> filespace_filter[nD];

        hsize_t memoryspace_dimension_hsize[nD];
        hsize_t filespace_dimension_hsize[nD];

        hsize_t my_start_count_memoryspace[nD]; //Number of points that has been reserved by process with less mpi rank
        hsize_t my_count_memoryspace[nD];   //Number of points a process will write to disk along each dimention

        hsize_t my_start_index_filespace[nD];
        hsize_t my_end_index_filespace[nD];


        this->nD_ = my_id.size();
        this->my_id_ = my_id;
        this->numprocs_ = numprocs;
        this->memoryspace_dimension_ = memoryspace_dimension;
        this->memoryspace_expression_ = memoryspace_expression;
        this->filespace_dimension_ = filespace_dimension;
        this->filespace_expression_ = filespace_expression;
        this->dtype_ = dtype;


        //Set number of points that this process will have along each dimension.
        // for (hsize_t d=0; d<nD; d++)
        //  this->local_N_memoryspace[d] = 
        //  this->local_N_filespace[d] = 


        //Create an empty memoryspace_
        for (hsize_t d=0; d<nD; d++)
            memoryspace_dimension_hsize[d] = memoryspace_dimension[d]/numprocs[d];

        this->memoryspace_ = H5Screate_simple(nD, memoryspace_dimension_hsize, NULL);       
        H5Sselect_none(this->memoryspace_);

        //Set memoryspace_ in plan
        for (Expression::size_type i=0; i<memoryspace_expression.size(); i++) {
            Modify_memoryspace(nD, my_id.data(), numprocs.data(), memoryspace_dimension.data(),  memoryspace_expression[i]);
        }
        for (hsize_t d=0; d<nD; d++) {
            memoryspace_filter[d].resize(memoryspace_dimension[d]);
            memoryspace_filter[d] = 0;
        }

        //Flatten memoryspace_ expression
        for (Expression::size_type i=0; i<memoryspace_expression.size(); i++)
            for (hsize_t d=0; d<nD; d++)
                memoryspace_filter[d](memoryspace_expression[i][d]) = 1;

        //Assume MEMORYSPACE_EQUAL_DIVISION
        //Get start_count and local_count
        for (hsize_t d=0; d<nD; d++) {
            my_start_count_memoryspace[d] = blitz::sum(memoryspace_filter[d](blitz::Range(0,my_id[d]*memoryspace_dimension[d]/numprocs[d]-1)));

            my_count_memoryspace[d] = blitz::sum(memoryspace_filter[d](blitz::Range(my_id[d]*memoryspace_dimension[d]/numprocs[d], (my_id[d]+1)*memoryspace_dimension[d]/numprocs[d]-1)));

        }

        //Create an empty filespace_
        for (hsize_t d=0; d<nD; d++)
            filespace_dimension_hsize[d] = filespace_dimension[d];
            this->filespace_ = H5Screate_simple(nD, filespace_dimension_hsize, NULL);
            H5Sselect_none(this->filespace_);

        for (hsize_t d=0; d<nD; d++) {
            filespace_filter[d].resize(filespace_dimension[d]);
            filespace_filter[d] = 0;
        }

        if (filespace_expression.isEmpty()) {
            filespace_expression = Select::all(nD);
        }

        //Flatten filespace_ expression
        for (Expression::size_type i=0; i<filespace_expression.size(); i++) {
            for (hsize_t d=0; d<nD; d++)
                filespace_filter[d](filespace_expression[i][d]) = 1;
        }

        //Get my_start_index and my_end_index
        for (hsize_t d=0; d<nD; d++) {
            hsize_t i;

            hsize_t my_start_count_filespace = 0;
            for (i=0; i<filespace_dimension[d] && my_start_count_filespace < my_start_count_memoryspace[d]; i++)
                my_start_count_filespace += filespace_filter[d](i);
            my_start_index_filespace[d] = i;

            hsize_t my_count_filespace = 0;
            for (i=my_start_index_filespace[d]; i<filespace_dimension[d] && my_count_filespace<my_count_memoryspace[d]; i++)
                my_count_filespace += filespace_filter[d](i);
            my_end_index_filespace[d] = i;
        }


        //Set filespace_ in plan
        for (Expression::size_type i=0; i<filespace_expression.size(); i++)
            Modify_filespace(nD, my_id.data(), numprocs.data(), filespace_dimension.data(), filespace_expression[i], my_start_index_filespace, my_end_index_filespace);


        hsize_t memoryspace_num_selected_points = H5Sget_select_npoints(this->memoryspace_);
        hsize_t filespace_num_selected_points = H5Sget_select_npoints(this->filespace_);


        if (memoryspace_num_selected_points != filespace_num_selected_points) {
            std::cerr << "Plan::set_plan: Invalid parameter: Number of selected points in memory space = " << " " << memoryspace_num_selected_points << ", does not match with that in file space = " << " " << filespace_num_selected_points << std::endl;
            exit(1);
        }
    }

/*************
* Structures and Functions useful for:
* void Plan::Set_plan(int rank, int* my_id, int* numprocs, Array<int,1>* filespace_filter, Array<int,1>* memoryspace_filter, hid_t datatype)
*
* These structures and functions are not required anywhere else.
*/

struct H5_filter_blocks {
    size_t filespace_start;
    size_t memoryspace_start;
    size_t blocklength;

    H5_filter_blocks(size_t filespace_start, size_t memoryspace_start, size_t blocklength): filespace_start(filespace_start), memoryspace_start(memoryspace_start), blocklength(blocklength){}
};

 struct H5_hyperspace {
    blitz::Array<hsize_t, 1> dimension;
    blitz::Array<hsize_t, 1> start;
    blitz::Array<hsize_t, 1> blockdim;
    blitz::Array<hsize_t, 1> blockstride;
    blitz::Array<hsize_t, 1> blockcount;

    H5_hyperspace(size_t rank);
};

H5_hyperspace::H5_hyperspace(size_t rank) {
    dimension.resize(rank);
    start.resize(rank);
    blockdim.resize(rank);
    blockstride.resize(rank);
    blockcount.resize(rank);

    dimension=0;
    start=0;
    blockdim=0;
    blockstride=1;
    blockcount=0;
}

hsize_t Get_filespace_start(blitz::Array<int,1> filter, hsize_t count){
    size_t sum=0;
    hsize_t index=0;
    for (index=0; ( (index<filter.size()) && (sum<count) ); index++)
        sum += filter(index);

    return index-1;
}

void Get_properties_memspace(blitz::Array<int,1> filter, hsize_t begin_at, int my_id, int numprocs, hsize_t &start, hsize_t &blocklength)
{
    hsize_t local_start = my_id*filter.size()/numprocs;
    hsize_t local_end = (my_id+1)*filter.size()/numprocs-1;
    hsize_t end;

    start = local_start + begin_at + first(filter(blitz::Range(local_start+begin_at, local_end))==1);
    end = start + first(filter(blitz::Range(start, local_end))==0);

    if (start<local_start || start >local_end){
        start=0;
        blocklength=0;
    }
    else if (end<local_start || end >local_end){
        end=local_end+1;
        blocklength=end-start;
        start-=local_start;
    }
    else{
        blocklength=end-start;
        start-=local_start;
    }
}



    void Plan::set_plan(int rank, int* my_id, int* numprocs, blitz::Array<int,1>* filespace_filter, blitz::Array<int,1>* memoryspace_filter, hid_t dtype)
    {   
        this->nD_ = rank;
        this->my_id_ = std::vector<int>(my_id, my_id+rank);
        this->numprocs_ = std::vector<int>(numprocs, numprocs+rank);
        this->dtype_ = dtype;

        std::vector<H5_filter_blocks>* filter_blocks;
        filter_blocks = new std::vector<H5_filter_blocks>[rank];

        size_t local_start_index;
        //size_t local_end;

        hsize_t filespace_start, memoryspace_start, blocklength;
        
        hsize_t filespace_dimension[rank];
        hsize_t memoryspace_dimension[rank];


        for (int r=0; r<rank; r++) {
            if (sum(filespace_filter[r]) != sum(memoryspace_filter[r])) {
                if ((my_id[0]==0))
                    std::cerr << "Plan::set_plan: Invalid parameter: Number of selected points in memory space = " << " " << blitz::sum(memoryspace_filter[r]) << ", does not match with that in file space = " << blitz::sum(filespace_filter[r]) << " along direction " << r << std::endl;
                exit(1);
            }

            filespace_dimension[r]=filespace_filter[r].size();
            memoryspace_dimension[r]=memoryspace_filter[r].size()/numprocs[r];

            local_start_index = my_id[r]*memoryspace_filter[r].size()/numprocs[r];
            //local_end = (my_id[r]+1)*memoryspace_filter[r].size()/numprocs[r]-1; 

            int j=0;
            do {
                Get_properties_memspace(memoryspace_filter[r], j, my_id[r], numprocs[r], memoryspace_start, blocklength);
                j = memoryspace_start + blocklength;

                if (j>0){
                    filespace_start = Get_filespace_start(filespace_filter[r],blitz::sum(memoryspace_filter[r](blitz::Range(0, local_start_index + memoryspace_start))));

                    filter_blocks[r].push_back(H5_filter_blocks(filespace_start, memoryspace_start, blocklength));
                }
            }
            while(j>0);
        }

        this->memoryspace_dimension_ = std::vector<hsize_t>(memoryspace_dimension, memoryspace_dimension+rank);
        this->filespace_dimension_ = std::vector<hsize_t>(filespace_dimension, filespace_dimension+rank);

        // for (int r=0; r<rank; r++){
        //  for (int j=0; j<filter_blocks[r].size(); j++){
        //      cout << "j = " << my_id[r] << " " << j << " " << filter_blocks[r][j].filespace_start << " " << filter_blocks[r][j].memoryspace_start << " " << filter_blocks[r][j].blocklength << endl;
        //  }
        //  cout << endl;
        // }

        size_t num_combinations=1;
        for (int r=0; r<rank; r++)
            num_combinations*=filter_blocks[r].size();

        std::vector<H5_hyperspace> filespace;
        std::vector<H5_hyperspace> memspace;

        filespace.reserve(num_combinations);
        memspace.reserve(num_combinations);

        for (size_t i=0; i<num_combinations; i++){
            filespace.push_back(H5_hyperspace(rank));
            memspace.push_back(H5_hyperspace(rank));
        }


        this->filespace_ = H5Screate_simple(rank, filespace_dimension, NULL);
        this->memoryspace_ = H5Screate_simple(rank, memoryspace_dimension, NULL);

        //Select no elements in the hyperspace
        H5Sselect_none(this->filespace_);
        H5Sselect_none(this->memoryspace_);

        if (num_combinations>0){
            //Perform all the combination
            size_t repeat_rth_rank=num_combinations;
            size_t index;

            for (int r=0; r<rank; r++){
                index=-1;
                repeat_rth_rank = repeat_rth_rank/filter_blocks[r].size();

                for (size_t j=0; j<num_combinations;){
                    index = (index+1)%filter_blocks[r].size();

                    for (size_t k=0; k<repeat_rth_rank; j++, k++){
                        filespace[j].dimension(r)=filespace_filter[r].size();
                        filespace[j].start(r)=filter_blocks[r][index].filespace_start;
                        filespace[j].blockdim(r)=filter_blocks[r][index].blocklength;
                        filespace[j].blockstride(r)=1;
                        filespace[j].blockcount(r)=1;

                        memspace[j].dimension(r)=memoryspace_filter[r].size()/numprocs[r];
                        memspace[j].start(r)=filter_blocks[r][index].memoryspace_start;
                        memspace[j].blockdim(r)=filter_blocks[r][index].blocklength;
                        memspace[j].blockstride(r)=1;
                        memspace[j].blockcount(r)=1;
                    }
                }
            }

            
            //Select required region in the hyperspace
            for (int i=0; i<num_combinations; i++){
                /*if (my_id[0]==0){
                    std::cout << "Filespace: " << my_id[0] << " " << filespace[i].dimension << " " << filespace[i].start << " " << filespace[i].blockstride << " " << filespace[i].blockcount << " " << filespace[i].blockdim << std::endl;
                    std::cout << "Memspace: " << my_id[0] << " " << memspace[i].dimension << " " << memspace[i].start << " " << memspace[i].blockstride << " " << memspace[i].blockcount << " " << memspace[i].blockdim << std::endl;
                }
                
            */
                 H5Sselect_hyperslab(this->filespace_, H5S_SELECT_OR, filespace[i].start.data(), filespace[i].blockstride.data(), filespace[i].blockcount.data(), filespace[i].blockdim.data());

                 H5Sselect_hyperslab(this->memoryspace_, H5S_SELECT_OR, memspace[i].start.data(), memspace[i].blockstride.data(), memspace[i].blockcount.data(), memspace[i].blockdim.data());
            }
        }

    }   

    hid_t Plan::filespace() const {
        return filespace_;
    }
    
    hid_t Plan::memoryspace() const {
        return memoryspace_;
    }

    std::vector<int> Plan::my_id() const {
        return this->my_id_;
    }

    std::vector<int> Plan::numprocs() const {
        return this->numprocs_;
    }

    std::vector<hsize_t> Plan::memoryspace_dimension() const {
        return this->memoryspace_dimension_;
    }

    Expression Plan::memoryspace_expression() const {
        return this->memoryspace_expression_;
    }

    std::vector<hsize_t> Plan::filespace_dimension() const {
        return this->filespace_dimension_;
    }

    Expression Plan::filespace_expression() const {
        return this->filespace_expression_;
    }

    hid_t Plan::dtype() const {
        return dtype_;
    }

}
