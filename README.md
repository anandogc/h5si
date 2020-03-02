Prerequisite:
HDF5(tested upto v1.8.20), Blitz++


CMake usage:

    CXX=mpicxx cmake [-DFIND_LIBRARIES=OFF] [-DCMAKE_BUILD_TYPE=DEBUG] [-DENABLE_PARALLEL=FALSE] -DCMAKE_INSTALL_PREFIX=$HOME/local /path/to/CMakeLists.txt

Installation example:

    CXX=mpicxx cmake ../h5si/trunk
    make -j8
	make install

I/O Examples can be found in 'trunk/examples' folder
