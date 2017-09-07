/**/

#ifndef KEYS_H_
#define KEYS_H_

#include <math.h>
//#include <stdlib.h>

#include "render.h"
#include "history.h"

#define minBoxSize 0.003f

//external global variables
extern char enteredIters[7];
extern bool MANUAL_ITER_MODE;

void processNormalKeys(unsigned char, int, int);
void processMouseClicks(int, int, int, int);
void processMouseLoc(int, int);

void zoomIn();


#endif //KEYS_H_