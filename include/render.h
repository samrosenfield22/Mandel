/**/

#ifndef RENDER_H_
#define RENDER_H_

#include <windows.h>
#include <GL/glut.h>
#include <stdbool.h>

#include "mandel.h"

#define X_SCREEN_GL 960
#define Y_SCREEN_GL 640

#define X_SCREENLEN ((float)X_SCREEN_GL/78)
#define Y_SCREENLEN ((float)Y_SCREEN_GL/78)

#define COLOR0 	{1.0f, 0.0f, 0.0f}			//red
#define COLOR1	{0.29f, 0.0f, 0.51f}		//indigo
#define COLOR2	{0.0f, 0.5f, 1.0f}			//cyan-ish
#define COLOR3	{0.0f, 1.0f, 0.0f}			//green
#define COLOR4	{1.0f, 1.0f, 0.0f}			//yellow

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