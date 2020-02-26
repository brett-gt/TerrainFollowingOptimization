
#ifndef _GLOBAL_DATA_H_
#define _GLOBAL_DATA_H_

#include "cMap.h"
#include "cAnt.h"
#include "cFuzzyInterface.h"

//-----------------------------------

#define LAT 1
#define LON 0

//-----------------------------------
extern cMap worldMap;
extern cMap pheromoneMap;
extern cMap maskingMap;

extern cFuzzyInterface lookAheadHeuristic;
extern cFuzzyInterface pheromoneHeuristic;

#endif