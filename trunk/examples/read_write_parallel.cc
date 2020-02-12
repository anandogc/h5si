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
 * \file  read_write_parallel.cc
 * @author  A. G. Chatterjee
 */

 #include "h5si.h"

#include <iostream>
#include <complex>
#include <hdf5.h>

#include <mpi.h>

using namespace std;
using namespace blitz;

#define Complex complex<double>
#define H5Complex "cdouble"

int N = 16;
int my_rank, numprocs;
double MY_EPS = 1E-12;

void init_array(Array<Complex,3> A) {
	for (int i=0; i<A.extent(0); i++)
		for (int j=0; j<A.extent(1); j++)
			for (int k=0; k<A.extent(2); k++)
				A(i,j,k) = Complex(i*A.extent(1)*A.extent(2)*2 + j*A.extent(2)*2 + k*2, i*A.extent(1)*A.extent(2)*2 + j*A.extent(2)*2 + k*2+1);

}

void write_test() {
	assert(numprocs<N && N%numprocs == 0);  // numprocs Must be less than N and a divisor of it
	int local_x = N/numprocs;

	Array<Complex,3> A(local_x,N,N);  // Create array with local array shape

	init_array(A);

	h5::File f;
	f.mpioInit(MPI_COMM_WORLD);  // If using parallel IO, mpioInit must be called before using any other function
	f.open("foo.h5", "w");

	//Method1
	h5::Dataset ds1 = f.create_dataset("U.V1", h5::shape(N,N,N), H5Complex); //Give global shape of the dataset
	ds1 << A.data();


	//Method 2
	h5::Plan plan;
	plan.set_plan(MPI_COMM_WORLD, h5::shape(N,N,N), h5::Select::all(3), h5::shape(N,N,N), h5::Select::all(3), h5::Dtype(H5Complex));
	h5::Dataset ds2 = f.create_dataset("U.V2", plan);
	ds2 << A.data();
}


void read_test() {
	assert(numprocs<N && N%numprocs == 0); // numprocs Must be less than N and a divisor of it
	int local_x = N/numprocs;
	Array<Complex,3> A(local_x,N,N);

	h5::File f("foo.h5", "r");
	f.mpioInit(MPI_COMM_WORLD);  // If using parallel IO, mpioInit must be called before using any other function

	//Method 1
	f["U.V1"] >> A.data();

	//Error checking
	int error = 0; // For error checking
	int count = 0; // Count no of process in which error occured

	Array<Complex,3> A_orig(local_x,N,N);  //For comparing with read values
	init_array(A_orig);


	if (max(abs(A-A_orig)) > MY_EPS)  // If maximum of difference is more than MY_EPS, it is considered as an error
		error = 1;

	MPI_Reduce(&error, &count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	if (my_rank == 0 and count > 0)
		cerr << "Error reading values in " << count << " no. of process." << endl;
	else if (my_rank == 0)
		cout << "All is well." << endl;


	//Method 2
	h5::Plan plan;
	plan.set_plan(MPI_COMM_WORLD, h5::shape(N,N,N), h5::Select::all(3), h5::shape(N,N,N), h5::Select::all(3), h5::Dtype(H5Complex));

	h5::Dataset ds = f["U.V2"];
	ds.set_plan(plan);
	ds >> A.data();

	//Error checking
	error = 0;
	count = 0;
	if (max(abs(A-A_orig)) > MY_EPS)  // If maximum of difference is more than MY_EPS, it is considered as an error
		error = 1;

	MPI_Reduce(&error, &count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	if (my_rank == 0 and count > 0)
		cerr << "Error reading values in " << count << " no. of process." << endl;
	else if (my_rank == 0)
		cout << "All is well." << endl;

}

int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);
	h5::init();

	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

	write_test();
	read_test();

	h5::finalize();
	MPI_Finalize();

	return 0;
}

