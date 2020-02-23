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


PROJECT(H5SI CXX C)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.2)

####################################################
#     Instructions for installing H5SI             #
####################################################
#CMake usage:
#CXX=mpicxx cmake [-DFIND_LIBRARIES=OFF] -DCMAKE_INSTALL_PREFIX=$HOME/local /path/to/CMakeLists.txt
#
#Example:
#CC=mpicc CXX=mpicxx cmake ../h5si
#make
####################################################

SET (DEFAULT_FIND_LIBRARIES ON)                  #ON, OFF - Try to locate packages using pkg-config when ON
SET (DEFAULT_CMAKE_BUILD_TYPE "DISTRIBUTION")    #DISTRIBUTION, DEBUG
SET (DEFAULT_ENABLE_PARALLEL TRUE)               #TRUE, FALSE

SET (INCLUDE_DIRS
        #/path/to/include
    )
SET (LIBRARY_DIRS
       #/path/to/lib
    )

SET (SYSTEM_LIBRARIES blitz hdf5)


####################################################
# Users need not modify any thing beyond this line #
####################################################

#Set version
SET (VERSION "1.1")

#Set default value for FIND_LIBRARIES
IF(NOT DEFINED FIND_LIBRARIES)
    SET (FIND_LIBRARIES ${DEFAULT_FIND_LIBRARIES})
ENDIF()

#Set default value for CMAKE_BUILD_TYPE
IF(NOT CMAKE_BUILD_TYPE)
   SET (CMAKE_BUILD_TYPE ${DEFAULT_CMAKE_BUILD_TYPE})
ENDIF()


#Set default value for ENABLE_PARALLEL
IF(NOT DEFINED ENABLE_PARALLEL)
    SET (ENABLE_PARALLEL ${DEFAULT_ENABLE_PARALLEL})
ENDIF()

#Check VERSION is not empty
IF (NOT VERSION)
    MESSAGE (FATAL_ERROR "VERSION can not be empty")
ENDIF()

MESSAGE(STATUS "Configuring from: '${H5SI_SOURCE_DIR}'")

#Check CXX compiler supports MPI
IF (NOT CXX_SUPPORTS_MPI)        #CXX_SUPPORTS_MPI is undefined when run for the first time.
    MESSAGE(STATUS "MPI support in CXX compiler: ${CMAKE_CXX_COMPILER}")
    FILE(WRITE ${PROJECT_BINARY_DIR}/CMakeFiles/CXX_has_MPI.cc 
               "#include <mpi.h>
                int main(int argc, char** argv)
                { MPI_Init(&argc, &argv);
                  MPI_Finalize();} ")

    EXECUTE_PROCESS(COMMAND ${CMAKE_CXX_COMPILER} ${PROJECT_BINARY_DIR}/CMakeFiles/CXX_has_MPI.cc
                    ERROR_VARIABLE err
                    RESULT_VARIABLE CXX_HAS_MPI)

    IF (CXX_HAS_MPI MATCHES 0)
        MESSAGE(STATUS "MPI support in CXX compiler: ${CMAKE_CXX_COMPILER} -- supported")
    ELSE()
        MESSAGE(FATAL_ERROR "${CMAKE_CXX_COMPILER} does not support MPI")
    ENDIF()
    SET (CXX_SUPPORTS_MPI TRUE CACHE BOOL "CXX_SUPPORTS_MPI")
ENDIF()

#Find the required libraries
IF (NOT ALL_LIBRARIES_FOUND AND FIND_LIBRARIES)
    FUNCTION(DISPLAY_FOUND_LIBRARY DISPLAY_NAME LIB_NAME INCLUDE_PATH ERROR_MESSAGE)
        IF (${LIB_NAME}_FOUND)
            EXECUTE_PROCESS(COMMAND echo ${INCLUDE_PATH}
                            COMMAND rev
                            COMMAND cut -d/ -f2-
                            COMMAND rev
                            OUTPUT_STRIP_TRAILING_WHITESPACE
                            OUTPUT_VARIABLE ${LIB_NAME}_ROOT_DIR)

            MESSAGE(STATUS "Found ${DISPLAY_NAME}: ${${LIB_NAME}_ROOT_DIR} (version \"${${LIB_NAME}_VERSION}\")")
        ELSE()
            MESSAGE(FATAL_ERROR "${DISPLAY_NAME} not found. ${ERROR_MESSAGE}")
        ENDIF()
    ENDFUNCTION()

#Check whether pkg-config is installed
    EXECUTE_PROCESS(COMMAND which pkg-config
                    RESULT_VARIABLE PKG_CONFIG_FOUND
                    OUTPUT_VARIABLE PKG_CONFIG_LOCATION)

    IF (PKG_CONFIG_FOUND MATCHES 0)
        FIND_PACKAGE(PkgConfig REQUIRED)
        
        PKG_CHECK_MODULES(BLITZ QUIET blitz)
        DISPLAY_FOUND_LIBRARY(Blitz++ BLITZ "${BLITZ_INCLUDE_DIRS}" "Set PKG_CONFIG_PATH, e.g. $HOME/local/lib/pkgconfig") 
        
        FIND_PACKAGE(HDF5 QUIET HINTS HDF5_ROOT)
        IF (HDF5_FOUND)  
            #Find installed version of HDF5
            EXECUTE_PROCESS(COMMAND ${HDF5_DIFF_EXECUTABLE} --version
                            COMMAND cut -d\  -f3
                            OUTPUT_STRIP_TRAILING_WHITESPACE
                            OUTPUT_VARIABLE HDF5_VERSION
                            ERROR_VARIABLE HDF5_VERSION_ERROR)
            DISPLAY_FOUND_LIBRARY(HDF5 HDF5 "${HDF5_INCLUDE_DIRS}" "Set HDF5_ROOT, e.g. /usr/local")
            IF (HDF5_IS_PARALLEL) #Check whether HDF5 has parallen installation or not
                    MESSAGE(STATUS "HDF5 supports parallel IO: yes")
            ELSE()
                    MESSAGE(FATAL_ERROR "HDF5 supports parallel IO: no")
            ENDIF()
        ENDIF()
    ELSE()
        MESSAGE(STATUS "Warning: pkg-config not found, libraries will not be searched for. Supply -DFIND_LIBRARIES=OFF to CMake to disable library search.")
    ENDIF()
    SET(ALL_LIBRARIES_FOUND TRUE CACHE BOOL "ALL_LIBRARIES_FOUND")
ENDIF()


#Folders where header files will be searched for
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/lib

${BLITZ_INCLUDE_DIRS}
${HDF5_INCLUDE_DIRS}
${CACHED_INCLUDES}
${INCLUDE_DIRS}
)

#Folders where libraries will be searched for
LINK_DIRECTORIES(
${BLITZ_LIBRARY_DIRS}
${HDF5_LIBRARY_DIRS}
${CACHED_LIBRARY_DIRS}
${LIBRARY_DIRS}
)

ADD_DEFINITIONS(-DVERSION=${VERSION} ${HDF5_DEFINITIONS})

IF (ENABLE_PARALLEL)
    ADD_DEFINITIONS(-DH5SI_ENABLE_MPI)
ENDIF()

SET(SYSTEM_LIBRARIES ${SYSTEM_LIBRARIES} ${HDF5_LIBRARIES} ${BLITZ_LIBRARIES} ${CACHED_LIBRARIES})

IF (NOT CACHED_INCLUDES)
    SET (CACHED_INCLUDES ${BLITZ_INCLUDE_DIRS} ${HDF5_INCLUDE_DIRS} CACHE STRING "CACHED_INCLUDES")
ENDIF ()

IF (NOT CACHED_LIBRARY_DIRS)
    SET (CACHED_LIBRARY_DIRS ${BLITZ_LIBRARY_DIRS} ${HDF5_LIBRARY_DIRS} CACHE STRING "CACHED_LIBRARY_DIRS")
ENDIF ()

IF (NOT CACHED_LIBRARIES)
    SET (CACHED_LIBRARIES ${BLITZ_LIBRARIES} ${HDF5_LIBRARIES} CACHE STRING "CACHED_LIBRARIES")
ENDIF ()



#Build type
IF (NOT CMAKE_BUILD_TYPE_DISPLAYED)
    IF ( (NOT ${CMAKE_BUILD_TYPE} MATCHES "DISTRIBUTION") AND (NOT ${CMAKE_BUILD_TYPE} MATCHES "DEBUG") )
        MESSAGE(FATAL_ERROR "Build type can be one of DISTRIBUTION, DEBUG")
    ENDIF()

    MESSAGE(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
    SET (CMAKE_BUILD_TYPE_DISPLAYED TRUE CACHE BOOL "CMAKE_BUILD_TYPE_DISPLAYED")
ENDIF ()

MESSAGE(STATUS "Parallel: ${ENABLE_PARALLEL}")

SET(CMAKE_CXX_FLAGS_DISTRIBUTION "-O3")

SET(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

ADD_SUBDIRECTORY(lib)
ADD_SUBDIRECTORY(examples)



