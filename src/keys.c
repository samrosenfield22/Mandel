/**/

#include "../include/keys.h"

/*
Function: void processNormalKeys(unsigned char, int, int)
Arguments:	unsigned char key (ASCII value of the key being pressed)
			int x, y (not used)
Returns: none
Description: processes key presses. For more details on specific keys, consult the readme.
*/
void processNormalKeys(unsigned char key, int x, int y)
{
	//static char enteredIters[7];
	static uint8_t ip=0;
	//static bool MANUAL_ITER_MODE = false;

	switch(key)
	{
		case 27:		//esc
			exit(0);
			break;

		case 'h':
			xmax=XMAX_INITIAL;
			xmin=XMIN_INITIAL;
			ymax=YMAX_INITIAL;
			ymin=YMIN_INITIAL;
			ITERATIONS = ITERATIONS_INITIAL;
			zoom = 1.0;
			nextzoom = 1.0;
			addNewFrame(xmax, xmin, ymax, ymin, ITERATIONS_INITIAL, zoom);
			//pretestMandelValues();
			computeMandelValues();
			break;

		case 'r':
			returnToPrevFrame();
			//pretestMandelValues();
			computeMandelValues();
			break;

		//dev mode?
		case 'i':
			MANUAL_ITER_MODE = true;
			ip = 0;
			enteredIters[0] = '\0';
			break;

		case ' ':
			if (MANUAL_ITER_MODE == true)
			{
				ITERATIONS = strtoul(enteredIters, NULL, 10);
				fprintf(db, "read %s, setting iters to %d\n", enteredIters, ITERATIONS);
				MANUAL_ITER_MODE = false;
				computeMandelValues();
			}
			break;
	}

	if (key>='0' && key<='9')
	{
		if (MANUAL_ITER_MODE == true)
		{
			fprintf(db, "entered key %c at location %d\n", key, ip);
			enteredIters[ip] = key;
			ip++;
			enteredIters[ip] = '\0';
		}
	}
	
}

/*
Function: void processMouseClicks(int, int, int, int)
Arguments: 	int button (not used)
			int state (state of the mouse button)
			int x, y (coordinates of the mouse click)
Returns: none
Description: checks if the mouse button is being pressed or released. If it's pressed, start drawing a zoom box;
if it's released, zoom into the current zoom box
*/
void processMouseClicks(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		mouseHeld = true;
		zoomBoxStartx = (float)x/X_SCREEN_GL;
		zoomBoxStarty = 1.0-((float)y/Y_SCREEN_GL);
		zoomBoxEndx = zoomBoxStartx;
		zoomBoxEndy = zoomBoxStarty;
	}
	else
	{
		mouseHeld = false;

		//make sure user didn't just click
		fprintf(db, "box size: %f\n", fabs(zoomBoxStartx-zoomBoxEndx));
		if (fabs(zoomBoxStartx-zoomBoxEndx)>minBoxSize)
			zoomIn();
	}
}

/*
Function: void processMouseLoc(int, int)
Arguments: 	int x, y (coordinates of the mouse's location)
Returns: none
Description: whenever the mouse is moved (not clicked), this function checks if the mouse is held, and
if it is, updates the zoom box coordinates
*/
void processMouseLoc(int x, int y)
{
	float xboxdist, yboxdist;
	float xdir, ydir;

	if (mouseHeld)
	{
		//record mouse location (normalized from 0-1)
		zoomBoxEndx = (float)x/X_SCREEN_GL;
		zoomBoxEndy = 1.0-((float)y/Y_SCREEN_GL);

		//make sure the box preserves the 3:2 window ratio
		xboxdist = zoomBoxEndx - zoomBoxStartx;
		yboxdist = zoomBoxEndy - zoomBoxStarty;

		if (fabs(xboxdist) > fabs(yboxdist))	//too long
		{
			xdir = xboxdist / fabs(xboxdist);
			zoomBoxEndx = zoomBoxStartx + xdir*fabs(yboxdist);
		}
		else
		{
			ydir = yboxdist / fabs(yboxdist);
			zoomBoxEndy = zoomBoxStarty + ydir*fabs(xboxdist);
		}

		if (fabs(zoomBoxStartx-zoomBoxEndx)>minBoxSize)
			nextzoom = zoom / fabs(zoomBoxEndx - zoomBoxStartx);
		else
			nextzoom = zoom;
	}
}

/*
Function: void zoomIn(void)
Arguments: none
Returns: none
Description: zooms in to the area selected by the zoom box. This function maps the graphical box coordinates to the
appropriate mathematical values.
*/
void zoomIn()
{
	float xspan, yspan;
	float temp;

	//set box orientation if it's inverted
	if (zoomBoxStartx > zoomBoxEndx)
	{
		temp = zoomBoxStartx;
		zoomBoxStartx = zoomBoxEndx;
		zoomBoxEndx = temp;
	}
	if (zoomBoxStarty > zoomBoxEndy)
	{
		temp = zoomBoxStarty;
		zoomBoxStarty = zoomBoxEndy;
		zoomBoxEndy = temp;
	}

	//fprintf(db, "graphical box coords: %f,%f,%f,%f\n", zoomBoxStartx, zoomBoxEndx, zoomBoxStarty, zoomBoxEndy);

	//compute the numerical span of the current view
	xspan = xmax-xmin;
	yspan = ymax-ymin;

	//compute the new view corners' numerical values
	xmax = xmin + xspan*zoomBoxEndx;
	xmin = xmin + xspan*zoomBoxStartx;
	ymax = ymin + yspan*zoomBoxEndy;
	ymin = ymin + yspan*zoomBoxStarty;		

	//fprintf(db, "new numerical coords: %f,%f,%f,%f\n", xmin, xmax, ymin, ymax);

	//increase the iteration count
	//ITERATIONS /= (zoomBoxEndx - zoomBoxStartx);
	//if (ITERATIONS > 10000)
	//	ITERATIONS = 10000;
	zoom = nextzoom;


	//fprintf(db, "Zooming in to (%lf, %lf, %lf, %lf)\t\titerations:\t%d\n", xmin, xmax, ymin, ymax, ITERATIONS);

	//recompute the image
	pretestMandelValues();
	addNewFrame(xmax, xmin, ymax, ymin, ITERATIONS, zoom);
	computeMandelValues();
}