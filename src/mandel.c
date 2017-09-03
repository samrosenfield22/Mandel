/**/

#include "../include/mandel.h"

void pretestMandelValues()
{
	const uint16_t pretestCt = 10;		//test (pretest_ct^2) points on the new frame
	uint32_t iterationHighCutoff, iterationLowCutoff;
	const float xstep = (xmax-xmin)/pretestCt;
	const float ystep = (ymax-ymin)/pretestCt;
	//const uint16_t maxAcceptableHighEscapes = 0.1 * (float)(pretestCt*pretestCt);

	//uint16_t highEscapeCt = 0;
	mvalue_t x,y;
	uint32_t val;
	//uint32_t highestEscape;
	uint32_t highestEscapes[3] = {0, 0, 0};
	uint32_t highestEscapeAvg;
	uint8_t i;

	bool highEscapesDetected = true;
	
	//
	while(highEscapesDetected == true)
	{
		//initialize values
		//highestEscape = 0;
		memset(highestEscapes, 0, 3);
		iterationHighCutoff = ITERATIONS/2;
		iterationLowCutoff = ITERATIONS/5;

		//test points
		for(y=ymax; y>ymin; y-=ystep)
			for(x=xmin; x<xmax; x+=xstep)
			{
				val = testPoint(x,y);

				//if (val>highestEscape && val!=ITERATIONS) highestEscape = val;
				if (val!=ITERATIONS)
				{
					for(i=0; i<3; ++i)
					{
						if (val > highestEscapes[i])
							highestEscapes[i] = val;
					}
				}
			}

		//
		//fprintf(db, "\n-----------PRETEST-----------\nchecking all points w iteration greater than %d (max. iter = %d)\n", iterationHighCutoff, ITERATIONS);
		
		highestEscapeAvg = (highestEscapes[0] + highestEscapes[1] + highestEscapes[2])/3;
		if (highestEscapeAvg > iterationHighCutoff)
		{
			//too high!
			fprintf(db, "iters too high, dropping from %d to %d\n", ITERATIONS, 2*highestEscapeAvg);
			ITERATIONS = 2*highestEscapeAvg;
		}
		else if (highestEscapeAvg < iterationLowCutoff)
		{
			fprintf(db, "iters too low, dropping from %d to %d\n", ITERATIONS, 3*highestEscapeAvg);
			//ITERATIONS = 3*highestEscape;
			ITERATIONS /= 2;
			break;
		}
		else break;

		//fprintf(db, "%d out of %d tested points escaped with high values!\n", highEscapeCt, pretestCt*pretestCt);
		//tooManyHighEscapes = (highEscapeCt > maxAcceptableHighEscapes)? true:false;
		//return tooManyHighEscapes;
	}

}

void computeMandelValues()
{
	mvalue_t x,y;

	uint32_t highestEscapeIter = 0;

	mvalue_t xresolution = (xmax-xmin) / INCR;
	mvalue_t yresolution = (ymax-ymin) / (INCR*0.667);

	//compression-related vars
	uint32_t val, nextVal, repeatCount=1;

	uint32_t page[PAGE_SIZE];
	uint16_t pindex = 0;

	const uint8_t histBins = 50;
	uint32_t histogram[histBins];
	uint32_t histBinSize = ITERATIONS/histBins;
	uint32_t histCount;

	memset(histogram, 0, histBins);

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

		for(x=xmin+xresolution; x<xmax; x+=xresolution)
		{
			nextVal = testPoint(x,y);
			//fprintf(hist, "%d, ", nextVal);
			for(histCount=0; histCount<histBins; ++histCount)
			{
				if (nextVal < (histCount*histBinSize))
				{
					histogram[histCount]++;
					break;
				}
			}

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

	fprintf(db, "highest escaped iteration value was %d\n\n", highestEscapeIter);
	
	for(histCount=0; histCount<histBins; ++histCount)
	{
		fprintf(hist, "%d, %d\n", histCount, histogram[histCount]);
	}

	fclose(mdata);
	fclose(hist);

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

inline uint32_t testPoint(mvalue_t cx, mvalue_t cy)
{
	uint32_t i;
	mvalue_t x, y, xsqr, ysqr, nextx, nexty;

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
