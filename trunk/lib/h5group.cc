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
 * \file  h5group.cc
 * @author  A. G. Chatterjee
 * @date feb 2012
 * @bug  No known bugs
 */

#include "h5group.h"

//Todo create copy constructor


namespace h5 {

    Group::Group() {
        this->id_ = 0;
        this->parent_ = 0;
        this->num_obj_ = -1;
        this->is_root_ = false;
        this->parent_ = NULL;
        this->driver_ = "";
#ifdef H5SI_ENABLE_MPI
        this->MPI_COMMUNICATOR = MPI_COMM_NULL;
#endif
    }

        

    //Copy exesting dataset object
    Group::Group(const Group& grp):is_root_(grp.is_root_), id_(grp.id_), name_(grp.name_), num_obj_(grp.num_obj_), parent_(grp.parent_), driver_(grp.driver_) {

#ifdef H5SI_ENABLE_MPI
        this->MPI_COMMUNICATOR = this->parent_->MPI_COMMUNICATOR;
#endif

        this->id_ = H5Gopen2(this->parent_->id_, this->name_.c_str(), H5P_DEFAULT) ;
    }

    Group::Group(Group *parent, std::string name, std::string access_mode) {

        // Readonly, file must exist
        if (access_mode == "r") {
            this->id_ = H5Gopen2(parent->id_, name.c_str(), H5P_DEFAULT) ;
        }

        // Read/write if exists, create otherwise
        else if (access_mode == "a") {
            this->id_ = H5Gopen2(parent->id_, name.c_str(), H5P_DEFAULT) ;
            if (this->id_ < 0)
                this->id_ = H5Gcreate2(parent->id_, name.c_str(), H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        }

        else {
            std::cerr << "Invalid access mode" << std::endl;
            exit(1);
        }


        if (this->id_ > 0) {
            this->name_ = this->parent_->name_ + "/" + name;

            H5Gget_num_objs(this->id_, &this->num_obj_);

            // this->parent_->register_node(static_cast<Node>(*this));
        }

        this->parent_ = parent;
        this->is_root_ = false;
        this->driver_ = this->parent_->driver_;

#ifdef H5SI_ENABLE_MPI
        this->MPI_COMMUNICATOR = this->parent_->MPI_COMMUNICATOR;
#endif
    }

    Group::~Group() {
        this->close();
        //this->close_all();
    }

    void Group::register_node(Node node) {
        //this->child_nodes_opened.push_back(node);
    }

    void Group::close() {
        if (!this->is_root_/* and reference_count_[this->id_] == 1*/)
            H5Gclose(this->id_);
    }
    void Group::close_all() {
        /*for (std::vector<Node>::size_type i=0; i<this->child_nodes_opened.size(); i++)
            this->child_nodes_opened[i].close_all();
        */
        this->close();
    }

    Group Group::createGroup(std::string name) {
        return Group(this, name, "r");
    }

    Group Group::requireGroup(std::string name) {
        return Group(this, name, "a");
    }

/*

    H5Dataset Group::requireDataset(std::string name) {
        // return H5Dataset(this->id_, name, "a");
    }*/

    std::string Group::name() {
        return this->name_;
    }

    Group *Group::parent() {
        return this->parent_;
    }

    std::string Group::driver() const {
        return this->driver_;
    }

    hsize_t Group::getNumObjects() {
        return this->num_obj_;
    }

    //indexing_order is one of
    //H5_INDEX_NAME          -> Sort by name (default)
    //H5_INDEX_CRT_ORDER     -> Sort by creation order
    std::vector<std::string> Group::keys(H5_index_t indexing_order) {
            
        std::vector<std::string> name_list;

        size_t nodename__max_length = 65535;
        char nodename_[nodename__max_length+1]; // +1 for null character

        if (this->id_ <=0)
            return name_list;

        
        name_list.resize(num_obj_);

        for (hsize_t i = 0; i < num_obj_; i++) {
            H5Lget_name_by_idx(this->id_, ".", indexing_order, H5_ITER_INC, i, nodename_, nodename__max_length+1, H5P_DEFAULT);

            name_list[i] = nodename_;
        }
        
        return name_list;
    }

    std::vector<Node> Group::values() {
            
        std::vector<std::string> name_list = this->keys();
        std::vector<Node> node_list;

        node_list.reserve(name_list.size());
        
        for (std::vector<std::string>::size_type i=0; i<name_list.size(); i++)
            node_list[i] = Node(this, name_list[i]);
        
        return node_list;
    }

    bool Group::contains(std::string name) {
        std::vector<std::string> name_list = this->keys();
        if (std::find(name_list.begin(), name_list.end(), name) != name_list.end())
            return true;

        return false;
    }

    Node Group::operator[](const std::string name) {
        return Node(this, name);
    }

    Node Group::operator[](const hsize_t n) {
        size_t nodename__max_length = 65535;
        char nodename_[nodename__max_length+1]; // +1 for null character

        H5Lget_name_by_idx(this->id_, ".", H5_INDEX_NAME, H5_ITER_INC, n, nodename_, nodename__max_length+1, H5P_DEFAULT);

        return Node(this, nodename_);
    }

    void Group::operator=(const Group &group) {
        if (this->id_ > 0)
            H5Gclose(this->id_);

        this->is_root_ = group.is_root_;

        this->name_ = group.name_;

        this->num_obj_ = group.num_obj_;

        this->parent_ = group.parent_;

        this->driver_ = group.parent_->driver_;

#ifdef H5SI_ENABLE_MPI
        this->MPI_COMMUNICATOR = group.parent_->MPI_COMMUNICATOR;
#endif

        this->id_ = H5Gopen2(this->parent_->id_, this->name_.c_str(), H5P_DEFAULT) ;
    }

    void Group::operator delete(void* ptr, std::size_t sz)
    {
        Group *group = (Group*)ptr;
        H5Ldelete(group->parent_->id_, group->name_.c_str(), H5P_DEFAULT);
    }
}
