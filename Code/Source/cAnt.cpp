
/*********************************************************************
 cAnt.cpp

 Implements the ant class.  Once created, FormSolution can be called to 
 cause the ant to attempt path creation between the supplied start and 
 end point.  The results are stored as part of the ant class and accessed
 after the solution has been formed.

 Remember that location is usually stated (lat, lon) but this represents
 (y, x) if thinking in conventional sense.

 Additional ant class functions are found in cAnt_Move.cpp

**********************************************************************/

#include "..\Include\cAnt.h"


/******************************************************************************
 cAnt::cAnt()

 Class initializer.  Sets number of moves and exploitation (Po) to zero.
 Also preallocated memory for the trimmedPath and rawPath used for the ant.  
 Both paths are allocated to maximum size for speed (single shot allocation).

******************************************************************************/
cAnt::cAnt()
{
	Po = 0;
	numMoves = 0;
	
	//TODO: Do I need destructor to deallocate?
	trimmedPath = (ANT_STATE_TYPE *)malloc(_ANT_PATH_SIZE_);
	rawPath     = (ANT_STATE_TYPE *)malloc(_ANT_PATH_SIZE_);
}




/******************************************************************************
 int cAnt::FormSolution(int start[2], int end[2])

 Description:
	Forms a solution for the ent using the passed (lat,lon) start and end point.

 Inputs:
	int start[2] -> (lon,lat) of start waypoint
	int end[2]   -> (lon,lat) of end waypoint

 Outputs:
	Return of 0 means ant was uncessful in finding a solution
	Return of 1 means ant finished soltuion successfully

******************************************************************************/
int cAnt::FormSolution(int start[2], int end[2])
{
	//------------------------
	// Setup state variables 
	//------------------------
	pathGrade = 0;
	pathLength = 0;
	numMoves = 0;

	startState.position[LAT] = start[LAT];
	startState.position[LON] = start[LON];
	startState.heading = snapAngle(GetAngleToTarget(start, end));

	targetState.position[LAT] = end[LAT];
	targetState.position[LON] = end[LON];


	//Assign starting values
	currentState = startState;
	rawPath[0] = startState;

	int found = 0;

	while(numMoves < MAX_PATH_SIZE)
	{
		ANT_STATE_TYPE newState;

		//Requirement: makeMove function returns: -1 deadend, 0 not done, 1 found
		found = makeMove(ALPHA, BETA, &newState);

		if(found == 1) //found the target
			return 1;
		else if (found == -1) //deadend
			return 0;

		rawPath[numMoves] = newState;
		currentState = newState;
		
		/* Dont need to bother grading this one because of trim function
		pathGrade += maskingMap.ReadFromMap(currentState.position[LAT], currentState.position[LON]);
		if(currentState.heading % 90 == 0)
			pathLength += 1.0f;
		else
			pathLength += 1.414f;
		*/

		numMoves++;
	}

	return 0;
}



/******************************************************************************
 void cAnt::TrimPath(void)

 Description:
	THe trim path algorithm looks for loops in the constructed solution and
	removes them.  Calculates path grade of resulting solution.
	
	Assumes that a path has already been created.

	Note:  This can create discontinuties in the final path.

 Inputs:
	None

 Outputs:
	None

******************************************************************************/
void cAnt::TrimPath(void)
{

	rawMoves = numMoves;

	trimmedPath[0] = rawPath[0];

	pathGrade = maskingMap.ReadFromMap(trimmedPath[0].position[LAT], trimmedPath[0].position[LON]);

	numMoves = 0;
	int i = 0;

	// Looks for point uniqueness in the solution, if the point is not unique then the last one is
	// used and path between first and last is removed (everything between first appearance and last
	// appearance was a loop)
	while(i < rawMoves)
	{
		for(int j=i+1;j<rawMoves;j++)
		{
			if(rawPath[i].position[LAT] == rawPath[j].position[LAT] &&
			   rawPath[i].position[LON] == rawPath[j].position[LON])
			{
				i = j;
			}
		}

		trimmedPath[numMoves] = rawPath[i];

		pathGrade += maskingMap.ReadFromMap(trimmedPath[numMoves].position[LAT], trimmedPath[numMoves].position[LON]);

		/*
		if(trimmedPath[numMoves].heading % 90 == 0)
			pathLength += 1.0f;
		else
			pathLength += 1.414f;
		*/


		i++;
		numMoves++;
	}

	pathLength = numMoves;
	
	pathGrade = pathGrade/pathLength;

	#ifdef _PRINT_TRIM_RESULTS_
		printf("Before trim size %i, after trim size %i\n", rawMoves, numMoves);
	#endif
}




/******************************************************************************
 void cAnt::SetExploitationLevel(int level)

 Description:
	Access function allowing exploitation level of ant to be adjusted.

 Inputs:
	int level -> new explotation level

 Outputs:
	None
******************************************************************************/
void cAnt::SetExploitationLevel(int level)
{
	Po = level;
}



/******************************************************************************
 void cAnt::PrintPath(bool printRaw)

 Description:
	Debug function that prints the ant path to the screen.

 Inputs:
	bool printRaw -> true = print raw path, false = print trimmed path

 Outputs:
	None
******************************************************************************/
void cAnt::PrintPath(bool printRaw)
{
	int rowCount = 0;

	if(printRaw)
	{
		for(int i=0;i<rawMoves;i++)
		{
			printf("[%i, %i] ", rawPath[i].position[LAT], rawPath[i].position[LON]);

			if(rowCount == 5)
			{
				rowCount = 0;
				printf("\n");
			}
			else
				rowCount++;
		}
	}
	else
	{
		for(int i=0;i<numMoves;i++)
		{
			printf("[%i, %i] ", trimmedPath[i].position[LAT], trimmedPath[i].position[LON]);

			if(rowCount == 5)
			{
				rowCount = 0;
				printf("\n");
			}
			else
				rowCount++;
		}
	}
}



/******************************************************************************
 int cAnt::WritePathToFile(const char* filename, bool printRaw)

 Description:
	Saves the ant path to a csv file (used to import results into MATLAB for plotting)

 Inputs:
    const char* filename -> filename to be used for ant path
	bool printRaw        -> true = print raw path, false = print trimmed path

 Outputs:
	return 0 indicated file problem, return 1 is success
******************************************************************************/
int cAnt::WritePathToFile(const char* filename, bool printRaw)
{
	FILE *write_fp = fopen(filename, "w");

	if (write_fp == NULL)
	{
		fprintf(stderr, "Error WriteAntPath : unable to open input file %s\n", filename);
		return 0;
	}

	if(printRaw)
	{
		for(int i = 0;i < rawMoves; i++)
		{
			fprintf(write_fp,"%i, %i\n", rawPath[i].position[LON],rawPath[i].position[LAT]); 
		}
	}
	else
	{
		for(int i = 0;i < numMoves; i++)
		{
			fprintf(write_fp,"%i, %i\n", trimmedPath[i].position[LON],trimmedPath[i].position[LAT]); 
		}
	}
	
	fclose(write_fp); 
	return 1;
}



/******************************************************************************
 int cAnt::FindLargestAltitudeChange(void)

 Description:
	Searches through the solution to examine altitude change between points.

 Inputs:
    None

 Outputs:
	Returns largest altitude change
******************************************************************************/
int cAnt::FindLargestAltitudeChange(void)
{
	int largestChange = 0;
	int rawChange = 0;
	for(int i =0;i<numMoves-1;i++)
	{
	
		int currentChange =	(int)worldMap.ReadFromMap(trimmedPath[i].position[LAT],trimmedPath[i].position[LON]) -
							(int)worldMap.ReadFromMap(trimmedPath[i+1].position[LAT],trimmedPath[i+1].position[LON]);

		if(abs(currentChange) > largestChange)
			largestChange = abs(currentChange);

		/*
		printf("%i Current altitude %i, next altitude %i, delta %i \n", i, 
			(int)worldMap.ReadFromMap(trimmedPath[i].position[LAT],trimmedPath[i].position[LON]), 
			(int)worldMap.ReadFromMap(trimmedPath[i+1].position[LAT],trimmedPath[i+1].position[LON]),
			currentChange);		
			*/
	}

	for(int i=0;i<rawMoves-1;i++)
	{
		int currentRawChange = (int)worldMap.ReadFromMap(rawPath[i].position[LAT],rawPath[i].position[LON]) -
							   (int)worldMap.ReadFromMap(rawPath[i+1].position[LAT],rawPath[i+1].position[LON]);
		if(abs(currentRawChange) > rawChange)
			rawChange = abs(currentRawChange);
	}
	
	printf("Largest altitude change-> trimmed: %i   raw: %i\n", largestChange, rawChange);
	return largestChange;
}