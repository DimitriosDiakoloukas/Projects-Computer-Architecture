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

        uint512_dt v1_local[BUFFER_SIZE];
        uint512_dt v2_local[BUFFER_SIZE];
        uint512_dt result_local[BUFFER_SIZE];

        for (int i = 0; i < BUFFER_SIZE; i++) // Iterate over rows of Matrix 1
        {
#pragma HLS PIPELINE
            // Load a row of Matrix 1 into local memory
            v1_local[i] = in1[i];
        }

        for (int j = 0; j < BUFFER_SIZE; j++) // Iterate over columns of Matrix 2
        {
#pragma HLS PIPELINE
            // Load a column of Matrix 2 into local memory
            v2_local[j] = in2[j];
        }

        for (int i = 0; i < BUFFER_SIZE; i++) // Iterate over rows of Matrix 1
        {
            for (int j = 0; j < BUFFER_SIZE; j++) // Iterate over columns of Matrix 2
            {
                ap_uint<32> sum = 0;
                for (int k = 0; k < VECTOR_SIZE; k++) // Perform the dot product
                {
#pragma HLS UNROLL
                    ap_uint<32> a = v1_local[i].range(32 * (k + 1) - 1, 32 * k);
                    ap_uint<32> b = v2_local[j].range(32 * (k + 1) - 1, 32 * k);
                    sum += a * b;
                }
                // Write the result back to the corresponding position
                result_local[i].range(32 * (j + 1) - 1, 32 * j) = sum;
            }
            out[i] = result_local[i]; // Store the final row into the output
        }
    }
}

