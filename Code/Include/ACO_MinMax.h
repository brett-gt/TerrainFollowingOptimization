
#ifndef _MINMAX_H_
#define _MINMAX_H_

#include "cAnt.h"
#include <sstream>

void MinMax(int start[2], int end[2]);

void GradePaths(void);

void GetTopAnts(float fuzzyRankings[], int indexArray[]);

void GlobalPheromoneUpdate(float pheromoneLevel, cAnt *theAnt);

void AddBestAnt(void);

void SaveBestAnt(int bestAntPosition);

#endif