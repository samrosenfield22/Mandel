

//#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <windows.h>
#include <GL/glut.h>

#include "../include/mandel.h"
#include "../include/render.h"
#include "../include/keys.h"
#include "../include/history.h"

//local definition of global variables
uint32_t ITERATIONS;												//current maximum iteration value
FILE *mdata, *db;													//pointers to files (for Mandelbrot computation data, and for debug)

mvalue_t xmax, xmin, ymax, ymin;									//current window corners, in Mandelbrot data point form 
float zoom, nextzoom;												//zoom depth
bool mouseHeld = false;												//mouse button state
float zoomBoxStartx, zoomBoxStarty, zoomBoxEndx, zoomBoxEndy;		//coordinates of the zoom box

char enteredIters[7];												//
bool MANUAL_ITER_MODE = false;										//

framestack history;													//stack of previous window parameters


int main(int argc, char **argv)
{
	//initialize GLUT, create the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,50);
	glutInitWindowSize(X_SCREEN_GL, Y_SCREEN_GL);
	glutCreateWindow("Mandelbrot set explorer");

	//open the debug file
	db = fopen("../dat/debug.txt", "w");
	if(db == NULL) return 0;
	//fprintf(db, "Files opened...\n");

	//load the first window/frame
	history.fcnt = (uint8_t)-1;
	addNewFrame(XMAX_INITIAL, XMIN_INITIAL, YMAX_INITIAL, YMIN_INITIAL, ITERATIONS_INITIAL, 1.0f);

	//compute the initial data set
	//pretestMandelValues();
	computeMandelValues();

	//callbacks
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(renderScene);

	//mouse/keyboard events
	glutKeyboardFunc(processNormalKeys);
	glutMouseFunc(processMouseClicks);
	glutMotionFunc(processMouseLoc);

	//start all GLUT events
	glutMainLoop();

	return 0;
}


