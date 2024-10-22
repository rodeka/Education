#include <stdlib.h>
#include <stdio.h>
#include "matrix.h"
#include "circuit.h"

void initCircuit(Circuit* circuit) {
    circuit->components = NULL;
    circuit->countIntensitas = 0;
    circuit->countVoltage = 0;
    circuit->countResistor = 0;
    circuit->countCapacity = 0;
    circuit->countInductor = 0;
    circuit->answer = malloc(2 * sizeof(Matrix*));
}

void parseFile(Circuit* circuit, char* fileName){
    FILE* file;
    file = fopen(fileName, "r");
    fscanf(file, "%d %d\n", &circuit->countNodes, &circuit->countEdges);
    circuit->components = malloc(circuit->countEdges * sizeof(Component*));

    char type;
    int high, low;
    double value;
    for (int i = 0; i < circuit->countEdges; ++i) {
        circuit->components[i] = malloc(sizeof(Component*));
        fscanf(file, "%c %d %d %lf\n", &type, &high, &low, &value);
        circuit->components[i]->type = type;
        circuit->components[i]->high = high;
        circuit->components[i]->low = low;
        circuit->components[i]->value = value;
        switch (type)
        {
        case 'V':
            circuit->countVoltage++;
            break;
        case 'I':
            circuit->countIntensitas++;
            break;
        case 'R':
            circuit->countResistor++;
            break;
        case 'C':
            circuit->countCapacity++;
            break;
        case 'L':
            circuit->countInductor++;
            break;
        }   
    }
    return;
}

Matrix* initMatrix(int size_i, int size_j){
    Matrix* matrix = malloc(sizeof(Matrix));
    matrix->i = size_i;
    matrix->j = size_j;
    matrix->M = malloc(size_i * sizeof(double*));
    for (int i = 0; i < size_i; ++i) {
        matrix->M[i] = malloc(size_j * sizeof(double));
        for (int j = 0; j < size_j; ++j){
            matrix->M[i][j] = 0.0;
        }
    }
    return matrix;
}

void freeMatrix(Matrix* matrix) {
    for(int i=0; i < matrix->i; i++){
        free(matrix->M[i]);
    }
    free(matrix->M);
    free(matrix);
    return;
}

void printMatrix(Matrix* A){
    for (int i=0; i < A->i; ++i){
        for (int j = 0; j < A->j; ++j){
            printf("%.3lf ", A->M[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    return;
}

void calculateMatrixs(Circuit* circuit){
    int matrixSize = circuit->countNodes + circuit->countVoltage - 1;
    int g2Count = circuit->countVoltage + circuit-> countInductor;
    printf("matrix:%d\ng2:%d\n", matrixSize, g2Count);
    Matrix* A = initMatrix(matrixSize, matrixSize);
    Matrix* b = initMatrix(matrixSize, 1);
    int g2Index = matrixSize - g2Count;
    for(int i = 0; i < circuit->countEdges; ++i){
        int high = circuit->components[i]->high;
        int low = circuit->components[i]->low;
        double value = circuit->components[i]->value;
        if(circuit->components[i]->type == 'R'){
            if(high != 0)
                A->M[high - 1][high - 1] += 1 / value;
            if(low != 0)
                A->M[low - 1][low - 1] += 1 / value;
            
            if(high != 0 && low != 0){
                A->M[high - 1][low - 1] -= 1 / value;
                A->M[low - 1][high - 1] -= 1 / value;
            }
        }
        
        // Для C не контур
        // if(circuit->components[i]->type == 'C')

        if(circuit->components[i]->type == 'L'){
            if(high != 0){
                A->M[high - 1][g2Index] += 1;
                A->M[g2Index][high - 1] += 1;
            }
            if(low != 0){
                A->M[low - 1][g2Index] -= 1;
                A->M[g2Index][low - 1] -= 1;
            }
            b->M[g2Index][0] = 0;
            g2Index += 1;
        }

        if(circuit->components[i]->type == 'V'){
            if(high != 0){
                A->M[high - 1][g2Index] += 1;
                A->M[g2Index][high - 1] += 1;
            }
            if(low != 0){
                A->M[low - 1][g2Index] -= 1;
                A->M[g2Index][low - 1] -= 1;
            }
            b->M[g2Index][0] = value;
            g2Index += 1;
        }

        if(circuit->components[i]->type == 'I'){
            if(high != 0){
                b->M[high - 1][0] -= value;
            }
            if(low != 0){
                b->M[low - 1][0] += value;
            }
        }
    }
    circuit->answer[0] = A;
    circuit->answer[1] = b;
    return;
}

void freeAll(Circuit* circuit){
    for(int i = 0; i < circuit->countEdges; i++){
        free(circuit->components[i]);
    }
    free(circuit->components);
    freeMatrix(circuit->answer[0]);
    freeMatrix(circuit->answer[1]);
    free(circuit->answer);
    free(circuit);
}

void printSLAU(Circuit* circuit){
    Matrix* A = circuit->answer[0];
    Matrix* b = circuit->answer[1];
    printMatrix(A);
    printMatrix(b);
    int size = A->i;
    for(int i = 0; i < size; ++i){
        for(int j = 0; j < size; ++j){
            if(A->M[i][j] != 0.0)
                printf("%.3f*p%d + ", A->M[i][j], j);
        }
        printf("= %.3f\n", b->M[i][0]);
    }
}

int main(){
    Circuit* circuit = (Circuit*)malloc(sizeof(Circuit));
    initCircuit(circuit);
    parseFile(circuit, "test.txt");
    calculateMatrixs(circuit);
    printSLAU(circuit);
    freeAll(circuit);
    return 0;
}