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
 * \file  h5node.cc
 * @author  A. G. Chatterjee
 * @date feb 2012
 * @bug  Does not close sub groups. close_all needs to be implemented for this
 */

#include "h5node.h"
#include "h5group.h"

namespace h5 {

    Node::Node(Group *parent, std::string name) {
        this->parent_ = parent;
        this->name_ = name;

        H5Oget_info_by_name(parent->id(), name.c_str(), &this->info_, H5P_DEFAULT);
    }

    void Node::close_all() {
        /*if (this->isGroup())
            Group(*this).close_all();

        else if (this->isDataset())
            Dataset(*this).close();*/
    }

    H5O_type_t Node::is() {
        return this->info_.type;
    }

    bool Node::isUnknown() {
        return (this->info_.type == H5O_TYPE_UNKNOWN);
    }

    bool Node::isGroup() {
        return (this->info_.type == H5O_TYPE_GROUP);
    }

    bool Node::isDataset() {
        return (this->info_.type == H5O_TYPE_DATASET);
    }

    bool Node::isNamedDatatype() {
        return (this->info_.type == H5O_TYPE_NAMED_DATATYPE);
    }

    bool Node::isNtypes() {
        return (this->info_.type == H5O_TYPE_NTYPES);
    }

    Node::operator Group() {
        return Group(this->parent_, this->name_, "r");
    }

    Node::operator Dataset() {
        return Dataset(this->parent_, this->name_);
    }
}
