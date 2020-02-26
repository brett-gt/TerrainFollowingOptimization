/*********************************************************************
 cAnt_Move.cpp

 Extends the cAnt class.  This file contains functions required for
 incremental path creation.  

**********************************************************************/

#include "..\Include\cAnt.h"

/******************************************************************************
 int cAnt::makeMove(float alpha, float beta, ANT_STATE_TYPE *newState)

 Description:
	Moves the ant from its current position to a new position.  Each position
	move is selected from a list of valid possible moves (aircraft dynamic limits).

	Each candidate in the candidate list is graded and then a new position is
	selected using biased exploration.  

 Inputs:
	float alpha   -> exponent coefficient used to weight pheromone level
	float beta    -> exponent coefficient used to weight fuzzy ranking
	ANT_STATE_TYPE *newState -> pointer to location where new state is stored

 Outputs:
	Returns: int - 1 = found, 0 = new postion, -1 = no valid moves

******************************************************************************/
int cAnt::makeMove(float alpha, float beta, ANT_STATE_TYPE *newState)
{

	//-------------------------------------------------------------------------
	// If the target can be reached from current position then we are done, even
	// if this violates the flight dynamics
	//-------------------------------------------------------------------------
	if(checkTarget() == 1)
		return 1;

	getCandidateMoves(currentState);

	/*
		printf("\n\nIn makeMoves...\n\n");
		printf("CurrentState: [%i %i]:%i \n Candidates:",currentState.position[0],currentState.position[1], currentState.heading);
		for(int i=0;i<3;i++)
			printf("[%i]: [%i %i]:%i ", i, candidateStates[i].position[0],candidateStates[i].position[1], candidateStates[i].heading);
		printf("\n");
	*/

	int numCandidates = gradeCandidates();

	//If there were no valid moves return -1
	if(numCandidates == 0)
		return -1;
	
	/* Prints individual candidate grades
	for(int i=0;i<3;i++)
		printf("\nGrade[%i]: %f ", i, fuzzyRankings[i]);
	printf("\n\n");
	*/
		
	int bestPlace;
	calcProbabilities(&bestPlace, alpha, beta, numCandidates);

	/*
		for(int i=0;i<3;i++)
			printf("Prob[%i]: %f ", i, probabilities[i]);

		printf("BestPlace: %i \n\n", bestPlace);
	*/


	//---------------------------------------------
	// Run the probabilities to pick a new position
	//--------------------------------------------
	
	//Exploitation
	int p = rand() % 100;

	#ifdef _DEBUG_INDIVIDUAL_ANT_
		printf("Random Exploitation random %i \n", p);
	#endif	

	if(p < Po)
	{
		newState->position[LAT] = validCandidates[bestPlace].position[LAT];
		newState->position[LON] = validCandidates[bestPlace].position[LON];
		newState->heading = validCandidates[bestPlace].heading;

		//printf("Explotation taken, bestplace %i\n", bestPlace);
	}

	//Biased exploration
	else
	{
		float r = (float)rand()/(float)RAND_MAX;

		//printf("Biased Exploration random %f \n", r);

		float total = 0;
		for(int i=0;i<numCandidates;i++)
		{
			total = total + probabilities[i];

			if(total >= r)
			{
				newState->position[LAT] = validCandidates[i].position[LAT];
				newState->position[LON] = validCandidates[i].position[LON];
				newState->heading = validCandidates[i].heading;

				/*
					printf("Biased exploration taken at %i \n", i);
					printf("newPlace [%i %i]:%i\n", newState->position[0], newState->position[1], newState->heading);
				*/
				break;
			}
		}
	}

	return 0;
}


/******************************************************************************
 int cAnt::gradeCandidates(void)

 Description:
	Grade candidates first checks the candidate positions for altitude change.
	Any move violating the altitude limit is removed from the solution list.  

	The intervis and angle to target for valid moves are then normalized and
	applied to a fuzzy engine which creates a fuzzy grade for the solutions. The
	fuzzy grades are stored in global buffers for the class.

 Inputs:
	None

 Outputs:
	Returns: Number of valid moves (0 - LOOKAHEAD_NUM)

******************************************************************************/
int cAnt::gradeCandidates(void)
{
	//Predefine intermediate arrays that hold data
	int intervisRanking[LOOKAHEAD_NUM];
	float intervisNormalized[LOOKAHEAD_NUM];
	float step1[LOOKAHEAD_NUM];
	int angleToTarget[LOOKAHEAD_NUM];

	int intervisSum = 0;
	int numValid = 0;
	int currentAltitude = (int)worldMap.ReadFromMap(currentState.position[LAT], currentState.position[LON]);


	/*----------------------------------------------------------------------
	Loops through every candidate point.  
	
	First checks the altitude change to see if this violates the change limit. 
	If it fails the alt test then that candidate point is not used.  If all points
	fail altitude test then	there are no candidates and we have reached dead end.  

	Valid candidates are copied into a new buffer (validCandidates) and a running
	total of their intervis rankings / angle to target are calculated.  
	-----------------------------------------------------------------------*/
	for(int i=0;i<LOOKAHEAD_NUM;i++)
	{

		//Altitude check
		int newAltitude = (int)worldMap.ReadFromMap(candidateStates[i].position[LAT], candidateStates[i].position[LON]);
		if((int)abs(currentAltitude - newAltitude) < (int)ALTITUDE_CHANGE_LIMIT)
		{
			//Copy passed candidate over
			memcpy(&validCandidates[numValid],&candidateStates[i],sizeof(ANT_STATE_TYPE)); 

			//Calculate intervis 		
			intervisRanking[numValid] = maskingMap.ReadFromMap(candidateStates[i].position[LAT], candidateStates[i].position[LON]);
			intervisSum = intervisSum + intervisRanking[numValid];

			//Calculate angle
			float angleForCalc = GetAngleToTarget(candidateStates[i].position,targetState.position);
			float headForCalc;

			if(candidateStates[i].heading < 0)
				headForCalc = 360 + (float)candidateStates[i].heading;
			else
				headForCalc = (float)candidateStates[i].heading;
	
			if(angleForCalc < 0)
				angleForCalc = 360 + angleForCalc;
  
			float diff = abs(headForCalc - angleForCalc);
    
			if(diff > 180)
				diff = diff - 360;
 
			angleToTarget[numValid] =  (int)abs(diff);
			numValid++;
		}
	}

	//Kick out if there were not any valid moves
	if(numValid == 0)
		return 0;


	/*----------------------------------------------------------------------
	Current grading scheme:

	Intervis is normalized by first finding the average intervis grade.  
	The average is then	subtracted from each intervis grade (Step1).
	The result from step 1 is then divided by the maximum - minimum of step 1 points.
	The final result is a normalized intervis grade ranging from -1 to 1

	Angle to target is a value ranging from 0 (heading straight towards target) to
	180 degrees (heading straight away from target).

	These two inputs are applied to a fuzzy engine that combines the value to
	give a final fuzzy grade.
	-----------------------------------------------------------------------*/
	float intervisAverage = (float)intervisSum/numValid;

	for(int i=0;i<numValid;i++)
	{
		step1[i] = intervisRanking[i] - intervisAverage;
	}
	
	float denominator = FindArrayMax(step1, numValid) - FindArrayMin(step1,numValid);

	if(denominator == 0)
		denominator = 1;

	for(int i=0;i<numValid;i++)
	{
		intervisNormalized[i] = step1[i]/denominator;
	}
	

	//-------------------------
	// Perform fuzzy ranking
	//-------------------------
	for(int i=0;i<numValid;i++)
	{
		float inputs[2];
		inputs[0] = (float)angleToTarget[i] * PI/180;
		inputs[1] = intervisNormalized[i];

		//printf("FI[%i] = Angle %f   Intervis %f\n", i, inputs[0], inputs[1]);

		fuzzyRankings[i] = lookAheadHeuristic.Run(inputs,2);
	}
	
	return numValid;
}





/******************************************************************************
 int cAnt::checkTarget(void)

 Description:
	Checks the current position to see if the target can be reached (if it is 
	within sqrt(2) distance of the target).

 Inputs:
	None

 Outputs:
	Returns: 1 if target can be reached, 0 if not

******************************************************************************/
int cAnt::checkTarget(void)
{
	if((abs(currentState.position[LAT] - targetState.position[LAT]) <= 1) &&
	   (abs(currentState.position[LON] - targetState.position[LON]) <= 1))
	{
		return 1;
	}

	return 0;
}


/******************************************************************************
 int cAnt::getCandidateMoves(ANT_STATE_TYPE currentState)

 Description:
	Takes the current ant dynamics (location and heading) and creates a list of
    possible new positions.  After creating the list, the boundMove function is called
	to make sure the ant does not move outside of the map.

	As currently implemented, three candidate positions are created correlating to
	the ant continuing on its current heading or changing heading by +/- 45 degrees.

	TODO:	This function can be updated to include more candidates or change the dyanmics
	used to limit candidate positions.

 Inputs:
	ANT_STATE_TYPE currentState -> current ant position and heading

 Outputs:
	Returns: should always return 0, there is an error catch in the switch statement
	that returns -1 if for some reason the ant reaches an invalid heading.

******************************************************************************/
int cAnt::getCandidateMoves(ANT_STATE_TYPE currentState)
{
	switch(currentState.heading)
	{
		case 0:
			candidateStates[0].heading = 0;
			candidateStates[0].position[LAT] = currentState.position[LAT] ;
			candidateStates[0].position[LON] = currentState.position[LON] + 1;
	
			candidateStates[1].heading = 45;
			candidateStates[1].position[LAT] = currentState.position[LAT] + 1;
			candidateStates[1].position[LON] = currentState.position[LON] + 1;

			candidateStates[2].heading = -45;
			candidateStates[2].position[LAT] = currentState.position[LAT] - 1;
			candidateStates[2].position[LON] = currentState.position[LON] + 1;
			break;

		case 45:
			candidateStates[0].heading = 0;
			candidateStates[0].position[LAT] = currentState.position[LAT] ;
			candidateStates[0].position[LON] = currentState.position[LON] + 1;
	
			candidateStates[1].heading = 45;
			candidateStates[1].position[LAT] = currentState.position[LAT] + 1;
			candidateStates[1].position[LON] = currentState.position[LON] + 1;

			candidateStates[2].heading = 90;
			candidateStates[2].position[LAT] = currentState.position[LAT] + 1;
			candidateStates[2].position[LON] = currentState.position[LON];
			break;
       
		case 90:
			candidateStates[0].heading = 90;
			candidateStates[0].position[LAT] = currentState.position[LAT] + 1;
			candidateStates[0].position[LON] = currentState.position[LON];
	
			candidateStates[1].heading = 45;
			candidateStates[1].position[LAT] = currentState.position[LAT] + 1;
			candidateStates[1].position[LON] = currentState.position[LON] + 1;

			candidateStates[2].heading = 135;
			candidateStates[2].position[LAT] = currentState.position[LAT] + 1;
			candidateStates[2].position[LON] = currentState.position[LON] - 1;
			break;

		case 135:
			candidateStates[0].heading = 90;
			candidateStates[0].position[LAT] = currentState.position[LAT] + 1;
			candidateStates[0].position[LON] = currentState.position[LON];
	
			candidateStates[1].heading = 135;
			candidateStates[1].position[LAT] = currentState.position[LAT] + 1;
			candidateStates[1].position[LON] = currentState.position[LON] - 1;

			candidateStates[2].heading = 180;
			candidateStates[2].position[LAT] = currentState.position[LAT];
			candidateStates[2].position[LON] = currentState.position[LON] - 1;
			break;

		case 180:
			candidateStates[0].heading = -135;
			candidateStates[0].position[LAT] = currentState.position[LAT] - 1;
			candidateStates[0].position[LON] = currentState.position[LON] - 1;
	
			candidateStates[1].heading = 135;
			candidateStates[1].position[LAT] = currentState.position[LAT] + 1;
			candidateStates[1].position[LON] = currentState.position[LON] - 1;

			candidateStates[2].heading = 180;
			candidateStates[2].position[LAT] = currentState.position[LAT];
			candidateStates[2].position[LON] = currentState.position[LON] - 1;
			break;

		case -135:
			candidateStates[0].heading = -135;
			candidateStates[0].position[LAT] = currentState.position[LAT] - 1;
			candidateStates[0].position[LON] = currentState.position[LON] - 1;
	
			candidateStates[1].heading = -90;
			candidateStates[1].position[LAT] = currentState.position[LAT] - 1;
			candidateStates[1].position[LON] = currentState.position[LON];

			candidateStates[2].heading = 180;
			candidateStates[2].position[LAT] = currentState.position[LAT];
			candidateStates[2].position[LON] = currentState.position[LON] - 1;
			break;

		case -90:
			candidateStates[0].heading = -135;
			candidateStates[0].position[LAT] = currentState.position[LAT] - 1;
			candidateStates[0].position[LON] = currentState.position[LON] - 1;
	
			candidateStates[1].heading = -90;
			candidateStates[1].position[LAT] = currentState.position[LAT] - 1;
			candidateStates[1].position[LON] = currentState.position[LON];

			candidateStates[2].heading = -45;
			candidateStates[2].position[LAT] = currentState.position[LAT] - 1;
			candidateStates[2].position[LON] = currentState.position[LON] + 1;
			break;

		case -45:
			candidateStates[0].heading = 0;
			candidateStates[0].position[LAT] = currentState.position[LAT];
			candidateStates[0].position[LON] = currentState.position[LON] + 1;
	
			candidateStates[1].heading = -90;
			candidateStates[1].position[LAT] = currentState.position[LAT] - 1;
			candidateStates[1].position[LON] = currentState.position[LON];

			candidateStates[2].heading = -45;
			candidateStates[2].position[LAT] = currentState.position[LAT] - 1;
			candidateStates[2].position[LON] = currentState.position[LON] + 1;
			break;


		default:
			printf("Error in getCandidates: currentState.heading is %i\n",currentState.heading);
			candidateStates[0].heading = 0;
			candidateStates[0].position[LAT] = currentState.position[LAT];
			candidateStates[0].position[LON] = currentState.position[LON]  + 1;
	
			candidateStates[1].heading = 45;
			candidateStates[1].position[LAT] = currentState.position[LAT] + 1;
			candidateStates[1].position[LON] = currentState.position[LON] + 1;

			candidateStates[2].heading = -45;
			candidateStates[2].position[LAT] = currentState.position[LAT] - 1;
			candidateStates[2].position[LON] = currentState.position[LON] + 1;

			printf("Error in getCandidates\n");
			return -1;
		}
 
	for(int i=0;i<LOOKAHEAD_NUM;i++)
		boundMove(&candidateStates[i]);

	return 0;
}



/******************************************************************************
 int cAnt::boundMove(ANT_STATE_TYPE *currentPos)

 Description:
	Limits the passed position to be within the map.  If the position is outside
	the map, it is set to the map boundary and the heading is reversed.

 Inputs:
	ANT_STATE_TYPE *currentPos -> pointer to the position to be limited

 Outputs:
	Always returns 0

******************************************************************************/
int cAnt::boundMove(ANT_STATE_TYPE *currentPos)
{
	bool reverse = false;
	if(currentPos->position[LAT] < 1)
	{
		currentPos->position[LAT] = 1;
		reverse = true;
	}
	else if (currentPos->position[LAT] > maskingMap.mapColumns)
	{
		currentPos->position[LAT] = maskingMap.mapColumns;
		reverse = true;
	}

	if(currentPos->position[LON] < 1)
	{
		currentPos->position[LON] = 1;
		reverse = true;
	}
	else if (currentPos->position[LON] > maskingMap.mapRows)
	{
		currentPos->position[LON] = maskingMap.mapRows;
		reverse = true;
	}	

	//If we hit an edge, reverse the direction
	if(reverse)
	{
		if(currentPos->heading <= 0)
			currentPos->heading = currentPos->heading + 180;
		else
			currentPos->heading = currentPos->heading - 180;
	}

	return  0;
}



/******************************************************************************
 int cAnt::calcProbabilities(int *bestPlace, float alpha, float beta, int numCandidates)

 Description:
	Combines the fuzzy rankings with the pheromone level for the candidate position.
	Individual results are divided by the sum of the result to created a weighted probability
	for taking that move.

	Also searches and finds the best place and returns the index of that place in the array
	of solutions.

 Inputs:
	int *bestPlace    -> index return to where best solution is in the array of solutions
	float alpha       -> exponent coefficient used to weight pheromone level
	float beta        -> exponent coefficient used to weight fuzzy ranking
	int numCandidates -> number of valid solutions in the array of solutions

 Outputs:
	Returns: Always returns 0

******************************************************************************/
int cAnt::calcProbabilities(int *bestPlace, float alpha, float beta, int numCandidates)
{
	float sumRankings = 0.0f;
	float pheromoneLevel;

	float *totalRank = new float[numCandidates];

	for(int i=0;i<numCandidates;i++)
	{
		pheromoneLevel = pheromoneMap.ReadFromMap(validCandidates[i].position[LAT], validCandidates[i].position[LON]);
		totalRank[i] = pow(pheromoneLevel, alpha) * pow(fuzzyRankings[i],beta);
		sumRankings = sumRankings + totalRank[i];
	}

	/* Prints probability rolls
	printf("\n\n");
	for(int i=0;i<LOOKAHEAD_NUM;i++)
		printf("TotalRank[%i] %f \n",i, totalRank[i]);

	printf("SumRanking %f \n", sumRankings);
	*/

	float bestResult = 0;
	for(int i=0;i<numCandidates;i++)
	{
		probabilities[i] = totalRank[i]/sumRankings;

		if(totalRank[i] > bestResult)
		{
			*bestPlace = i;
			bestResult = totalRank[i];
		}
	}

	delete[] totalRank;

	return 0;
}
    


  