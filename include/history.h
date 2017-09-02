/**/

#ifndef HISTORY_H_
#define HISTORY_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "mandel.h"
#include "render.h"

typedef struct
{
	mvalue_t xmax, xmin, ymax, ymin;
	uint32_t iters;
	float zoom;
} frame;

typedef struct
{
	//frame *frame;
	frame frame[255];
	uint8_t fcnt;
} framestack;

extern framestack history;

//
void addNewFrame(mvalue_t, mvalue_t, mvalue_t, mvalue_t, uint32_t, float);
void returnToPrevFrame(void);
void loadCurFrame(void);
void dumpHistory(void);

#endif //HISTORY_H_