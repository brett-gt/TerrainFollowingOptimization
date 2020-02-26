#include "..\Include\MiscFunctions.h"

//------------------------------------------------
float FindArrayMax(float arrayPtr[], int arraySize)
{
	float currentMax = arrayPtr[0];

	for(int i=1;i<arraySize;i++)
	{
		if(currentMax < arrayPtr[i])
			currentMax = arrayPtr[i];
	}

	return currentMax;
}


//-----------------------------------------------
float FindArrayMin(float arrayPtr[], int arraySize)
{
	float currentMin = arrayPtr[0];

	for(int i=1;i<arraySize;i++)
	{
		if(currentMin > arrayPtr[i])
			currentMin = arrayPtr[i];
	}

	return currentMin;
}



//--------------------------------------------------
int snapAngle(float angleIn)
{
	int snappedAngle;

    if(angleIn > -22.5  && angleIn <= 22.5)
        snappedAngle = 0;

    else if(angleIn > 22.5  && angleIn <= 67.5)
        snappedAngle = 45;

    else if(angleIn > 67.5  && angleIn <= 112.5)
        snappedAngle = 90;    

    else if(angleIn > 112.5  && angleIn <= 157.5)
        snappedAngle = 135;  

    else if(angleIn > 157.5  || angleIn <= -157.5)
        snappedAngle = 180;  

    else if(angleIn > -157.5  && angleIn <= -112.5)
        snappedAngle = -135; 

    else if(angleIn > -112.5  && angleIn <= -67.5)
        snappedAngle = -90;  

    else if(angleIn > -67.5  && angleIn <= -22.5)
        snappedAngle = -45;  

    else
	{
		printf("SnapAngle: Error angleIn not within range: %f \n", angleIn);
        snappedAngle = 0;
	}

	return snappedAngle;
	  
}



//----------------------------------------------------
float GetAngleToTarget(int currentPoint[2], int targetPoint[2])
{
	int a = targetPoint[LAT] - currentPoint[LAT];
	int b = targetPoint[LON] - currentPoint[LON];

	float c = sqrt((float)a*a + (float)b*b);

	float theta = asin(a/c);

	theta = theta * 180/PI;

	//quadrant 2
	if(a >= 0 && b < 0)
		theta = theta + 90;
	else if(a < 0 && b < 0)
		theta = -theta - 180;

	return theta;
}
  


