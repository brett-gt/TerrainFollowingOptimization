
#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_

//---------------------------------------------
//#define _DEBUG_INDIVIDUAL_ANT_
//#define _DEBUG_PROGRESS_
//#define _DEBUG_TOP_ANTS_
//#define _PRINT_TRIM_RESULTS_
//----------------------------------------------

//------------------------------
// Algorithm Parameters
//------------------------------
#define NUM_ITERATIONS   1000  //number of iterations of the ant algorithm to run
#define NUM_ANTS         50   //number of ants per interation
#define NUM_TO_UPDATE    5    //number of paths to apply pheromone to
#define PATH_GAIN        0.5f //pheromone gain for updated path

#define INITIAL_Po   20       
#define MAX_Po       95
#define INCREMENT_Po 1

#define DISSIPATION_FACTOR  0.9f    //pheromone dissipation factor
#define PHEROMONE_MAX       10.0f   //max pheromone on map
#define PHEROMONE_MIN       1.0f    //min pheromone on map


//------------------------------
// Individual ant parameters
//------------------------------
#define MAX_PATH_SIZE    400       //max path size (quit trying on ant after this)
#define ALTITUDE_CHANGE_LIMIT  300 //altitude change limit in feet
#define LOOKAHEAD_NUM    3         //This is how many places are examined every move

#define ALPHA   1.0f          //exponential coefficient for pheromone
#define BETA    2.0f          //exponential coefficient for path grade

#endif