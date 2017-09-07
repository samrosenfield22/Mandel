/**/

#include "../include/render.h"

/*
Function: void renderScene(void)
Arguments: none
Returns: none
Description: redraws the current scene
*/
void renderScene()
{
	static float angle = 0.0f;
	char zoomstr[30];		//8 text chars (including the null), + up to 22 digits (including the decimal point + tenth)
	char iterstr[13];

	//do some openGL magic
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear Color and Depth Buffers
	glLoadIdentity();                   // Reset transformations
	gluLookAt(  0.0f, 0.0f, 10.0f,            // Set the camera
	0.0f, 0.0f,  0.0f,
	0.0f, 1.0f,  0.0f);
	glRotatef(angle, 0.0f, 1.0f, 0.0f);         //set camera rotation

	  
	//render the current display
	renderMandel();

	if (nextzoom<1000.0f) sprintf(zoomstr, "Zoom: %.1fx", nextzoom);
	else sprintf(zoomstr, "Zoom: %1.2ex", nextzoom);
	renderText(-5.8, -3.85, zoomstr, 0.0, 0.6, 0.0, GLUT_BITMAP_9_BY_15);
	
	
	if (MANUAL_ITER_MODE == true)
	{
		sprintf(iterstr, "Iters: %s", enteredIters);
		renderText(4.6, -3.85, iterstr, 0.6, 0.6, 0.6, GLUT_BITMAP_9_BY_15);
	}
	else
	{
		sprintf(iterstr, "Iters: %d", ITERATIONS);
		renderText(4.6, -3.85, iterstr, 0.0, 0.6, 0.0, GLUT_BITMAP_9_BY_15);
	}

	if (mouseHeld) renderZoomBox();

	//update display buffer
	glutSwapBuffers();
}

/*
Function: void changeSize(int, int)
Arguments: int w, int h (height and width)
Returns: none
Description: 
*/
void changeSize(int w, int h)
{
	float ratio;

	if (h == 0)
		h = 1;
	ratio =  w * 1.0 / h;

	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective.
	gluPerspective(45.0f, ratio, 0.1f, 100.0f);

	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
}

/*
Function: void renderMandel(void)
Arguments: none
Returns: none
Description: draws the Mandelbrot set. The data file (dat/mdata.txt) is parsed, the compression is decoded, and
each individual pixel is drawn with its appropriate color
*/
void renderMandel()
{
	uint32_t repeatCount;

	float x,y;
	uint32_t iters = 0;
	
	float xinc = X_SCREENLEN / INCR;
	float yinc = Y_SCREENLEN / (INCR*0.667);
	float xpix=-0.5*X_SCREENLEN, ypix=0.5*Y_SCREENLEN;

	uint32_t page[PAGE_SIZE];
	uint16_t piter = 0;


	mdata = fopen("../dat/mdata.txt", "rb");
	fseek(mdata, 0, SEEK_SET);

	readPageFromFile(page, mdata);

	while(iters != CMP_END_OF_FILE)
	{
		iters = readWordFromPage(page, &piter);

		switch(iters)
		{
			case CMP_START_REPEAT_SET:
				repeatCount = readWordFromPage(page, &piter);
				iters = readWordFromPage(page, &piter);

				//fprintf(db, "found a repeat block, 0x%0x repeats %d times\n", iters, repeatCount);
				for(; repeatCount; repeatCount--)
				{
					drawPixel(xpix, ypix, xinc, yinc, iters);
					xpix += xinc;
				}
				break;

			case CMP_LINE_FEED:
				xpix = -0.5*X_SCREENLEN;
				ypix -= yinc;
				break;

			default:
				drawPixel(xpix, ypix, xinc, yinc, iters);
				xpix += xinc;
				break;
		}
	}

	fclose(mdata);

	
}

/*
Function: uint32_t readWordFromPage(uint32_t[PAGE_SIZE], uint16_t *)
Arguments:	uint32_t page[PAGE_SIZE] (block of data being read)
			uint16_t *i (pointer to an index, which stores the current element in the page)
Returns: uint32_t (the word read from the page)
Description: reads a 32-bit word from the current page
*/
uint32_t readWordFromPage(uint32_t page[PAGE_SIZE], uint16_t *i)
{
	uint32_t word;

	word = page[*i];
	(*i)++;
	
	if (*i >= PAGE_SIZE)
	{
		readPageFromFile(page, mdata);
		*i = 0;
	}

	return word;
}

/*
Function: void readPageFromFile(uint32_t[PAGE_SIZE], FILE *)
Arguments:	uint32_t page[PAGE_SIZE] (block of data being read)
			FILE *fp (the file being read from)
Returns: none
Description: reads a page from the current location in the file pointed to by fp
*/
void readPageFromFile(uint32_t page[PAGE_SIZE], FILE *fp)
{
	fread(page, 4, PAGE_SIZE, fp);
}

/*
Function: void drawPixel(float, float, float, float, uint32_t)
Arguments:	float xpix, ypix (coordinates of the pixel)
			float xinc, yinc (size of the pixel)
			uint32_t iters (number of iterations for the given point)
Returns: none
Description: computes the color of the current pixel from its iteration value, and draws the pixel
at the specified location. The colors linearly blend between various predefined colors, which are
set in render.h
*/
void drawPixel(float xpix, float ypix, float xinc, float yinc, uint32_t iters)
{
	float r, g, b;
	float pixboxStartx, pixboxEndx, pixboxStarty, pixboxEndy;
	const float dimfactor = 0.01f;

	float cslope, compcolor;

	//const uint32_t cutoff_increment = 6;
	//uint32_t cutoff = 30;
	//uint32_t lastCutoff;
	//uint8_t colSeg=1, lastColSeg=0;
	
	//const uint32_t cutoffs[] = {0, 30, 200, 800, 2000, 6000};
	const uint32_t cutoffs[] = {0, 30, 200, 800, 2000, 5000, 15000, 50000, 120000, 400000, 800000};
	const float colorsegs[][3] = 
	{
		{0.0f, 0.0f, 0.0f},
		COLOR0,	COLOR1, COLOR2, COLOR3, COLOR4,
		COLOR0,	COLOR1, COLOR2, COLOR3, COLOR4
	};
	bool colorWithinRange = false;
	uint8_t i, j;


	if (iters == ITERATIONS)	//point is in the set
	{
		r = 0.0;
		g = 0.0;
		b = 1.0;
	}
	else
	{
		r = 0.0; g = 0.0; b = 0.0;
		for(i=1; i<sizeof(cutoffs)/sizeof(uint32_t); ++i)
		{
			if (iters < cutoffs[i])
			{
				//fprintf(db, "iters = %d\n", iters);
				for(j=0; j<3; ++j)	//index through colors
				{
					//eq of the line, where x values are the iters (in cutoffs[]) and y values are the colorsegs
					//from this slope we can use point-slope form
					//y - y1 = m(x - x1)
					//which gives us y, the color

					cslope = (colorsegs[i][j]-colorsegs[i-1][j]) / (float)(cutoffs[i]-cutoffs[i-1]);
					compcolor = cslope*(float)(iters-cutoffs[i-1]) + colorsegs[i-1][j];

					//fprintf(db, "\tfor color channel %d, computed m=%f, intensity=%f\n", j, cslope, compcolor);

					switch(j)
					{
						case 0:
							r = compcolor; break;
						case 1:
							g = compcolor; break;
						case 2:
							b = compcolor; break;
					}
				}

				colorWithinRange = true;
				break;
			}
		}

		if (!colorWithinRange)
		{
			//use a ____ function to fade out the final color
			//6000/x
			
			compcolor = ((float)cutoffs[5]/(float)iters);
			r = colorsegs[5][0] * compcolor;
			g = colorsegs[5][1] * compcolor;
			b = colorsegs[5][2] * compcolor;
			fprintf(db, "iters=%d, setting color division factor to %f\n", iters, compcolor);
		}
	}

	
	
	/*
	//dim everything outside the box (needs work -- pixbox's should be precomputed, dim area is wrong)
	if (mouseHeld)
	{
		pixboxStartx = (zoomBoxStartx-0.5) * X_SCREENLEN;
		pixboxEndx = (zoomBoxEndx-0.5) * X_SCREENLEN;
		pixboxStarty = (zoomBoxStarty-0.5) * Y_SCREENLEN;
		pixboxEndy = (zoomBoxEndy-0.5) * Y_SCREENLEN;
		fprintf(db, "dimming all points out of %f,%f,%f,%f\n", pixboxStartx, pixboxEndx, pixboxStarty, pixboxEndy);
		if (	((xpix>pixboxStartx && xpix>pixboxEndx) || (xpix<pixboxStartx && xpix<pixboxEndx))
			&& ((ypix>pixboxStarty && ypix>pixboxEndy) || (ypix<pixboxStarty && ypix<pixboxEndy))	)
		{
			r *= dimfactor;
			g *= dimfactor;
			b *= dimfactor;
		}
	}*/

	glColor3f(r, g, b);

	//draw the pixel
	glBegin(GL_QUADS);
		glVertex3f(xpix, 		ypix, 0.0);
		glVertex3f(xpix+xinc,	ypix, 0.0);
		glVertex3f(xpix+xinc, 	ypix-yinc, 0.0);
		glVertex3f(xpix, 		ypix-yinc, 0.0);
	glEnd();
	glPopMatrix();
}

/*
Function: void renderZoomBox(void)
Arguments: none
Returns: none
Description: draws the box indicating where the user wants to zoom to. This is only called if the mouse is held
*/
void renderZoomBox()
{
	float thickness = 0.03f;

	//convert (0-1) values to screen render coordinates
	float startNx = zoomBoxStartx*X_SCREENLEN - (X_SCREENLEN/2);
	float endNx = zoomBoxEndx*X_SCREENLEN - (X_SCREENLEN/2);
	float startNy = zoomBoxStarty*Y_SCREENLEN - (Y_SCREENLEN/2);
	float endNy = zoomBoxEndy*Y_SCREENLEN - (Y_SCREENLEN/2);

	glColor3f(0.5f, 0.0, 0.0);
	glBegin(GL_QUADS);		//top line
		glVertex3f(startNx-thickness,	startNy+thickness,	0.0);
		glVertex3f(startNx-thickness,	startNy,			0.0);
		glVertex3f(endNx+thickness,		startNy,			0.0);
		glVertex3f(endNx+thickness,		startNy+thickness,	0.0);
	glEnd();
	glPopMatrix();

	glColor3f(0.5f, 0.0, 0.0);
	glBegin(GL_QUADS);		//left line
		glVertex3f(startNx-thickness,	startNy+thickness,	0.0);
		glVertex3f(startNx-thickness,	endNy-thickness,	0.0);
		glVertex3f(startNx,				endNy-thickness,	0.0);
		glVertex3f(startNx,				startNy+thickness,	0.0);
	glEnd();
	glPopMatrix();

	glColor3f(0.5f, 0.0, 0.0);
	glBegin(GL_QUADS);		//right line
		glVertex3f(endNx,				startNy+thickness,	0.0);
		glVertex3f(endNx,				endNy-thickness,	0.0);
		glVertex3f(endNx+thickness,		endNy-thickness,	0.0);
		glVertex3f(endNx+thickness,		startNy+thickness,	0.0);
	glEnd();
	glPopMatrix();

	glColor3f(0.5f, 0.0, 0.0);
	glBegin(GL_QUADS);		//bottom line
		glVertex3f(startNx-thickness,	endNy,				0.0);
		glVertex3f(startNx-thickness,	endNy-thickness,	0.0);
		glVertex3f(endNx+thickness,		endNy-thickness,	0.0);
		glVertex3f(endNx+thickness,		endNy,				0.0);
	glEnd();
	glPopMatrix();
}

/*
Function: void renderText(float, float, char *, float, float, float, void *)
Arguments: 	float x, y (coordinates of the text)
			char *c (points to the string being printed)
			float r, g, b (color channel values of the text)
			void *font (points to a constant which defines the text font)
Returns: none
Description: draws a string
*/
void renderText(float x, float y, char *c, float r, float g, float b, void *font)
{
	glColor3f(r, g, b);
	glRasterPos2f(x, y);	

	while(*c)
		glutBitmapCharacter(font, *c++);

	glPopMatrix();
}