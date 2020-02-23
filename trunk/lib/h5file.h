/* H5SI
 *
 * Copyright (C) 2020, Mahendra K. Verma, Anando Gopal Chatterjee
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
 * \file  h5file.h
 * @author  A. G. Chatterjee
 * @date feb 2012
 * @bug  No known bugs
 */

#ifndef _H_H5FILE
#define _H_H5FILE

#include <hdf5.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include "h5group.h"

namespace h5 {

    /**
     *  This class is used to open an HDF5 file. A more elaborate class description.
     */

    class File : public Group
    {
        hid_t fapl_; ///< Create a file access template to be used by corresponding drivers to set properties

        std::string filename_;
        std::string access_mode_;

    public:
        File();
        File(std::string filename, std::string access_mode);
        ~File();

        int open(std::string filename, std::string access_mode);
        void close();
        void flush();

        void resetFapl();

        hid_t getFapl();
        void  setFapl(hid_t fapl);


        // driver specific initializers
        herr_t coreInit(size_t increment, hbool_t backing_store);
        herr_t stdioInit();

#ifdef H5SI_ENABLE_MPI

        herr_t mpioInit(MPI_Comm MPI_COMMUNICATOR);

#endif

        Node operator[](const std::string name);
        Node operator[](const hsize_t n);

        // void operator=(const File &file);

    };

}

#endif
