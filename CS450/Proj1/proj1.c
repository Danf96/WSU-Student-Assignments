//Daniel Fuchs
//Written in C
//CS 427
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
double rowReduction(double *row[30], int n);
void printDeterminantSmall(double *row[30], double detScalar, int n);

/*
*   Takes in a NXN matrix (max N is 30), then performs a row reduction and prints out the result to standard out
*/
int main (int argc, char *argv[]){
    char inputSizeStr[256];
    int inputSizeInt;
    char inputBuffer[1024];
    char * token;
    double detScalar;
    double matrix[30][30], *ptr[30]; //array of pointers for quick row swaps
    
    fgets(inputSizeStr, 256, stdin); //getting size of nxn matrix
    inputSizeInt = atoi(inputSizeStr);
    for(int i = 0; i < inputSizeInt; i++){ //filling matrix with values
        fgets(inputBuffer, 1024, stdin);
        token = strtok(inputBuffer, " ");
        matrix[i][0] = atof(token);
        for(int j = 1; j < inputSizeInt; j++){
            token = strtok(NULL, " ");
            matrix[i][j] = atof(token);
        }
        ptr[i] = &matrix[i][0];
    }
    detScalar = rowReduction(ptr, inputSizeInt); //perform row reduction on matrix
    printDeterminantSmall(ptr, detScalar, inputSizeInt);
    
}

/*
*   Row reduction based on wikipedia pseudocode: https://en.wikipedia.org/wiki/Row_echelon_form
*/
double rowReduction(double *row[30], int n){ //
    double * tempRow;
    double detScalar = 1; //to be used to account for doubling determinant and rows
    int allZeros;
    int i;
    for(i = 0; i < n; i++){ //initial check to see if there is a row of all zeroes
        allZeros = 1;
        for(int j = 0; j < n; j++){
            if (row[i][j] != 0){
                allZeros = 0;
            }
        }
        if (allZeros == 1){ //if any row is all zeros we know the determinant will be zero
            printf("%f", (double)0);
            exit(0);
        }
    }
    for(i = 0; i < n; i++){ //initial check to see if there is a column of all zeroes
        allZeros = 1;
        for(int j = 0; j < n; j++){
            if (row[j][i] != 0){
                allZeros = 0;
            }
        }
        if (allZeros == 1){ //if any column is all zeros we know the determinant will be zero
            printf("%f", (double)0);
            exit(0);
        }
    }
    int pivot = 0;
    double x;

    
    while(pivot < n){ //may be able to remove the p < n check but leaving it for now
        nextPivot:
            i = 1;
            while(row[pivot][pivot] == (double)0){ //tries to swap the pivot with a lower row to get a nonzero entry
                if(pivot + i >= n){  //restored this portion from pseudocode and flipped <= to >=
                    pivot++;
                    goto nextPivot;
                }
                //swap row pivot with row pivot + i ***MODIFY DET RESULT HERE*
                tempRow = row[pivot];
                row[pivot] = row[pivot + i];
                row[pivot + i] = tempRow;
                i++;
                detScalar = -detScalar;
            }
            for(i = 1; i < (n - pivot); i++){ //checks every element in the column below the pivot point and cancels out the elements in the column below the pivot point
                if (row[pivot + i][pivot] != (double)0){
                    x = (-row[pivot + i][pivot]) / row[pivot][pivot]; 

                    detScalar /= x;
                    for(int c = pivot; c < n; c++){
                        row[pivot][c] = row[pivot][c] * x;
                        row[pivot + i][c] = row[pivot][c] + row[pivot + i][c];
                    }
                }
            }
            pivot++;
    }
    return detScalar;    
}


/*
*   Multiplies all the diagonals then prints the result
*/
void printDeterminantSmall(double *row[30], double detScalar, int n){
    double result = 1;
    for(int i = 0; i < n; i++){
        for (int j = 0; j < n; j++){
            if(i == j){
                if (row[i][j] == 0){
                    printf("%f", (double)0);
                    exit(0);
                }
                result *= row[i][j];
                break;
            }
        }
    }
    printf("%f",(result * detScalar));
}
