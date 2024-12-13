#include <ap_int.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 16
#define DATAWIDTH 512
#define VECTOR_SIZE (DATAWIDTH / 32) // vector size is 16 (512/32 = 16)
typedef ap_uint<DATAWIDTH> uint512_dt;

// TRIPCOUNT identifier
const unsigned int c_chunk_sz = BUFFER_SIZE;
const unsigned int c_size = VECTOR_SIZE;

extern "C"
{
    void vadd(
        const uint512_dt *in1, // Read-Only Matrix 1
        const uint512_dt *in2, // Read-Only Matrix 2
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
		//matrices which we will use
        uint512_dt v1_local[BUFFER_SIZE];
        uint512_dt v2_local[BUFFER_SIZE];
        uint512_dt result_local[BUFFER_SIZE];
		//variables which we will use
		ap_uint<32> sum;
		ap_uint<32> a;
		ap_uint<32> b;
		ap_uint<32> temp;
		
        for (int i = 0; i < BUFFER_SIZE; i++) // Iterate over rows of Matrix 1
        {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT min = 1 max = BUFFER_SIZE
            //load a row of Matrix 1 into local memory
            v1_local[i] = in1[i];
			//load a column of Matrix 2 into local memory
            v2_local[i] = in2[i];
        }

	//Transposes the v2_local (source_in2) matrix to be fed in the multiplication function
	for (int i = 0; i < BUFFER_SIZE; i++) {
#pragma HLS LOOP_TRIPCOUNT min = 1 max = BUFFER_SIZE
		 for (int j = i+1; j < BUFFER_SIZE; j++) {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT min = 1 max = BUFFER_SIZE
			 temp = v2_local[i].range(32 * (j + 1) - 1, 32 * j);
			 v2_local[i].range(32 * (j + 1) - 1, 32 * j) = v2_local[j].range(32 * (i + 1) - 1, 32 * i);
			 v2_local[j].range(32 * (i + 1) - 1, 32 * i) = temp;
			 //v2_local[i].range(32 * (j + 1) - 1, 32 * j); //traverses row right
			 //v2_local[j].range(32 * (i + 1) - 1, 32 * i); //traverse collumn down
		 }
	}


//lab2 optimisations:
#pragma HLS ARRAY_PARTITION variable=v1_local type=complete dim=1
#pragma HLS ARRAY_PARTITION variable=v2_local type=complete dim=1
	
        // for (int j = 0; j < BUFFER_SIZE; j++) // Iterate over columns of Matrix 2
        // {
// #pragma HLS PIPELINE
            //load a column of Matrix 2 into local memory
            // v2_local[j] = in2[j];
        // }

        for (int i = 0; i < BUFFER_SIZE; i++) //Iterate over rows of Matrix 1
        {
#pragma HLS LOOP_TRIPCOUNT min = 1 max = BUFFER_SIZE
            for (int j = 0; j < BUFFER_SIZE; j++) //Iterate over columns of Matrix 2
            {
#pragma HLS LOOP_TRIPCOUNT min = 1 max = BUFFER_SIZE
                // ap_uint<32> sum = 0;
                sum = 0;
                for (int k = 0; k < VECTOR_SIZE; k++) //Perform the dot product (a single number), by traversing 2 rows of source_in1 and 2 (or 2 elements of in1 and 2)
                {
//lab2 optimisation:
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT min = 1 max = VECTOR_SIZE

                    // ap_uint<32> a = v1_local[i].range(32 * (k + 1) - 1, 32 * k);
                    a = v1_local[i].range(32 * (k + 1) - 1, 32 * k);
                    // ap_uint<32> b = v2_local[j].range(32 * (k + 1) - 1, 32 * k);
                    b = v2_local[j].range(32 * (k + 1) - 1, 32 * k);
                    sum += a * b;
                }
                //Write the result back to the corresponding position
                result_local[i].range(32 * (j + 1) - 1, 32 * j) = sum;
				//when k loop ends: 1 element of source_hw is written
				//when j loop ends: 1 row of source_hw (1 element of out[]) is written
				//when i loop ends: all matrix is written
            }
            out[i] = result_local[i]; //Store the final row of source_hw (or 1 element of result_local) into the output
        }
    }
}
