
#ifndef _MISC_FUNCTIONS_H_
#define _MISC_FUNCTIONS_H_

#include "cAnt.h"
#include "GlobalData.h"
#include <stdio.h>
#include <math.h>


#define PI 3.141593f


float FindArrayMin(float arrayPtr[], int arraySize);
float FindArrayMax(float arrayPtr[], int arraySize);

int snapAngle(float angleIn);
float GetAngleToTarget(int currentPoint[2], int targetPoint[2]);

#endif