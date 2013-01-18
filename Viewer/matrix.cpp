#include "../include/common.h"
#include <math.h>
#include <stdio.h>

void print_matrix(float *A, int order)
{
    for (int i = 0; i < order*order; i++) {
        if (i % order == 0) printf("\n");
        printf(" %f ", A[i]);
    }
    printf("\n");
    
}


// Returns the Matrix with column c and row r removed
void minor_matrix(float *A, int width, int size, int r, int c, float *res)
{
    int row, col, resi = 0;
    
    for (int i = 0; i < size; i++) {
        row = i / width;
        col = i - (row * width);
        
        if ( (row != r) && (col != c) )
            res[resi++] = A[i];
        
    }
}


float Det(float *A, int order)
{
    int size = order * order;
    float det = 0, res[size - (order * 2) + 1];
    
    // Base Case
    if (order == 2) {
        return A[0]*A[3] - A[2]*A[1];
    }
    
    // Expand along the 0th row 
    for (int i = 0; i<order; i++) {
        minor_matrix(A, order, size, 0, i, res);
        det += powf(-1, 0 + i) * Det(res, order - 1) * A[i];
        
    }
    return det;
}

// Return the Cofactor matrix for a given Square matrix A
void Cofactor_Matrix(float *A, int size, int width, float *res)
{
    int row, col;
    
    float tmp[size - (width * 2) + 1];
    int   minor_order = width - 1;
    
    for (int i=0; i < size; i++) {
        row = i / width;
        col = i - (row * width);
        
        minor_matrix(A, width, size, row, col, tmp);
        res[i] = powf(-1, row + col) * Det(tmp, minor_order);
        //printf("row %d col %d %d\n", row, col, i);
        
    }
    
}

// Transposes a Matrix in place in
// O(n/2) where n is the total size
void Transpose(float *A, int order)
{
    float tmp;
    int index = -1;
    
    for (int start=0; start < order; start++) {
        for (int i = start; i <= (start + ( order * (order -1) ) ); i+= order) {
            index++;
            if ( index >= i) continue;
            tmp = A[index];
            A[index] = A[i];
            A[i] = tmp;
        }
    }
}



// Return in inverse the inverse matrix of a square matrix A 
void My_Matrix_Inverse(float *A, int size, int order, float *inverse)
{
    float det = Det(A, order);
    
    Cofactor_Matrix(A, size, order, inverse);
    Transpose(inverse, order);
    for (int i = 0; i < size; i++) 
        inverse[i] *= 1/det;
    
}

/*int main() 
{
    float A[] = {1,-2,3,0, -1,1,0,2, 0,2,0,3, 3,4,0,-2};
    //float A[] = {1,3,1, 1,1,2, 2,3,4};
    //float A[] = {0,1,1,1, 2,0,1,1, 2,2,0,1, 2,2,2,0};
    //float A[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
    //float A[] = {1,2,3,4,5,6,7,8,9};
    //float A[] = {0,2,1, 3,-1,2, 4,0,1};
    //float A[] = {1, 2, 3, 4};
    //float A[] = {1,2,3,4, 0,1,0,1, 2,0,2,0, 3,1,3,1};
    float res[16];
    
    //printf("Det %f\n", Det(A, 3));
    
    //minor_matrix(A, 4, 16, 0, 3, res);
    My_Matrix_Inverse(A, 16, 4, res);
    print_matrix(res, 4);
    //Cofactor_Matrix(A, 9, 3, res);
    //print_matrix(res, 3);
    
   // print_matrix(A,4);
   // Transpose(A, 4);
   // print_matrix(A, 4);
    
    return 0;
}*/