/**/

#ifndef RENDER_H_
#define RENDER_H_

#include <windows.h>
#include <GL/glut.h>
#include <stdbool.h>

#include "mandel.h"

#define X_SCREEN_GL 960
#define Y_SCREEN_GL 640

#define X_SCREENLEN 12.4
#define Y_SCREENLEN 8.2
//#define ZOOM_DEPTH 1.2

//
//extern float crossx, crossy;
extern bool mouseHeld;
extern float zoomBoxStartx, zoomBoxStarty, zoomBoxEndx, zoomBoxEndy;
extern float zoom, nextzoom;

//
void renderScene(void);
void changeSize(int, int);
void renderMandel(void);
uint32_t readWordFromPage(uint32_t[], uint16_t *);
void readPageFromFile(uint32_t[], FILE *);
//uint32_t readWordFromFile(FILE *);
//void renderCrosshair(float, float);
void renderZoomBox(void);
void drawPixel(float, float, float, float, uint32_t);
void renderText(float, float, char *, float, float, float, void *);


#endif	//RENDER_H_