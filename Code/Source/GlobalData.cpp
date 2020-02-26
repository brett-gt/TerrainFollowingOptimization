
#include "..\Include\GlobalData.h"


//----------------------------------
cMap worldMap;
cMap pheromoneMap;
cMap maskingMap;


cFuzzyInterface lookAheadHeuristic("FuzzyLogic\\local.fcl");
cFuzzyInterface pheromoneHeuristic("FuzzyLogic\\pheromone.fcl");


