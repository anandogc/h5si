Prerequisite:
HDF5, Blitz++


CMake usage:
CXX=mpicxx cmake [-DFIND_LIBRARIES=OFF] [-DCMAKE_BUILD_TYPE=DEBUG] [-DENABLE_PARALLEL=FALSE] -DCMAKE_INSTALL_PREFIX=$HOME/local /path/to/CMakeLists.txt

Example:
CXX=mpicxx cmake ../h5si/trunk
make

Examples can be found in 'trunk/examples' folder