#include "matrix.h"

#ifndef __CIRCUIT_H__
#define __CIRCUIT_H__
typedef struct {
    char type;
    int high;
    int low;
    double value;
} Component; 

typedef struct 
{
    Component** components;
    int countNodes;
    int countEdges;
    int countIntensitas;
    int countVoltage;
    int countResistor;
    int countCapacity;
    int countInductor;
    Matrix** answer;
} Circuit;
#endif