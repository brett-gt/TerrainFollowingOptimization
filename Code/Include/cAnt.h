/*********************************************************************
 cAnt.ch

 Implements the ant class.  Header covers cAnt.cpp and cAnt_Move.cpp

**********************************************************************/

#ifndef _C_ANT_H_
#define _C_ANT_H_

#include <stdlib.h>
#include <math.h>
#include <ctime>

#include "Parameters.h"
#include "GlobalData.h"
#include "MiscFunctions.h"

//---------------------------
// Defines
//---------------------------

#define _ANT_PATH_SIZE_  sizeof(ANT_STATE_TYPE)*MAX_PATH_SIZE


struct ANT_STATE_TYPE
{
	int position[2];
	int heading;
};


class cAnt
{

	public:
		cAnt();
	
		int FormSolution(int start[2], int end[2]);
		void SetExploitationLevel(int level);
		void PrintPath(bool printRaw=false);
		int WritePathToFile(const char* filename, bool printRaw=false);
		void TrimPath(void);
		int FindLargestAltitudeChange(void);

		ANT_STATE_TYPE startState;
		ANT_STATE_TYPE currentState;
		ANT_STATE_TYPE targetState;

		ANT_STATE_TYPE    *rawPath;
		ANT_STATE_TYPE    *trimmedPath;

		float pathGrade;
		float pathLength;
		int numMoves;
		int rawMoves;

	private:
		//-------------------------------
		// Data
		//-------------------------------
		ANT_STATE_TYPE    candidateStates[LOOKAHEAD_NUM];
		ANT_STATE_TYPE	  validCandidates[LOOKAHEAD_NUM]; //holds candidates that pass the altitude test
		float             fuzzyRankings[LOOKAHEAD_NUM];
		float             probabilities[LOOKAHEAD_NUM];
		int               Po;


		//------------------------------
		// Functions
		//------------------------------
	


		//These functions are found in cAnt_Move.cpp
		int makeMove(float alpha, float beta, ANT_STATE_TYPE *newState);
		int getCandidateMoves(ANT_STATE_TYPE currentState);
		int gradeCandidates(void);
		int checkTarget(void);
		int calcProbabilities(int *bestPlace, float alpha, float beta, int numCandidates);
		int boundMove(ANT_STATE_TYPE *position);

};

#endif