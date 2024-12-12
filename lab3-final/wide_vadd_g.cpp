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

#define BUFFER_SIZE 16
#define DATAWIDTH 512
#define VECTOR_SIZE (DATAWIDTH / 32) // vector size is 16 (512/32 = 16)
typedef ap_uint<DATAWIDTH> uint512_dt;

//TRIPCOUNT identifier
const unsigned int c_chunk_sz = BUFFER_SIZE;
const unsigned int c_size     = VECTOR_SIZE;

/*
    Vector Addition Kernel Implementation using uint512_dt datatype
    Arguments:
        in1   (input)     --> Input Vector1
        in2   (input)     --> Input Vector2
        out   (output)    --> Output Vector
        size  (input)     --> Size of Vector in Integer
   */
extern "C"
{
    void vadd(
        const uint512_dt *in1, // Read-Only Vector 1
        const uint512_dt *in2, // Read-Only Vector 2
        uint512_dt *out,       // Output Result
        int size               // Size in integer
    )
    {
#pragma HLS INTERFACE m_axi port = in1 bundle = gmem
#pragma HLS INTERFACE m_axi port = in2 bundle = gmem1
#pragma HLS INTERFACE m_axi port = out bundle = gmem2
#pragma HLS INTERFACE s_axilite port = in1 bundle = control
#pragma HLS INTERFACE s_axilite port = in2 bundle = control
#pragma HLS INTERFACE s_axilite port = out bundle = control
#pragma HLS INTERFACE s_axilite port = size bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control

        uint512_dt v1_local[BUFFER_SIZE]; // Local memory to store vector1
        uint512_dt v2_local[BUFFER_SIZE];
        uint512_dt result_local[BUFFER_SIZE]; // Local Memory to store result

int chunk_size = BUFFER_SIZE;

            for (int j = 0; j < chunk_size; j++) {	//traverses each row of 1st matrix
// #pragma HLS pipeline
// #pragma HLS LOOP_TRIPCOUNT min = 1 max = 16
                    // v1_local[j] = in1[i * chunk_size + j];	//deprecated, we dont need i loop
                    v1_local[j] = in1[j];
                for (int k = 0; k < chunk_size; k++) {	//traverses each row of 2nd matrix
// #pragma HLS pipeline
// #pragma HLS LOOP_TRIPCOUNT min = 1 max = 16
                    // v2_local[k] = in2[i * chunk_size + k];	//deprecated, we dont need i loop
                    v2_local[k] = in2[k];
					
					ap_uint<32> current_result = 0;
					uint512_dt result_local = 0;
					
					for (int vector = 0; vector < VECTOR_SIZE; vector++) {	//traverses all elements of each given row
                    #pragma HLS UNROLL
                        ap_uint<32> tmp1 = v1_local[j].range(32 * (vector + 1) - 1, vector * 32);	//the row of the 1st matrix
                        ap_uint<32> tmp2 = v2_local[k].range(32 * (vector + 1) - 1, vector * 32);	//the row of the 2nd matrix
                        ap_uint<32> product = tmp1 * tmp2;

                        current_result = current_result + product;	//evaluates a single element once this loop ends
                    }
					result_local.range(32 * (k + 1) - 1, k * 32) = current_result;			//using k instead of vector, cuz vector exists only in the above loop. both vector and k go from 0 to 15
																							//when vector loop ends: 1element
																							//when k loop ends: 1 row
																							//when j loop ends: all matrix
				}
				// out[i * chunk_size + j] = result_local;	//deprecated, we dont need i loop
				out[j] = result_local;	//saves each row to each row of our matrix
										//each result temp contains 16 elements (one row)
										//each out[] element contains 16 elements (one row)
		}
	}
}
