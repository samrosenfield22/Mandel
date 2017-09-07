/**/

#include "../include/history.h"

/*
Function: void addNewFrame(mvalue_t, mvalue_t, mvalue_t, mvalue_t, uint32_t, float)
Arguments:	mvalue_t xmaxp, xminp, ymaxp, yminp (corners of the new frame)
			uint32_t itersp (maximum iterations for the new frame)
			float zoomp ()
Returns: none
Description: adds a new frame (description of a zoom state) to the stack, which allows us to return to previous images
*/
void addNewFrame(mvalue_t xmaxp, mvalue_t xminp, mvalue_t ymaxp, mvalue_t yminp, uint32_t itersp, float zoomp)
{
	history.fcnt++;
	history.frame[history.fcnt].xmax = xmaxp;
	history.frame[history.fcnt].xmin = xminp;
	history.frame[history.fcnt].ymax = ymaxp;
	history.frame[history.fcnt].ymin = yminp;
	history.frame[history.fcnt].iters = itersp;
	history.frame[history.fcnt].zoom = zoomp;

	loadCurFrame();
}

/*
Function: void returnToPrevFrame(void)
Arguments: none
Returns: none
Description: reloads the last image
*/
void returnToPrevFrame()
{
	if (history.fcnt)
		history.fcnt--;

	loadCurFrame();
}

/*
Function: 
Arguments: 
Returns: 
Description: after adding or returning from another frame, this function loads the parameters of the current frame so that it can be displayed
*/
void loadCurFrame()
{
	uint8_t i = history.fcnt;

	xmax = history.frame[i].xmax;
	xmin = history.frame[i].xmin;
	ymax = history.frame[i].ymax;
	ymin = history.frame[i].ymin;
	zoom = history.frame[i].zoom;
	ITERATIONS = history.frame[i].iters;
	nextzoom = zoom;

	dumpHistory();
}

/*
Description: for debug
*/
void dumpHistory()
{
	uint8_t i;

	fprintf(db, "FRAME HISTORY DUMP:\n");
	for(i=0; i<=history.fcnt; ++i)
	{
		fprintf(db, "\twindow: %f,%f,%f,%f\n\titers: %d\n\tzoom: %f\n",
			(double)history.frame[i].xmax, (double)history.frame[i].xmin,
			(double)history.frame[i].ymax, (double)history.frame[i].ymin, 
			history.frame[i].iters, history.frame[i].zoom);
	}

	
}