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
 * \file  h5group.h
 * @author  A. G. Chatterjee
 * @date feb 2012
 * @bug  No known bugs
 */

#include <hdf5.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string>
#include <vector>
#include <iostream>

#include "h5node.h"
#include "h5dataset.h"

//*********************************************************************************************
#ifndef _H_H5GROUP
#define _H_H5GROUP


namespace h5 {

    class Group
    {

    protected:
        bool is_root_;

        hid_t id_;
        std::string name_;

        hsize_t num_obj_;

        Group *parent_;

        std::string driver_;

//      std::vector<Node> child_nodes_opened;       //List of opened child nodes

    public:

#ifdef H5SI_ENABLE_MPI
        MPI_Comm MPI_COMMUNICATOR;
#endif

        typedef hsize_t size_type;

        Group();
        Group(const Group& ds);
        Group(Group *parent, std::string name, std::string access_mode);
        ~Group();

        void register_node(Node node);
        void close();
        void close_all();

        Group createGroup(std::string name);
        Group requireGroup(std::string name);

        Dataset create_dataset(std::string name, std::vector<hsize_t> shape, std::string dtype) {
            return Dataset(this, name, shape, dtype);
        }
        Dataset create_dataset(std::string name, Plan plan, std::string dtype="") {
            return Dataset(this, name, plan, dtype);
        }
        // Dataset requireDataset(std::string name);

        hid_t id() {return this->id_;};
        std::string name();
        Group *parent();
        std::string driver() const;

        hsize_t getNumObjects();

        std::vector<std::string> keys(H5_index_t indexing_order=H5_INDEX_NAME);
        std::vector<Node> values();

        bool contains(std::string name);

        Node operator[](const std::string name);
        Node operator[](const hsize_t n);

        void operator=(const Group &group);

        static void operator delete(void* ptr, std::size_t sz);     //Delete this group
    };

}

#endif
