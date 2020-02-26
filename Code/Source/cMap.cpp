
/******************************************************************************
 cMap.cpp

 The map file defines a map class that is used to create a data map. 

 The map itself is stored as a one dimensional array of floats, however through
 interface function reading/writing in a (row, column) format is acheived.

  Note: Indexing matches indexing into Matlab array, increased by 1 (row+1, column+1)
  So use 1 to mapRows/mapColumns indexing

******************************************************************************/



#include "..\Include\cMap.h"
#include "..\Include\csv_parser.hpp"
#include <stdlib.h>



/******************************************************************************
 cMap::cMap()

 Class initializer.  Initializes rows and columns of the map to 0.

******************************************************************************/
cMap::cMap()
{
	mapRows = 0;
	mapColumns = 0;
	allocated = false;
}


/******************************************************************************
 cMap::Initialize(int rows, int columns, bool limit, float minLimit, float maxLimit)

 Description:
	The initialize function creates the base map.  The map itself it allocated using
	the row and column values passed in.

 Inputs:
	int rows       -> the number of rows in the map
	int columns    -> the number of columns in the map
	bool limit     -> if true, then data in the map will be limited using limit arguments
	float minLimit -> minimum value used by limiting function
	float maxLimit -> maximum value used by limiting function

 Outputs:
	The function always returns a 1

******************************************************************************/
int cMap::Initialize(int rows, int columns, bool limit, float minLimit, float maxLimit)
{
	mapRows = rows;
	mapColumns = columns;

	useLimits = limit;
	mapMax = maxLimit;
	mapMin = minLimit;

	if(allocated)
	{
		free(theMap);
	}

	theMap = (float *)malloc(sizeof(float)*rows*columns);

	allocated = true;

	return 1;
}


/******************************************************************************
 int cMap::ReadMapFromFile(const char* filename)

 Description:
	Reads a map from the file and writes it into the map.  Calling this function
	will overwrite any current map data.

	Format:  Each row of the map is in its own row  in the file, columns are
	         comma delimited

 Inputs:
	const char* filename -> file name from which the map should be read.

 Outputs:
	The function always returns a 0

******************************************************************************/
int cMap::ReadMapFromFile(const char* filename)
{
	/* Declare the variables to be used */
	const char field_terminator = ',';
	const char line_terminator  = '\n';
	const char enclosure_char   = '"';

	csv_parser file_parser;

	/* Define how many records we're gonna skip. This could be used to skip the column definitions. */
	file_parser.set_skip_lines(0);

	/* Specify the file to parse */
	file_parser.init(filename);

	/* Here we tell the parser how to parse the file */
	file_parser.set_enclosed_char(enclosure_char, ENCLOSURE_OPTIONAL);
	file_parser.set_field_term_char(field_terminator);
	file_parser.set_line_term_char(line_terminator);

	int row_count = 0;
	int column_count = 0;

	//Count the rows and columns
	bool first = true;

	while(file_parser.has_more_rows())
	{
		row_count++;
		csv_row row = file_parser.get_row();

		if(first)
		{
			column_count = row.size();
			first = false;
		}
		//TODO: Can add a check to make sure every row is same size
	}
	
	printf("\n------------------------------------\n");
	printf("ReadFromFile: rows %i, columns %i\n\n", row_count, column_count);
	Initialize(row_count, column_count, false, 0.0f, 0.0f);

	//TODO: Figure out how not to repeat this initialization code
	csv_parser file_parser1;

	/* Define how many records we're gonna skip. This could be used to skip the column definitions. */
	file_parser1.set_skip_lines(0);

	/* Specify the file to parse */
	file_parser1.init(filename);

	/* Here we tell the parser how to parse the file */
	file_parser1.set_enclosed_char(enclosure_char, ENCLOSURE_OPTIONAL);
	file_parser1.set_field_term_char(field_terminator);
	file_parser1.set_line_term_char(line_terminator);

	row_count = 0;

	//printf("------ ROW %i -------\n", row_count);

	//Now actually read data in
	while(file_parser1.has_more_rows())
	{
		unsigned int i = 0;

		/* Get the record */
		csv_row row = file_parser1.get_row();
		
		// Write read data into the map array
		for (i = 0; i < row.size(); i++)
		{
			WriteToMap(row_count, i, (float)atoi(row[i].c_str()));
			//printf("COLUMN %02d : %s\n", i + 1U, row[i].c_str());
		}
		row_count++;
	}

	return 0;
}



/******************************************************************************
 int cMap::WriteMapToFile(const char* filename)

 Description:
	Writes current map data to a file.  If the file already exists, it will be
	overwritten.  

	Format:  Each row of the map is in its own row  in the file, columns are
	         comma delimited

 Inputs:
	const char* filename -> file name from which the map should be read.

 Outputs:
	The function always returns a 0

******************************************************************************/
int cMap::WriteMapToFile(const char* filename)
{
	FILE *write_fp = fopen(filename, "w");

	if (write_fp == NULL)
	{
		fprintf(stderr, "Error WriteMapToFile : unable to open input file %s\n", filename);
		return false;
	}

	printf("\n----------------------------------------\n");
	printf("Writing map: rows %i, columns %i \n", mapRows,mapColumns);

	for(int i = 1;i <= mapRows; i++)
	{
		for(int j=1;j <= mapColumns; j++)
		{
			fprintf(write_fp,"%f", ReadFromMap(i,j)); 

			if(j != mapColumns)
				fprintf(write_fp,",");
		}

		fprintf(write_fp,"\n");
	}
	
	fclose(write_fp); 
	return true;
}



/******************************************************************************
 int cMap::WriteToMap(int row, int column, float value)

 Description:
	Writes value into the map as indexed by (row, column)

 Inputs:
	int row, column -> location to be written to
	float value     -> value to be written into the map

 Outputs:
	Returns -1 if index is invalid, 0 if successful

******************************************************************************/
int cMap::WriteToMap(int row, int column, float value)
{
	if(row < 0 || row >= mapRows || column < 0 || column >= mapColumns)
	{
		printf("ReadFromMap: Index out of bounds! Row: %i  Column: %i\n", row, column);
		return -1;
	}


	int index = column + row*mapColumns;

	if(useLimits)
	{
		if(value > mapMax)
			value = mapMax;
		else if(value < mapMin)
			value = mapMin;
	}

	//printf("theMap[%i] = %i;\n",column + row*mapColumns, value);
	theMap[index] = value;
	return 0;
}



/******************************************************************************
 float cMap::ReadFromMap(int row, int column)

 Description:
	Writes current map data to a file.  If the file already exists, it will be
	overwritten.  

	Format:  Each row of the map is in its own row  in the file, columns are
	         comma delimited

 Inputs:
	const char* filename -> file name from which the map should be read.

 Outputs:
	The floating point value being read

******************************************************************************/
float cMap::ReadFromMap(int row, int column)
{
	row = row - 1;
	column = column - 1;
	if(row < 0 || row >= mapRows || column < 0 || column >= mapColumns)
	{
		printf("ReadFromMap: Index out of bounds! Row: %i  Column: %i\n", row, column);
		return -1;
	}

	return theMap[column + row*mapColumns];
}



/******************************************************************************
 void cMap::GlobalAdjustment(float adjustmentFactor)

 Description:
	Multiplies every value in the map by the adjustment factor

 Inputs:
	float adjustmentFactor

 Outputs:
	Nonr

******************************************************************************/
void cMap::GlobalAdjustment(float adjustmentFactor)
{
	for(int i = 0;i < mapRows*mapColumns;i++)
	{
		theMap[i] = theMap[i] * adjustmentFactor;

		if(useLimits)
		{
			if(theMap[i] > mapMax)
				theMap[i] = mapMax;
			else if(theMap[i] < mapMin)
				theMap[i] = mapMin;
		}
	}
}