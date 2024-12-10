#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include "C:\Users\Anna\AppData\Roaming\Xilinx\Vitis\matrixdimlab1.h"
//#define lm 6
//#define ln 6
//#define lp 6

//#define m (1 << lm)
//#define n (1 << ln)
//#define p (1 << lp)

void matrix_mul(uint8_t A[m][n], uint8_t B[n][p], uint32_t C[m][p]);

void sw_matrix_mul(uint8_t A[m][n], uint8_t B[n][p], uint32_t C[m][p]) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            uint32_t sum = 0;
            for (int k = 0; k < n; k++) {
                sum += (uint32_t)A[i][k] * (uint32_t)B[k][j];
            }
            C[i][j] = sum;
        }
    }
}

void print_matrix(int a, int b, uint8_t Mat[a][b]) {
    printf("\n");
    for (int i = 0; i < a; i++) {
        for (int j = 0; j < b; j++) {
            printf("%u\t", Mat[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void print_matrix_uint32(int a, int b, uint32_t Mat[a][b]) {
    printf("\n");
    for (int i = 0; i < a; i++) {
        for (int j = 0; j < b; j++) {
            printf("%u\t", Mat[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void check(int a, int b, uint32_t Mat1[a][b], uint32_t Mat2[a][b]) {
    int flag = 1;
    for (int i = 0; i < a; i++) {
        for (int j = 0; j < b; j++) {
            if (Mat1[i][j] != Mat2[i][j]) {
                flag = 0;
                break;
            }
        }
    }
    if (flag) {
        printf("\nTest Passed\n");
    } else {
        printf("\nTest Failed\n");
    }
}

int main() {
    uint8_t A[m][n], B[n][p];
    uint32_t C_sw[m][p], C_hw[m][p];
    int i;
    int j;

    //srand(time(NULL));

    for (i = 0; i < m; i++) {
        for (j = 0; j < n; j++) {
            A[i][j] = rand() % 256;
        }
    }
    printf("\nA matrix:\n");
    print_matrix(m, n, A);

    for (i = 0; i < n; i++) {
        for (j = 0; j < p; j++) {
            B[i][j] = rand() % 256;
        }
    }
    printf("\nB matrix:\n");
    print_matrix(n, p, B);

    matrix_mul(A, B, C_hw);
    sw_matrix_mul(A, B, C_sw);

    printf("\nC_hw matrix hardware emulation:\n");
    print_matrix_uint32(m, p, C_hw);

    printf("\nC_sw matrix software reference:\n");
    print_matrix_uint32(m, p, C_sw);

    check(m, p, C_sw, C_hw);

    return 0;
}

