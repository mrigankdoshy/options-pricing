/* C++ program for estimation of Pi using Monte 
Carlo Simulation */

#include <iostream>
#include <stdlib.h>      //defines rand(), srand(), RAND_MAX
#include <time.h>        //defines ctime() for random numbers
#include <math.h>        //defines math functions
#include <stdio.h>

/* Defines precision for x and y values. More the interval, more the number of significant digits */
#define INTERVAL 1000000

using namespace std; 

int main() 
{  
	double randX, randY, distance, pi; 
	double interval, circlePoints = 0, squarePoints = 0; 

	/* Initializing rand() */
	srand(time(NULL)); 

	/* Total Random numbers generated = possible x values * possible y values */
	for (double i = 0; i < INTERVAL; i++) { 

		/* Randomly generated x and y values */
		randX = double(rand() % (INTERVAL + 1)) / INTERVAL; 
		randY = double(rand() % (INTERVAL + 1)) / INTERVAL; 

		/* Distance between (x, y) from the origin */
		distance = (randX * randX) + (randY * randY); 

		/* Checking if (x, y) lies inside the define */
		/* Circle with R = 1 */ 
		if (distance <= 1) 
			circlePoints++; 

		/* Total number of points generated */
		squarePoints++; 

		// estimated pi after this iteration 
		pi = double(4 * circlePoints) / squarePoints; 

		// For visual understanding (Optional) 
		cout << "x: "<<randX << "\ty: " << randY << "\t\tInside Circle: " << circlePoints 
			<< "\t\tInside Square: " << squarePoints << "\t\tEstimated PI: " << pi << endl << endl; 

	} 

	// Final Estimated Value 
	cout << "\nFinal Estimation of Pi = " << pi; 

	return 0; 
} 
