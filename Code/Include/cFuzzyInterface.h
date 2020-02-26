
#ifndef _FUZZY_INTERFACE_H_
#define _FUZZY_INTERFACE_H_

#include "FFLLAPI.h"
#include <stdio.h>

class cFuzzyInterface
{

	public:
		cFuzzyInterface(const char *fcl_file);
		float Run(float inputs[], int numInputs);

	private:
		int model;
		int child;
};

#endif