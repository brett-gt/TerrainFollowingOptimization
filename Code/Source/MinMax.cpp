
#include "..\Include\ACO_MinMax.h"

//------------------------------------------------
// Global
//------------------------------------------------
int numThatMadeIt = 10;
cAnt bestAnt;

cAnt ants[NUM_ANTS+1]; //ant array for ants used every iteration

//plus one is to make room for the best ant
float antGrade[NUM_ANTS + 1];
float antLength[NUM_ANTS + 1];
int foundAntIndex[NUM_ANTS + 1];

int totalGrade = 0;

bool firstTime = true;

float globalMaxPathGrade = -1000.0f;
int maxGradePosition = 0;

float globalMinPathGrade = 1000000.0f;
int minGradePosition = 0;

float idealDistance;


//------------------------------------------------

void MinMax(int start[2], int end[2])
{
	//Initialize randomization
	srand(time(0));

	short flipFlop = 0;

	int currentPo = INITIAL_Po;
	idealDistance = sqrt(((float)end[LAT]-(float)start[LAT])*((float)end[LAT]-(float)start[LAT]) 
		               + ((float)end[LON]-(float)start[LON])*((float)end[LON]-(float)start[LON]));


	for(int i=0;i<NUM_ITERATIONS;i++)
	{
		#ifdef _DEBUG_PROGRESS_
			printf("\n\nIteration: %i \n", i);
		#endif

		numThatMadeIt = 0;

		//Run all the ants
		for(int j=0;j<NUM_ANTS;j++)
		{
			/*
			printf("\n\n\n###############################################\n");
			printf("\n\nRunning Ant %i \n", j);
			*/
			
			int result;
			if(flipFlop == 0)
			{
				result = ants[j].FormSolution(start, end);
				flipFlop = 1;
			}
			else
			{
				result = ants[j].FormSolution(end, start);
				flipFlop = 0;
			}
		
			//Check found
			if(result == 1)
			{
				ants[j].TrimPath();
				
				#ifdef _DEBUG_PROGRESS_
					printf("Ant %i found Target in %i moves\n", j, ants[j].numMoves);
					//ants[j].PrintPath();
				#endif
				
				foundAntIndex[numThatMadeIt] = j;

				//totalGrade = totalGrade + ants[j].pathGrade;

				//Identify min/max
				if(ants[j].pathGrade > globalMaxPathGrade)
				{
					globalMaxPathGrade = ants[j].pathGrade;
					maxGradePosition = j;
				}
				if (ants[j].pathGrade < globalMinPathGrade)
				{
					globalMinPathGrade = ants[j].pathGrade;
					minGradePosition = j;
				}

				numThatMadeIt++;
			}
		}

		//------------------------------------
		// Add in the best ant
		//------------------------------------
		AddBestAnt();

		//------------------------------------
		// Grade the ants and update pheromone
		//------------------------------------
		GradePaths();


		//Increment percent take best
		if(currentPo < MAX_Po)
			currentPo = currentPo + INCREMENT_Po;

		if(i%20 == 0)
			printf("Done with iteration %i\n",i);
	}

	printf("\n-------------------------------------\n");
	printf("MinMax Algorithm Complete.\n");
	printf("Largest altitude change for best ant is %i\n", bestAnt.FindLargestAltitudeChange());

	pheromoneMap.WriteMapToFile("pheromoneMap.csv");
	bestAnt.WritePathToFile("bestAntTrimmed.csv",false);
	bestAnt.WritePathToFile("bestAntRaw.csv",true);
}

//---------------------------------------------
// Add in the best ant
//---------------------------------------------
void AddBestAnt(void)
{
	if(!firstTime)
	{
		ants[NUM_ANTS] = bestAnt;
		foundAntIndex[numThatMadeIt] = NUM_ANTS;
		numThatMadeIt++;

		//Identify min/max
		if(ants[NUM_ANTS].pathGrade > globalMaxPathGrade)
		{
			globalMaxPathGrade = ants[NUM_ANTS].pathGrade;
			maxGradePosition = NUM_ANTS;
		}
		if (ants[NUM_ANTS].pathGrade < globalMinPathGrade)
		{
			globalMinPathGrade = ants[NUM_ANTS].pathGrade;
			minGradePosition = NUM_ANTS;
		}
	}
	else
		firstTime = false;

}


//---------------------------------------------
// Save the best ant
//---------------------------------------------
void SaveBestAnt(int bestAntPosition)
{
	if(bestAntPosition != NUM_ANTS)
	{
		printf("New best ant-> Position:%i, Grade:%f, Length:%f\n",
			bestAntPosition, ants[bestAntPosition].pathGrade, ants[bestAntPosition].pathLength);
	}

	bestAnt.numMoves = ants[bestAntPosition].numMoves;
	bestAnt.rawMoves = ants[bestAntPosition].rawMoves;
	bestAnt.pathGrade = ants[bestAntPosition].pathGrade;
	bestAnt.pathLength = ants[bestAntPosition].pathLength;

	memcpy(bestAnt.rawPath,ants[bestAntPosition].rawPath,_ANT_PATH_SIZE_);
	memcpy(bestAnt.trimmedPath,ants[bestAntPosition].trimmedPath,_ANT_PATH_SIZE_);
}


//---------------------------------------------
void GradePaths(void)
{
	/*
	printf("\n\n---------------------------------------------------\n");
	printf("\nGrading iteration: numThatMadeIt %i \n", numThatMadeIt);
	*/
	
	if(numThatMadeIt > 0)
	{
		float *fuzzyRankings = new float[numThatMadeIt];
		int   *indexArray    = new int[numThatMadeIt];

		//These become default values if only 1 happened to make it
		if(numThatMadeIt == 1)
		{
			fuzzyRankings[0] = 1;
			indexArray[0] = 0;
		}
		else
		{
			float maxLowered = globalMaxPathGrade - globalMinPathGrade;
			

			//int count = 0;
			for(int i=0;i<numThatMadeIt;i++)
			{
				//----------------------
				// Normalization
				//----------------------
				float intervisNormalized = (ants[foundAntIndex[i]].pathGrade - globalMinPathGrade)/maxLowered;
				float movesNormalized = (ants[foundAntIndex[i]].pathLength - idealDistance) / idealDistance;

				float inputs[2];
				inputs[0] = intervisNormalized;
				inputs[1] = movesNormalized;

				// Prints out fuzzy inputs
				#ifdef _DEBUG_INDIVIDUAL_ANT_
					printf("Pheromone FI[%i] = Angle %f   Intervis %f\n", i, inputs[0], inputs[1]);
				#endif

				fuzzyRankings[i] = pheromoneHeuristic.Run(inputs,2);
				indexArray[i] = i; //links fuzzyRankings array to foundAntIndex
			}
		}//end else

	
		//Update the pheromone
		//Remember indexArray links fuzzyRankings to foundAntIndex
		GetTopAnts(fuzzyRankings, indexArray);


		//--------------------------------
		//Printing some to files
		//--------------------------------
		/*
		for(int m=0;m<NUM_TO_UPDATE;m++)
		{
			std::stringstream hold;
			hold << "bestAnt" << m << ".csv";
			string fileName = hold.str();
			ants[foundAntIndex[indexArray[m]]].WritePathToFile(fileName.c_str());
		}
		*/

		//Clip in case less than NUM_TO_UPDATE made it
		int numToUpdate = NUM_TO_UPDATE;
		if(numToUpdate > numThatMadeIt)
			numToUpdate = numThatMadeIt;

		#ifdef _DEBUG_TOP_ANTS_
			printf("\n\n Top Ants: \n");
			for(int i=0;i<numToUpdate;i++)
			{
				printf(" %i: Index:%i PathGrade: %f  Length: %f  NumMoves: %i\n",i+1, foundAntIndex[indexArray[i]],
					ants[foundAntIndex[indexArray[i]]].pathGrade, ants[foundAntIndex[indexArray[i]]].pathLength,
					ants[foundAntIndex[indexArray[i]]].numMoves);
			}
		#endif


		//-----------------------------------
		// Pheromone Updating
		//-----------------------------------
		for(int i=0;i<numToUpdate;i++)
		{
			GlobalPheromoneUpdate(PATH_GAIN/(float)(i+1),&ants[foundAntIndex[indexArray[i]]]);
		}
		
		pheromoneMap.GlobalAdjustment(DISSIPATION_FACTOR);

		
		//------------------------------------
		// Pick off and save best ant
		//------------------------------------
		SaveBestAnt(foundAntIndex[indexArray[0]]);

		free(fuzzyRankings);
		free(indexArray);

	}//numThatMadeIt > 0
}//end gradePaths


//------------------------------------------------------   
void GetTopAnts(float fuzzyRankings[], int indexArray[])
{
	int topToUse = NUM_TO_UPDATE;

	if(topToUse > numThatMadeIt)
		topToUse = numThatMadeIt;


	//Do a limited bubble sort
	for(int i = 0;i < topToUse;i++)
	{
		for(int j = numThatMadeIt; j > i;j--)
		{
			if(fuzzyRankings[j] > fuzzyRankings[j-1])
			{
				float hold = fuzzyRankings[j-1];
				int hold1 = indexArray[j-1];

				fuzzyRankings[j-1] = fuzzyRankings[j];
				indexArray[j-1] = indexArray[j];

				fuzzyRankings[j] = hold;
				indexArray[j] = hold1;
			}
		}
	}

	#ifdef _DEBUG_INDIVIDUAL_ANT_
		printf("\n");
		for(int i=0;i<numThatMadeIt;i++)
		{
			printf("[%i]=%f ",indexArray[i], fuzzyRankings[i]); 
		}
	#endif

}//end GetTopAnts


//------------------------------------------------------
void GlobalPheromoneUpdate(float pheromoneLevel, cAnt *theAnt)
{
	//printf("In GlobalPheromoneUpdate, theAnt numMoves %i \n", theAnt->numMoves);

	for(int i=0;i<theAnt->numMoves;i++)
	{
		float currentLevel = pheromoneMap.ReadFromMap(theAnt->trimmedPath[i].position[LAT], theAnt->trimmedPath[i].position[LON]);
		pheromoneMap.WriteToMap(theAnt->trimmedPath[i].position[LAT]-1, theAnt->trimmedPath[i].position[LON]-1, currentLevel+pheromoneLevel);
	}
}//end GlobalPheromoneUpdate
