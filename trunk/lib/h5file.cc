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
 * \file  h5file.cc
 * @author  A. G. Chatterjee
 * @date feb 2012
 * @bug  No known bugs
 */

#include "h5file.h"

//Todo create copy constructor

namespace h5 {

    File::File() {
        // create a file access template to be used by corresponding drivers to set properties
        this->is_root_ = true;
        this->driver_ = "sec2";
        this->fapl_ = H5Pcreate(H5P_FILE_ACCESS);
    }

    File::File(std::string filename, std::string access_mode) {
        this->is_root_ = true;
        this->driver_ = "sec2";
        this->fapl_ = H5Pcreate(H5P_FILE_ACCESS);

        open(filename, access_mode);
    }


    File::~File() {
        this->close();
    }

    int File::open(std::string filename, std::string access_mode)
    {
        //if (this->id_>0)
            //close_all();

        // Readonly, file must exist
        if (access_mode == "r")
            this->id_ = H5Fopen(filename.c_str(), H5F_ACC_RDONLY, this->fapl_);

        // Read/write, file must exist
        else if (access_mode == "r+")
            this->id_ = H5Fopen(filename.c_str(), H5F_ACC_RDWR, this->fapl_);

        // Create file, truncate if exists
        else if (access_mode == "w")
            this->id_ = H5Fcreate(filename.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, this->fapl_);

        // Create file, fail if exists
        else if (access_mode == "w-" || access_mode == "x")
            this->id_ = H5Fcreate(filename.c_str(), H5F_ACC_EXCL, H5P_DEFAULT, this->fapl_);

        // Read/write if exists, create otherwise
        else if (access_mode == "a") {
            this->id_ = H5Fopen(filename.c_str(), H5F_ACC_RDWR, this->fapl_);
            if (this->id_ < 0)
                this->id_ = H5Fcreate(filename.c_str(), H5F_ACC_EXCL, H5P_DEFAULT, this->fapl_);
        }
        else {
            std::cerr << "Invalid access mode" << std::endl;
            exit(1);
        }

        if (this->id_<0){
            std::cerr << "File:: Unable to open " << filename << " for writing." << std::endl;
            exit(1);
        }

        this->filename_ = filename;
        this->access_mode_ = access_mode;

        this->name_ = "/";
        H5Gget_num_objs(this->id_, &this->num_obj_);

        return 0;
    }

    void File::close() {
        Group::close_all();
        
        if (this->id_ > 0)
            H5Fclose(this->id_);

        if (this->fapl_ > 0)
            H5Pclose(this->fapl_) ;
    }

    void File::flush() {
        H5Fflush(this->id_, H5F_SCOPE_GLOBAL);
    }

    void File::resetFapl() {
        if (this->fapl_)
             H5Pclose(this->fapl_);

        this->fapl_ = H5Pcreate(H5P_FILE_ACCESS);
    }

    hid_t File::getFapl() {
        return this->fapl_;
    }

    void File::setFapl(hid_t fapl) {
        this->fapl_ = fapl;
    }

    /**
     * \brief Sets the File object to use the H5FD_CORE driver.
     *         This is used to work with a file in memory
     *
     *       The H5FD_CORE driver enables an application to work with a file in memory,
     * speeding reads and writes as no disk access is made. File contents are stored only
     * in memory until the file is closed. The \c backing_store parameter determines whether
     * file contents are ever written to disk.
     *
     * \c increment Specifies the increment by which allocated memory is to be increased
     * each time more memory is required.
     *
     * If the \c backing_store is set to 1 (TRUE), the file contents are
     * flushed to a file with the same name as this core file when the file is closed or
     * access to the file is terminated in memory.
     *
     * If an existing file with H5FD_CORE driver in Read/Write mode with the \c backing_store
     * set to 1, any change to the file contents are saved to the file when the file is
     * closed. If it is opened in Read/Write mode with \c backing_store set to 0, any change
     * to the file contents will be lost when the file is closed. If it is opened in
     * Read only mode, no change to the file is allowed either in memory or on file.
     *
     * Note:
     * Currently this driver cannot create or open family or multi files.
     *
     * \param increment      [in] Size, in bytes, of memory increments.
     * \param backing_store  [in] Boolean flag indicating whether to write the file contents
     *                            to disk when the file is closed.
     *
     * \returns Returns a non-negative value if successful. Otherwise returns a negative value.
     */
    herr_t File::coreInit(size_t increment, hbool_t backing_store) {
        this->driver_ = "core";
        return H5Pset_fapl_core(fapl_, increment, backing_store);
    }

    /**
     * \brief  Buffered I/O using functions from stdio.h.
     *
     * \returns Returns a non-negative value if successful. Otherwise returns a negative value. 
     */
    herr_t File::stdioInit() {
        this->driver_ = "sidio";
        return H5Pset_fapl_stdio(fapl_);
    }

#ifdef H5SI_ENABLE_MPI
    /**
    * \brief Stores MPI IO communicator information and some optimization parameters to 
    * the File object.
    *
    * This stores the user-supplied MPI IO parameters \c MPI_COMMUNICATOR, for communicator. 
    * This function is available only when compiled with the parallel HDF5 library and is
    * not a collective function.

    * \c MPI_COMMUNICATOR is the MPI communicator to be used for file open, as defined in
    * MPI_FILE_OPEN of MPI-2. This function makes a duplicate of the communicator, so
    * modifications to comm after this function call returns have no effect on the file.
    *
    * Note:
    * 1) This function does not create a duplicated communicator. Modifications to comm after
    *    this function call returns may have an undetermined effect on the access property list.
    *    Users should not modify the communicator while it is defined in a property list.
    *
    * 2) Raw dataset chunk caching is not currently supported when using this file driver in
    *    read/write mode. All IO will access the disk directly, and H5Pset_cache and
    *    H5Pset_chunk_cache will have no effect on performance.
    *    Raw dataset chunk caching is supported when this driver is used in read-only mode.
    *
    *
    * \param  MPI_COMMUNICATOR     [in]: MPI-2 communicator
    *
    * \returns Returns a non-negative value if successful. Otherwise returns a negative value.
    */
    herr_t File::mpioInit(MPI_Comm MPI_COMMUNICATOR) {
            // set the file access template for parallel IO access 
        this->fapl_ = H5Pcreate(H5P_FILE_ACCESS);
        // H5Pset_sieve_buf_size(acc_template, 262144); //256KB: must be a multiple of disk block size
        H5Pset_sieve_buf_size(this->fapl_, 0);
        H5Pset_alignment(this->fapl_, 524288, 262144); //512KB, 256KB: For MPI-IO and other parallel systems, choose an alignment which is a multiple of the disk block size. Any file object greater than or equal in size to threshold (middle argument) bytes will be aligned on an address which is a multiple of alignment (last argument)

        //
        // http://www.mpi-forum.org/docs/mpi-2.0/mpi-20-html/node182.htm for information on access_style, ..

        MPI_Info FILE_INFO_TEMPLATE;
        MPI_Info_create(&FILE_INFO_TEMPLATE);
        MPI_Info_set(FILE_INFO_TEMPLATE, (char*)"access_style", (char*)"write_once");
        MPI_Info_set(FILE_INFO_TEMPLATE, (char*)"collective_buffering", (char*)"true");
        MPI_Info_set(FILE_INFO_TEMPLATE, (char*)"cb_block_size", (char*)"1048576"); //1MB:   1024*1024 bytes
        MPI_Info_set(FILE_INFO_TEMPLATE, (char*)"cb_buffer_size", (char*)"4194304"); //4MB: 4*1024*1024 bytes

        // tell the HDF5 library that we want to use MPI-IO to do the writing 
        this->driver_ = "mpio";
        this->MPI_COMMUNICATOR = MPI_COMMUNICATOR;
        return H5Pset_fapl_mpio(this->fapl_, MPI_COMMUNICATOR, FILE_INFO_TEMPLATE);
    }

#endif

    Node File::operator[](const std::string name) {
        return Node(this, name);
    }

    Node File::operator[](const hsize_t n) {
        size_t nodename_max_length = 65535;
        char nodename_[nodename_max_length+1];  // +1 for null character

        H5Lget_name_by_idx(this->id_, ".", H5_INDEX_NAME, H5_ITER_INC, n, nodename_, nodename_max_length+1, H5P_DEFAULT);

        return Node(this, nodename_);
    }
/*
    void File::operator=(const File &file) {
        if (this->id_ > 0)
            H5Fclose(this->id_);

        this->parent_ = NULL;

        this->driver_ = file.driver_;

        this->fapl_ = file.fapl_;

#ifdef H5SI_ENABLE_MPI
        this->MPI_COMMUNICATOR = file.MPI_COMMUNICATOR;
#endif

        this->open(file.filename_, file.access_mode_);

    }*/

}
