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
 * \file  read_write.cc
 * @author  A. G. Chatterjee
 */
#include "h5si.h"

#include <iostream>
#include <hdf5.h>

using namespace std;
using namespace blitz;

double MY_EPS = 1E-12;

void write_test() {
	Array<int,3> A(3,4,5);

	firstIndex i;
	secondIndex j;
	thirdIndex k;

	A = i*4*5 + j*5 + k;


	h5::File f("foo.h5", "w");

	h5::Dataset ds = f.create_dataset("A", h5::shape(3,4,5), "int");

	ds << A.data();
}


void read_test() {
	Array<int,3> A(3,4,5);

	h5::File f("foo.h5", "r");

	f["A"] >> A.data();

	//Error checking
	Array<int,3> A_orig(3,4,5);

	//Initialize A_orig
	firstIndex i; 
	secondIndex j;
	thirdIndex k;
	A_orig = i*4*5 + j*5 + k;

	// If maximum of difference is more than MY_EPS, it is considered as an error
	if (max(abs(A-A_orig)) > MY_EPS)
		cerr << "Error in reading." << endl;
	else
		cout << "All is well."
}

int main(int argc, char *argv[])
{
	h5::init();

	write_test();
	read_test();

	h5::finalize();

	return 0;
}

