/**/

#ifndef MANDEL_H_
#define MANDEL_H_


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>



#define INCR 1000		//number of points (on the x-axis, 2/3rds of this value on the y-axis)

#define XMAX_INITIAL 1.0
#define XMIN_INITIAL -2.0
#define YMAX_INITIAL 1.0
#define YMIN_INITIAL -1.0

#define ITERATIONS_INITIAL 800

#define PAGE_SIZE 16834

//#define EXPORT_HIST_DATA true


enum COMPRESSION_CONSTS
{
	CMP_START_REPEAT_SET = 0xFFFFFFFD,
	CMP_LINE_FEED = 0xFFFFFFFE,
	CMP_END_OF_FILE = 0xFFFFFFFF
};

extern double xmax, xmin, ymax, ymin;

extern uint32_t ITERATIONS;
extern uint16_t *mandVals;
extern FILE *mdata;
extern FILE *db;

//
void addWordToPage(uint32_t[], uint16_t *, uint32_t);
void writePageToFile(uint32_t[], FILE *);
//void writeWordToFile(uint32_t);
void computeMandelValues(void);
uint32_t testPoint(long double, long double);



#endif	//MANDEL_H_