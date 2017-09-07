/**/

#ifndef MANDEL_H_
#define MANDEL_H_


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

//typedef long double mvalue_t;
typedef double mvalue_t;
/*#include <quadmath.h>
#ifdef __float128
	typedef __float128 mvalue_t;
#else
	typedef long double mvalue_t;
#endif*/

#define INCR 1000		//number of points (on the x-axis, 2/3rds of this value on the y-axis)

#define XMAX_INITIAL 1.0
#define XMIN_INITIAL -2.0
#define YMAX_INITIAL 1.0
#define YMIN_INITIAL -1.0

#define ITERATIONS_INITIAL 600
//highest value in home zoom was 190968 (!!!) when initial value was 600,000

#define PAGE_SIZE 16834

//#define EXPORT_HIST_DATA true


enum COMPRESSION_CONSTS
{
	CMP_START_REPEAT_SET = 0xFFFFFFFD,
	CMP_LINE_FEED = 0xFFFFFFFE,
	CMP_END_OF_FILE = 0xFFFFFFFF
};

extern mvalue_t xmax, xmin, ymax, ymin;

extern uint32_t ITERATIONS;
extern uint16_t *mandVals;
extern FILE *mdata;
extern FILE *db;

//
void addWordToPage(uint32_t[], uint16_t *, uint32_t);
void writePageToFile(uint32_t[], FILE *);
//void writeWordToFile(uint32_t);
void pretestMandelValues(void);
void computeMandelValues(void);
uint32_t testPoint(mvalue_t, mvalue_t);



#endif	//MANDEL_H_