/**/

#include "../include/history.h"

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

void returnToPrevFrame()
{
	if (history.fcnt)
		history.fcnt--;

	loadCurFrame();
}

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