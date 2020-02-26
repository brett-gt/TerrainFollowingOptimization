
#include "..\Include\cFuzzyInterface.h"


//------------------------------------------
cFuzzyInterface::cFuzzyInterface(const char *fcl_file)
{
    // create and load the model
	model = ffll_new_model();  

    int ret_val = (int)ffll_load_fcl_file(model, fcl_file);  

	if (ret_val < 0)
	{
		printf("Error opening %s %i", fcl_file, ret_val);
		return;
	}

	// create a child for the model...
    child = ffll_new_child(model);
}


//---------------------------------------------------------
float cFuzzyInterface::Run(float inputs[], int numInputs)
{
	for(int i = 0; i < numInputs;i++)
	{
		ffll_set_value(model, child, i, inputs[i]); 
	}

	return (float)ffll_get_output_value(model, child);
}
 

