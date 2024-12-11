/**********
Copyright (c) 2018, Xilinx, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**********/

/*******************************************************************************
Description:
    Wide Memory Access Example using ap_uint<Width> datatype
    Description: This is vector addition example to demonstrate Wide Memory
    access of 512bit Datawidth using ap_uint<> datatype which is defined inside
    'ap_int.h' file.
*******************************************************************************/

//Including to use ap_uint<> datatype
#include <ap_int.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 64
#define DATAWIDTH 512
#define VECTOR_SIZE (DATAWIDTH / 32) // vector size is 16 (512/32 = 16)
typedef ap_uint<DATAWIDTH> uint512_dt;

extern "C" {
    void matrix_mult(
        const uint512_dt *in1, // Read-Only Matrix A
        const uint512_dt *in2, // Read-Only Matrix B
        uint512_dt *out,       // Output Matrix C
        int m,                 // Rows of A
        int n,                 // Columns of A / Rows of B
        int p                  // Columns of B
    ) {
#pragma HLS INTERFACE m_axi port = in1 bundle = gmem
#pragma HLS INTERFACE m_axi port = in2 bundle = gmem1
#pragma HLS INTERFACE m_axi port = out bundle = gmem2
#pragma HLS INTERFACE s_axilite port = in1 bundle = control
#pragma HLS INTERFACE s_axilite port = in2 bundle = control
#pragma HLS INTERFACE s_axilite port = out bundle = control
#pragma HLS INTERFACE s_axilite port = m bundle = control
#pragma HLS INTERFACE s_axilite port = n bundle = control
#pragma HLS INTERFACE s_axilite port = p bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control

        uint512_dt v1_local[BUFFER_SIZE]; // Local memory for Matrix A
        uint512_dt v2_local[BUFFER_SIZE]; // Local memory for Matrix B
        uint512_dt result_local[BUFFER_SIZE]; // Local memory for Matrix C

#pragma HLS STREAM variable = v1_local depth = 64
#pragma HLS STREAM variable = v2_local depth = 64
#pragma HLS STREAM variable = result_local depth = 64
#pragma HLS DATAFLOW

        for (int i = 0; i < m; i++) { // Loop over rows of A
            for (int j = 0; j < p; j++) { // Loop over columns of B
                uint512_dt tmpOut = 0; // To store the result of one output element
                for (int k = 0; k < n; k += VECTOR_SIZE) { // Loop over columns of A and rows of B
#pragma HLS PIPELINE
                    uint512_dt tmpV1 = in1[i * n / VECTOR_SIZE + k / VECTOR_SIZE];
                    uint512_dt tmpV2 = in2[k / VECTOR_SIZE * p / VECTOR_SIZE + j / VECTOR_SIZE];

                    // Perform multiplication and summation
                    for (int vector = 0; vector < VECTOR_SIZE; vector++) {
#pragma HLS UNROLL
                        ap_uint<32> tmp1 = tmpV1.range(32 * (vector + 1) - 1, vector * 32);
                        ap_uint<32> tmp2 = tmpV2.range(32 * (vector + 1) - 1, vector * 32);
                        ap_uint<32> tmpResult = tmp1 * tmp2;

                        // Sum the results into tmpOut
                        ap_uint<32> prevSum = tmpOut.range(32 * (vector + 1) - 1, vector * 32);
                        tmpOut.range(32 * (vector + 1) - 1, vector * 32) = prevSum + tmpResult;
                    }
                }
                // Write result to output matrix
                out[i * p / VECTOR_SIZE + j / VECTOR_SIZE] = tmpOut;
            }
        }
    }
}
