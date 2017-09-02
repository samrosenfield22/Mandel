/**/

#include "../include/keys.h"


void processNormalKeys(unsigned char key, int x, int y)
{
	static char enteredIters[7];
	static uint8_t ip=0;
	static bool MANUAL_ITER_MODE = false;

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

void zoomIn()
{
	float xspan, yspan;
	float temp;

	//set box orientation
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

	addNewFrame(xmax, xmin, ymax, ymin, ITERATIONS, zoom);

	//fprintf(db, "Zooming in to (%lf, %lf, %lf, %lf)\t\titerations:\t%d\n", xmin, xmax, ymin, ymax, ITERATIONS);

	//recompute the image
	pretestMandelValues();
	computeMandelValues();
}

void processSpecialKeys(int key, int x, int y)
{
	/*switch(key)
	{
		case GLUT_KEY_UP:
			crossy += 0.05f;
			break;

		case GLUT_KEY_DOWN:
			crossy -= 0.05f;
			break;

		case GLUT_KEY_RIGHT:
			crossx += 0.05f;
			break;

		case GLUT_KEY_LEFT:
			crossx -= 0.05f;
			break;
	}*/

}