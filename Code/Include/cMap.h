
/*********************************************************************
 cMap.h

 The map file defines a map class that is used to create a data map. 

 The map itself is stored as a one dimensional array, however through interface
 function reading/writing in a (row, column) format is acheived.

**********************************************************************/

#ifndef _MAP_H_
#define _MAP_H_

#include <stdio.h>
#include <string>
#include <sstream>

class cMap
{
	public:
		cMap();

		int Initialize(int rows, int columns, bool limit, float minLimit, float maxLimit);

		int ReadMapFromFile(const char* filename);
		int WriteMapToFile(const char* filename);

		int WriteToMap(int row, int column, float value);
		float ReadFromMap(int row, int column);
		void GlobalAdjustment(float adjustmentFactor);

		int mapRows;
		int mapColumns;

	private:
		float *theMap;

		float mapMin;
		float mapMax;
		bool useLimits;

		bool allocated;
};


#endif