//#include <algorithm>
//#include <vector>
//#include <cstdlib>
//#include <iostream>
//#include <cstdint>

#define lm 4
#define ln 4
#define lp 4

#define m (1 << lm)
#define n (1 << ln)
#define p (1 << lp)

//void matrix_mul(uint32_t A[m][n], uint32_t B[n][p], uint32_t C[m][p]) {
extern "C" {
void vadd(unsigned int* in1, // Read-Only Vector 1 (1D array)
          unsigned int* in2, // Read-Only Vector 2 (1D array)
          unsigned int* out_r, // Output Result (1D array)
          int size) { // Size in integer

#pragma HLS INTERFACE m_axi port = in1 offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = in2 offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = out_r offset = slave bundle = gmem
#pragma HLS INTERFACE s_axilite port = in1 bundle = control
#pragma HLS INTERFACE s_axilite port = in2 bundle = control
#pragma HLS INTERFACE s_axilite port = out_r bundle = control
#pragma HLS INTERFACE s_axilite port = size bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control

    #pragma HLS ARRAY_PARTITION variable=in1 type=cyclic factor=64 dim=1
    #pragma HLS ARRAY_PARTITION variable=in2 type=cyclic factor=64 dim=1
//#pragma HLS ARRAY_PARTITION variable=C type=cyclic factor=16 dim=0
	//out_r[1 + ]=2;
	int i;
	int j;
	int k;
	unsigned int  sum;

    for (i = 0; i < m; i++) {
        #pragma HLS LOOP_TRIPCOUNT min=m max=m
        for (j = 0; j < p; j++) {
            #pragma HLS LOOP_TRIPCOUNT min=p max=p
            #pragma HLS PIPELINE II=1
            sum = 0;
            for (k = 0; k < n; k++) {
                #pragma HLS LOOP_TRIPCOUNT min=n max=n
            	//#pragma HLS UNROLL factor=64
                sum += in1[i * n + k] * in2[k * p + j];
            }
            out_r[i * p + j] = sum;
        }
    }
}
}


