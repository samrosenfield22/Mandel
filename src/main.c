

//#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <windows.h>
#include <GL/glut.h>

#include "../include/mandel.h"
#include "../include/render.h"
#include "../include/keys.h"

uint32_t ITERATIONS = ITERATIONS_INITIAL;
uint16_t *mandVals;
FILE *mdata, *db;

double xmax=XMAX_INITIAL, xmin=XMIN_INITIAL, ymax=YMAX_INITIAL, ymin=YMIN_INITIAL;
bool mouseHeld = false;
float zoomBoxStartx, zoomBoxStarty, zoomBoxEndx, zoomBoxEndy;
float zoom = 1.0f, nextzoom = 1.0f;

int main(int argc, char **argv)
{

  // init GLUT and create window
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowPosition(100,50);
  glutInitWindowSize(X_SCREEN_GL, Y_SCREEN_GL);
  glutCreateWindow("Mandelbrot set explorer");

  //open the file
  //mdata = fopen("../mdata.txt", "wb+");
  //if(mdata == NULL) return 0;
  db = fopen("../dat/debug.txt", "w");
  if(db == NULL) return 0;
  fprintf(db, "Files opened...\n");

  //compute the initial data set
  computeMandelValues();

  // register callbacks
  glutDisplayFunc(renderScene);
  glutReshapeFunc(changeSize);
  glutIdleFunc(renderScene);

  //
  glutKeyboardFunc(processNormalKeys);
  glutSpecialFunc(processSpecialKeys);
  //glutKeyboardUpFunc(processNormalKeyReleases);
  //glutSpecialUpFunc(processSpecialKeyReleases); 

  glutMotionFunc(processMouseLoc);
  glutMouseFunc(processMouseClicks);

  // enter GLUT event processing cycle
  glutMainLoop();

  return 0;
}


