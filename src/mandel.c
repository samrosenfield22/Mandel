/**/

#include "../include/mandel.h"



void computeMandelValues()
{
	long double x,y;
	//float xdim, ydim;
	uint32_t uncompMandSize, actualMandSize=0;

	uint32_t highestEscapeIter = 0;

	long double xresolution = (xmax-xmin) / INCR;
	long double yresolution = (ymax-ymin) / (INCR*0.667);

	//compression-related vars
	uint32_t val, nextVal, repeatCount=1;
	uint32_t slowEscapers = 0;

	uint32_t page[PAGE_SIZE];
	uint16_t pindex = 0;



	mdata = fopen("../dat/mdata.txt", "wb");
	fseek(mdata, 0, SEEK_SET);

	//new file for histogram data
	FILE *hist;
	hist = fopen("../dat/hist.csv", "wb");

	
	//compute each point's iteration value, and store the data in
	//a file. blocks of repeating values are compressed, and are stored as:
	//[cmp_start_repeat_set] [repeatcount] [value]
	for(y=ymax; y>ymin; y-=yresolution)
	{
		val = testPoint(xmin, y);
		fprintf(hist, "%d, ", val);

		for(x=xmin+xresolution; x<xmax; x+=xresolution)
		{
			nextVal = testPoint(x,y);
			fprintf(hist, "%d, ", nextVal);
			//if (nextVal > 500 && nextVal != ITERATIONS) slowEscapers++;

			if (val == nextVal)
			{
				repeatCount++;
			}
			else
			{
				//are we in the middle of a repeat set? if so, end it
				if (repeatCount > 1)
				{
					//write the repeat set
					addWordToPage(page, &pindex, CMP_START_REPEAT_SET);
					addWordToPage(page, &pindex, repeatCount);
					addWordToPage(page, &pindex, val);

					//clean up
					val = nextVal;
					repeatCount = 1;
				}
				else
				{
					//writeWordToFile(val);
					addWordToPage(page, &pindex, val);
					val = nextVal;
				}
			}


			if (val != ITERATIONS)
			{
				if (val > highestEscapeIter)
					highestEscapeIter = val;
			}
		}

		//are we in a repeat set at the end of the line? if so, end it
		if (repeatCount > 1)
		{
			//write the repeat set
			addWordToPage(page, &pindex, CMP_START_REPEAT_SET);
			addWordToPage(page, &pindex, repeatCount);
			addWordToPage(page, &pindex, val);

			repeatCount = 1;
		}

		//writeWordToFile(CMP_LINE_FEED);
		addWordToPage(page, &pindex, CMP_LINE_FEED);
	}

	//w/o compression: 167335 lines
	//w compression: 25342 lines
	//6.6x compression ratio (for INCR=1000, ITERATIONS=1000)

	//if we want to do it w/o compression:
	/*for(y=ymax; y>ymin; y-=yresolution)
	{
		for(x=xmin; x<xmax; x+=xresolution)
		{
			val = testPoint(x,y);
			writeWordToFile(val);
		}

		writeWordToFile(0xFFFFFFFE);
	}*/

	//end the block
	addWordToPage(page, &pindex, CMP_END_OF_FILE);
	writePageToFile(page, mdata);

	//actualMandSize++;\

	fprintf(db, "highest escaped iteration value was %d\n\n", highestEscapeIter);
	
	//fprintf(db, "Uncompressed number of inter values:\t%d\n", uncompMandSize);
	//fprintf(db, "Actual number of inter values:\t%d\n", actualMandSize);
	//fprintf(db, "slow escapers: %d out of %d\n", slowEscapers, INCR*INCR*2/3);

	fclose(mdata);
	fclose(hist);

	//return mandVals;
}

void addWordToPage(uint32_t page[PAGE_SIZE], uint16_t *i, uint32_t word)
{
	page[*i] = word;
	(*i)++;

	if (*i >= PAGE_SIZE)
	{
		writePageToFile(page, mdata);
		*i = 0;
	}
}

void writePageToFile(uint32_t page[PAGE_SIZE], FILE *fp)
{
	fwrite(page, 4, PAGE_SIZE, fp);
}

inline uint32_t testPoint(long double cx, long double cy)
{
	uint32_t i;
	long double x, y, xsqr, ysqr, nextx, nexty;

	//initialize the sequence
	x = cx;
	y = cy;

	//Z[n+1] = Z[n]^2 + c
	for(i=0; i<ITERATIONS; ++i)
	{
		
		//precompute x^2, y^2
		xsqr = x*x;
		ysqr = y*y;

		//check if we're out of bounds
		if (xsqr + ysqr >= 4)
			break;

		//compute Z[n]^2
		//(a+bj)(a+bj) = a^2-b^2 + 2abj
		//new a: a^2-b^2		new b: 2ab
		nextx = xsqr - ysqr + cx;
		nexty = 2*x*y + cy;

		//update the sequence
		x = nextx;
		y = nexty;
		
	}

	return i;
}
