// AntColony.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>

#include "Include\cMap.h"
#include "Include\MiscFunctions.h"
#include "Include\GlobalData.h"
#include "Include\cFuzzyInterface.h"
#include "Include\ACO_MinMax.h"

using namespace std;

int main()
{

	//------------------------------------
	// Initialize the maps
	//------------------------------------
	//worldMap.ReadMapFromFile("worldMap.csv");
	//maskingMap.ReadMapFromFile("gradedMap.csv");

	worldMap.ReadMapFromFile("Maps\\flatWorldMap.csv");
	maskingMap.ReadMapFromFile("Maps\\flatIntervisMap.csv");

	pheromoneMap.Initialize(worldMap.mapRows, worldMap.mapColumns, true, PHEROMONE_MIN, PHEROMONE_MAX);

	for (int i = 0; i < pheromoneMap.mapRows; i++)
	{
		for (int j = 0; j < pheromoneMap.mapColumns; j++)
		{
			pheromoneMap.WriteToMap(i, j, 10.0f);
		}
	}


	int start[2] = { 90, 100 };
	int end[2] = { 18, 55 };

	MinMax(start, end);

	/*
	float rankings[10] = {0.2, 30, 1, 0.1, 50, 60, 14, 0.5, 0.9, 2};
	int index[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

	GetTopAnts(&rankings[0], &index[0]);
	*/

	/* This tested matched up with matlab fuzzy output
	cFuzzyInterface test("local.fcl");
	float data[2] = {1.4f, -0.5f};

	float result = test.Run(data, 2);
	printf("%f \n", result);
	*/

	/* Testing read I/O
	CSV_IO_ReadMap("worldMap.csv", &worldMap);
	printf("worldMap: Rows: %i  Columns %i \n",worldMap.mapRows, worldMap.mapColumns);
	printf("worldMap(0,0) = %i  worldMap(120,120) = %i\n",worldMap.ReadFromMap(10,30),worldMap.ReadFromMap(2,8));
	*/

	/* Testing getCandidateMoves

	ANT_STATE_TYPE test;

	test.heading = 0;
	test.position[0] = 10;
	test.position[1] = 20;

	getCandidateMoves(test);

	printf("0: H:%i P:[%i,%i]  1: H:%i P:[%i,%i]  2: H:%i P:[%i,%i] \n",
		candidateStates[0].heading, candidateStates[0].position[0], candidateStates[0].position[1],
		candidateStates[1].heading, candidateStates[1].position[0], candidateStates[1].position[1],
		candidateStates[2].heading, candidateStates[2].position[0], candidateStates[2].position[1]);
	*/

	/*
	printf("Entering ant colony \n");

	cMap test(100,100);

	test.WriteToMap(10,15,5);

	printf("%i \n", test.ReadFromMap(10,15));
	*/

	/*
	float test[5];
	test[1] = 0.1f;
	test[2] = 5.0f;
	test[0] = 0.0f;
	test[3] = -100.0f;
	test[4] = -5.0f;

	printf("%f %f \n", FindArrayMax(test,5), FindArrayMin(test,5));
	*/

	/*
	int a[2] = {10, 10};
	int b[2] = {20, 20};
	printf("%f \n", GetAngleToTarget(a, b));
	*/

	cin.get();
}
