/**/

#ifndef KEYS_H_
#define KEYS_H_

#include <math.h>

#include "render.h"


void processNormalKeys(unsigned char, int, int);
void processSpecialKeys(int, int, int);
void processMouseClicks(int, int, int, int);
void processMouseLoc(int, int);

void zoomIn();


#endif //KEYS_H_