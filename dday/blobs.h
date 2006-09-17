//***********************************************************//
//* Blob analysis package  Version1.1 28 December 2003      *//
//* Added:                                                  *//
//* - BLOBCOLOR                                             *//
//* History:                                                *//
//* - Version 1.0 8 August 2003                             *//
//*                                                         *//
//* Input: IplImage* binary image                           *//
//* Output: attributes of each connected region             *//
//* Author: Dave Grossman                                   *//
//* Email: dgrossman@cdr.stanford.edu                       *//
//* Acknowledgement: the algorithm has been around > 20 yrs *//
//***********************************************************//

// defines for blob array sizes and indices
#define BLOBTOTALCOUNT 8000
#define BLOBROWCOUNT 1000
#define BLOBCOLCOUNT 1000

#define BLOBPARENT 0
#define BLOBCOLOR 1
#define BLOBAREA 2
#define BLOBPERIMETER 3
#define BLOBSUMX 4
#define BLOBSUMY 5
#define BLOBSUMXX 6
#define BLOBSUMYY 7
#define BLOBSUMXY 8
#define BLOBMINX 9
#define BLOBMAXX 10
#define BLOBMINY 11
#define BLOBMAXY 12

#define BLOBDATACOUNT 13

// Subroutine prototypes
void PrintRegionDataArray(float*[BLOBDATACOUNT]);
void Subsume(float*[BLOBDATACOUNT], int, int*, int, int);
int BlobAnalysis(IplImage*, int*, float*[BLOBDATACOUNT], int, int, uchar, int);

// Transfer fields from subsumed region to correct one
void Subsume(float(*RegionData)[BLOBDATACOUNT],
			int HighRegionNum,
			int* SubsumedRegion,
			int HiNum,
			int LoNum)
{
	// cout << "\nSubsuming " << HiNum << " into " << LoNum << endl; // for debugging

	int i;
	for(i = BLOBAREA; i < BLOBDATACOUNT; i++)	// Skip over BLOBCOLOR
	{
		if(i == BLOBMINX || i == BLOBMINY)
		{
			if(RegionData[LoNum][i] > RegionData[HiNum][i]) { RegionData[LoNum][i] = RegionData[HiNum][i]; }
		}
		else if(i == BLOBMAXX || i == BLOBMAXY)
		{
		 	if(RegionData[LoNum][i] < RegionData[HiNum][i]) { RegionData[LoNum][i] = RegionData[HiNum][i]; }
		}
		else // Area, Perimeter, SumX, SumY, SumXX, SumYY, SumXY
		{
			RegionData[LoNum][i] += RegionData[HiNum][i];
		}
	}
	
	// Make sure no region still has subsumed region as parent
	for(i = HiNum + 1; i <= HighRegionNum; i++)
	{
		if(RegionData[i][BLOBPARENT] == (float) HiNum) { RegionData[i][BLOBPARENT] = LoNum; }
	}

	// Mark dead region number for future compression
	SubsumedRegion[HiNum] = 1;
}

// Print region data array
void PrintRegionDataArray(float (*RegionData)[BLOBDATACOUNT])
{
	cout << "RegionData array:" << endl;
	for(int ThisRegion = 0; ThisRegion < BLOBTOTALCOUNT; ThisRegion++)
	{
		if(ThisRegion > 0 && RegionData[ThisRegion][0] < 0) break;
		if(RegionData[ThisRegion][BLOBAREA] < 0) break;
		cout << "Region=" << ThisRegion << ": ";
		for(int ThisData = 0; ThisData < BLOBDATACOUNT; ThisData++)
		{
			float Data = RegionData[ThisRegion][ThisData];
			cout << Data << " " ;
		}
		cout << endl;
	}
	cout << endl;
}

int BlobAnalysis(IplImage* ImageHdr,	// input image
	int* Transition,					// transition array for intermediate result
	float (*RegionData)[BLOBDATACOUNT],	// region data array to be output
	int Cols, int Rows,					// size of input image (+2 for Tran array)
	uchar Border,						// border color
	int MinArea)						// max trans in any row
{

	if(Cols > BLOBCOLCOUNT) 
	{
		cout << "Error in column count" << endl;
		return(-1);
	}
	if(Rows > BLOBROWCOUNT) 
	{
		cout << "Error in row count" << endl;
		return(-1);
	}
	
	char* Image = ImageHdr->imageData;
	int WidthStep = ImageHdr->widthStep; 

	// Convert image array into transition array. In each row
	// the transition array tells which columns have a color change
	int Trans = Cols;				// max trans in any row

	// row 0 and row Rows+1 represent the border
	int iTran, Tran;				// Data for a given run
	uchar ThisCell, LastCell;		// Contents (colors?) within this row
	int ImageOffset = - WidthStep - 1;	// Performance booster to avoid multiplication
	int TransitionOffset = 0;		// Performance booster to avoid multiplication
	int iRow, iCol;
	int i;

	// Initialize Transition array
	for(i = 0; i < (Rows + 2) * (Cols + 2); i++) { Transition[i] = 0; };
	Transition[0] = Transition[(Rows + 1) * (Cols + 2)] = Cols + 2;

	// Fill Transition array
	for(iRow = 1; iRow < Rows + 1; iRow++)		// Choose a row of Bordered image
	{
		ImageOffset += WidthStep;	// Performance booster to avoid multiplication
		TransitionOffset += Cols + 2;// Performance booster to avoid multiplication
		iTran = 0;					// Index into Transition array
		Tran = 0;					// No transitions at row start
		LastCell = Border;
		for(iCol = 0; iCol < Cols + 2; iCol++)	// Scan that row of Bordered image
		{
			if(iCol == 0 || iCol == Cols + 1) ThisCell = Border;
			else ThisCell = Image[ImageOffset + iCol];

			if(ThisCell != LastCell)
			{
				Transition[TransitionOffset + iTran] = Tran;	// Save completed Tran
				iTran++;						// Prepare new index
				LastCell = ThisCell;			// With this color
			}
			Tran++;	// Tran continues
		}
		Transition[TransitionOffset + iTran] = Tran;	// Save completed run
		Transition[TransitionOffset + iTran + 1] = -1;
	}


	// Process transition code depending on Last row and This row
	//
	// Last ---++++++--+++++++++++++++-----+++++++++++++++++++-----++++++-------+++---
	// This -----+++-----++++----+++++++++----+++++++---++------------------++++++++--
	//
	// There are various possibilities:
	//
	// Case     1       2       3       4       5       6       7       8
	// Last |xxx    |xxxxoo |xxxxxxx|xxxxxxx|ooxxxxx|ooxxx  |ooxxxxx|    xxx|
	// This |    yyy|    yyy|  yyyy |  yyyyy|yyyyyyy|yyyyyyy|yyyy   |yyyy   |
	// Here o is optional
	// 
	// Here are the primitive tests to distinguish these 6 cases:
	//   A) Last end < This start - 1 OR NOT		Note: -1
	//   B) This end < Last start OR NOT
	//   C) Last start < This start OR NOT
	//   D) This end < Last end OR NOT
	//   E) This end = Last end OR NOT
	//
	// Here is how to use these tests to determine the case:
	//   Case 1 = A [=> NOT B AND C AND NOT D AND NOT E]
	//   Case 2 = C AND NOT D AND NOT E [AND NOT A AND NOT B]
	//   Case 3 = C AND D [=> NOT E] [AND NOT A AND NOT B]
	//   Case 4 = C AND NOT D AND E [AND NOT A AND NOT B]
	//   Case 5 = NOT C AND E [=> NOT D] [AND NOT A AND NOT B]
	//   Case 6 = NOT C AND NOT D AND NOT E [AND NOT A AND NOT B]
	//   Case 7 = NOT C AND D [=> NOT E] [AND NOT A AND NOT B]
	//   Case 8 = B [=> NOT A AND NOT C AND D AND NOT E]
	//
	// In cases 2,3,4,5,6,7 the following additional test is needed:
	//   Match) This color = Last color OR NOT
	//
	// In cases 5,6,7 the following additional test is needed:
	//   Known) This region was already matched OR NOT
	//
	// Here are the main tests and actions:
	//   Case 1: LastIndex++;
	//   Case 2: if(Match) {y = x;}
	//           LastIndex++;
	//   Case 3: if(Match) {y = x;}
	//           else {y = new}
	//           ThisIndex++;
	//   Case 4: if(Match) {y = x;}
	//           else {y = new}
	//           LastIndex++;
	//           ThisIndex++;
	//   Case 5: if(Match AND NOT Known) {y = x}
	//           else if(Match AND Known) {Subsume(x,y)}
	//           LastIndex++;ThisIndex++
	//   Case 6: if(Match AND NOT Known) {y = x}
	//           else if(Match AND Known) {Subsume(x,y)}
	//           LastIndex++;
	//   Case 7: if(Match AND NOT Known) {y = x}
	//           else if(Match AND Known) {Subsume(x,y)}
	//           ThisIndex++;
	//   Case 8: ThisIndex++;

	// BLOBTOTALCOUNT is max num of regions incl all temps and background
	// BLOBROWCOUNT is the number of rows in the image
	// BLOBCOLCOUNT is the number of columns in the image
	// BLOBDATACOUNT is number of data elements for each region as follows:
	// BLOBPARENT 0	these are the respective indices for the data elements
	// BLOBCOLOR 1		0=background; 1=non-background
	// BLOBAREA 2
	// BLOBPERIMETER 3
	// BLOBSUMX 4		means
	// BLOBSUMY 5
	// BLOBSUMXX 6		2nd moments
	// BLOBSUMYY 7
	// BLOBSUMXY 8
	// BLOBMINX 9		bounding rectangle
	// BLOBMAXX 10
	// BLOBMINY 11
	// BLOBMAXY 12

	int SubsumedRegion[BLOBTOTALCOUNT];			// Blob result array
	
	float ThisParent;	// These data can change when the line is current
	float ThisArea;
	float ThisPerimeter;
	float ThisSumX;
	float ThisSumY;
	float ThisSumXX;
	float ThisSumYY;
	float ThisSumXY;
	float ThisMinX;
	float ThisMaxX;
	float ThisMinY;
	float ThisMaxY;
	float LastPerimeter;	// This is the only data for retroactive change
	
	int HighRegionNum = 0;
	int RegionNum = 0;
	int ErrorFlag = 0;
	
	int LastRow, ThisRow;			// Row number
	int LastStart, ThisStart;		// Starting column of run
	int LastEnd, ThisEnd;			// Ending column of run
	int LastColor, ThisColor;		// Color of run
	
	int LastIndex, ThisIndex;		// Which run are we up to?
	int LastIndexCount, ThisIndexCount;	// Out of these runs
	int LastRegionNum, ThisRegionNum;	// Which assignment?
	int LastRegion[BLOBCOLCOUNT+2];	// Row assignment of region number
	int ThisRegion[BLOBCOLCOUNT+2];	// Row assignment of region number
	
	int LastOffset = -(Trans + 2);	// For performance to avoid multiplication
	int ThisOffset = 0;				// For performance to avoid multiplication
	int ComputeData;

	for(i = 0; i < BLOBTOTALCOUNT; i++)	// Initialize result arrays
	{
		SubsumedRegion[i] = 0;				// Flag indicates region is not subsumed
		RegionData[i][0] = (float) -1;		// Flag indicates null region
		for(int j = 1; j < BLOBDATACOUNT; j++)
		{
			if(j == BLOBMINX || j == BLOBMINY) RegionData[i][j] = (float) 1000000.0;
			else RegionData[i][j] = (float) 0.0;
		}
	}
	for(i = 0; i < BLOBROWCOUNT + 2; i++)	// Initialize result arrays
	{
		LastRegion[i] = -1;
		ThisRegion[i] = -1;
	}
	RegionData[0][BLOBPARENT] = (float) -1;
	RegionData[0][BLOBAREA] = (float) Transition[0];
	RegionData[0][BLOBPERIMETER] = (float) (2 + 2 * Transition[0]);

	ThisIndexCount = 1;
	ThisRegion[0] = 0;	// Border region

	// Initialize left border column
	for(i = 1; i < Rows + 2; i++) { ThisRegion[i] = -1; } // Flag as uninit
	
	// Loop over all rows
	for(ThisRow = 1; ThisRow < Rows + 2; ThisRow++)
	{
		//cout << "========= THIS ROW = " << ThisRow << endl;	// for debugging
	
		ThisOffset += Trans + 2;
		ThisIndex = 0;
		
		LastOffset += Trans + 2;
		LastRow = ThisRow - 1;
		LastIndexCount = ThisIndexCount;
		LastIndex = 0;

		int EndLast = 0;
		int EndThis = 0;
		for(int j = 0; j < Trans + 2; j++)
		{
			int Index = ThisOffset + j;
			int TranVal = Transition[Index];
			if(TranVal > 0) ThisIndexCount = j + 1;	// stop at highest 

			if(ThisRegion[j] == -1)  { EndLast = 1; }
			if(TranVal < 0) { EndThis = 1; }

			if(EndLast > 0 && EndThis > 0) { break; }

			LastRegion[j] = ThisRegion[j];
			ThisRegion[j] = -1;		// Flag indicates region is not initialized
		}

		int MaxIndexCount = LastIndexCount;
		if(ThisIndexCount > MaxIndexCount) MaxIndexCount = ThisIndexCount;

		// Main loop over runs within Last and This rows
		while (LastIndex < LastIndexCount && ThisIndex < ThisIndexCount)
		{
			ComputeData = 0;
		
			if(LastIndex == 0) LastStart = 0;
			else LastStart = Transition[LastOffset + LastIndex - 1];
			LastEnd = Transition[LastOffset + LastIndex] - 1;
			LastColor = LastIndex - 2 * (LastIndex / 2);
			LastRegionNum = LastRegion[LastIndex];
			
			if(ThisIndex == 0) ThisStart = 0;
			else ThisStart = Transition[ThisOffset + ThisIndex - 1];
			ThisEnd = Transition[ThisOffset + ThisIndex] - 1;
			ThisColor = ThisIndex - 2 * (ThisIndex / 2);
			ThisRegionNum = ThisRegion[ThisIndex];

if((LastColor != 1 && LastColor != 0) || (ThisColor != 1 && ThisColor != 0))
{
	cout << "1) LastColor=" << LastColor << "      ThisColor=" << ThisColor << endl;
}


			int TestA = (LastEnd < ThisStart - 1);	// initially false
			int TestB = (ThisEnd < LastStart);		// initially false
			int TestC = (LastStart < ThisStart);	// initially false
			int TestD = (ThisEnd < LastEnd);
			int TestE = (ThisEnd == LastEnd);

			int TestMatch = (ThisColor == LastColor);		// initially true
			int TestKnown = (ThisRegion[ThisIndex] >= 0);	// initially false

			int Case = 0;
			if(TestA) Case = 1;
			else if(TestB) Case = 8;
			else if(TestC)
			{
				if(TestD) Case = 3;
				else if(!TestE) Case = 2;
				else Case = 4;
			}
			else
			{
				if(TestE) Case = 5;
				else if(TestD) Case = 7;
				else Case = 6;
			}

			// Initialize common variables
			ThisArea = (float) 0.0;
			ThisSumX = ThisSumY = (float) 0.0;
			ThisSumXX = ThisSumYY = ThisSumXY = (float) 0.0;
			ThisMinX = ThisMinY = (float) 1000000.0;
			ThisMaxX = ThisMaxY = (float) -1.0;
			LastPerimeter = ThisPerimeter = (float) 0.0;
			ThisParent = (float) -1;

			// Determine necessary action and take it
			switch (Case)
			{ 
				case 1: //|xxx    |
						//|    yyy|
					
					ThisRegion[ThisIndex] = ThisRegionNum;
					LastRegion[LastIndex] = LastRegionNum;
					LastIndex++;
					break;
					
					
				case 2: //|xxxxoo |
						//|    yyy|
					
					if(TestMatch)	// Same color
					{
						ThisRegionNum = LastRegionNum;
						ThisArea = ThisEnd - ThisStart + 1;
						LastPerimeter = LastEnd - ThisStart + 1;	// to subtract
						ThisPerimeter = 2 + 2 * ThisArea - LastPerimeter;
						ComputeData = 1;
					}
					
					ThisRegion[ThisIndex] = ThisRegionNum;
					LastRegion[LastIndex] = LastRegionNum;
					LastIndex++;
					break;
					
					
				case 3: //|xxxxxxx|
						//|  yyyy |
					
					if(TestMatch)	// Same color
					{
						ThisRegionNum = LastRegionNum;
						ThisArea = ThisEnd - ThisStart + 1;
						LastPerimeter = ThisArea;	// to subtract
						ThisPerimeter = 2 + ThisArea;
					}
					else		// Different color => New region
					{
						ThisParent = LastRegionNum;
						ThisRegionNum = ++HighRegionNum;
						ThisArea = ThisEnd - ThisStart + 1;
						ThisPerimeter = 2 + 2 * ThisArea;
					}
					
					ThisRegion[ThisIndex] = ThisRegionNum;
					LastRegion[LastIndex] = LastRegionNum;
					ComputeData = 1;
					ThisIndex++;
					break;
					
					
				case 4:	//|xxxxxxx|
						//|  yyyyy|
					
					if(TestMatch)	// Same color
					{
						ThisRegionNum = LastRegionNum;
						ThisArea = ThisEnd - ThisStart + 1;
						LastPerimeter = ThisArea;	// to subtract
						ThisPerimeter = 2 + ThisArea;
					}
					else		// Different color => New region
					{
						ThisParent = LastRegionNum;
						ThisRegionNum = ++HighRegionNum;
						ThisArea = ThisEnd - ThisStart + 1;
						ThisPerimeter = 2 + 2 * ThisArea;
					}
					
					ThisRegion[ThisIndex] = ThisRegionNum;
					LastRegion[LastIndex] = LastRegionNum;
					ComputeData = 1;
					LastIndex++;
					ThisIndex++;
					break;
					
					
				case 5:	//|ooxxxxx|
						//|yyyyyyy|
					
					if(!TestMatch && !TestKnown)	// Different color and unknown => new region
					{
						ThisParent = LastRegionNum;
						ThisRegionNum = ++HighRegionNum;
						ThisArea = ThisEnd - ThisStart + 1;
						ThisPerimeter = 2 + 2 * ThisArea;
					}
					else if(TestMatch && !TestKnown)	// Same color and unknown
					{
						ThisRegionNum = LastRegionNum;
						ThisArea = ThisEnd - ThisStart + 1;
						LastPerimeter = LastEnd - LastStart + 1;	// to subtract
						ThisPerimeter = 2 + 2 * ThisArea - LastPerimeter;
						ComputeData = 1;
					}
					else if(TestMatch && TestKnown)	// Same color and known
					{
						LastPerimeter = LastEnd - LastStart + 1;	// to subtract
						ThisPerimeter = - LastPerimeter;
						if(ThisRegionNum > LastRegionNum)
						{
							Subsume(RegionData, HighRegionNum, SubsumedRegion, ThisRegionNum, LastRegionNum);
							for(int iOld = 0; iOld < MaxIndexCount; iOld++)
							{
								if(ThisRegion[iOld] == ThisRegionNum) ThisRegion[iOld] = LastRegionNum;
								if(LastRegion[iOld] == ThisRegionNum) LastRegion[iOld] = LastRegionNum;
							}					
							ThisRegionNum = LastRegionNum;
						}
						else if(ThisRegionNum < LastRegionNum)
						{
							Subsume(RegionData, HighRegionNum, SubsumedRegion, LastRegionNum, ThisRegionNum);
							for(int iOld = 0; iOld < MaxIndexCount; iOld++)
							{
								if(ThisRegion[iOld] == LastRegionNum) ThisRegion[iOld] = ThisRegionNum;
								if(LastRegion[iOld] == LastRegionNum) LastRegion[iOld] = ThisRegionNum;
							}					
							LastRegionNum = ThisRegionNum;
						}
					}

					ThisRegion[ThisIndex] = ThisRegionNum;
					LastRegion[LastIndex] = LastRegionNum;
					LastIndex++;
					ThisIndex++;
					break;
					
					
				case 6:	//|ooxxx  |
						//|yyyyyyy|

					if(TestMatch && !TestKnown)
					{
						ThisRegionNum = LastRegionNum;
						ThisArea = ThisEnd - ThisStart + 1;
						LastPerimeter = LastEnd - LastStart + 1;	// to subtract
						ThisPerimeter = 2 + 2 * ThisArea - LastPerimeter;
						ComputeData = 1;
					}
					else if(TestMatch && TestKnown)
					{
						LastPerimeter = LastEnd - LastStart + 1;	// to subtract
						ThisPerimeter = - LastPerimeter;
						if(ThisRegionNum > LastRegionNum)
						{
							Subsume(RegionData, HighRegionNum, SubsumedRegion, ThisRegionNum, LastRegionNum);
							for(int iOld = 0; iOld < MaxIndexCount; iOld++)
							{
								if(ThisRegion[iOld] == ThisRegionNum) ThisRegion[iOld] = LastRegionNum;
								if(LastRegion[iOld] == ThisRegionNum) LastRegion[iOld] = LastRegionNum;
							}					
							ThisRegionNum = LastRegionNum;
						}
						else if(ThisRegionNum < LastRegionNum)
						{
							Subsume(RegionData, HighRegionNum, SubsumedRegion, LastRegionNum, ThisRegionNum);
							for(int iOld = 0; iOld < MaxIndexCount; iOld++)
							{
								if(ThisRegion[iOld] == LastRegionNum) ThisRegion[iOld] = ThisRegionNum;
								if(LastRegion[iOld] == LastRegionNum) LastRegion[iOld] = ThisRegionNum;
							}					
							LastRegionNum = ThisRegionNum;
						}
					}

					ThisRegion[ThisIndex] = ThisRegionNum;
					LastRegion[LastIndex] = LastRegionNum;
					LastIndex++;
					break;
					
					
				case 7:	//|ooxxxxx|
						//|yyyy   |
					
					if(!TestMatch && !TestKnown)	// Different color and unknown => new region
					{
						ThisParent = LastRegionNum;
						ThisRegionNum = ++HighRegionNum;
						ThisArea = ThisEnd - ThisStart + 1;
						ThisPerimeter = 2 + 2 * ThisArea;
					}
					else if(TestMatch && !TestKnown)
					{
						ThisRegionNum = LastRegionNum;
						ThisArea = ThisEnd - ThisStart + 1;
						ThisPerimeter = 2 + ThisArea;
						LastPerimeter = ThisEnd - LastStart + 1;
						ThisPerimeter = 2 + 2 * ThisArea - LastPerimeter;
						ComputeData = 1;
					}
					else if(TestMatch && TestKnown)
					{
						LastPerimeter = ThisEnd - LastStart + 1;	// to subtract
						ThisPerimeter = - LastPerimeter;
						if(ThisRegionNum > LastRegionNum)
						{
							Subsume(RegionData, HighRegionNum, SubsumedRegion, ThisRegionNum, LastRegionNum);
							for(int iOld = 0; iOld < MaxIndexCount; iOld++)
							{
								if(ThisRegion[iOld] == ThisRegionNum) ThisRegion[iOld] = LastRegionNum;
								if(LastRegion[iOld] == ThisRegionNum) LastRegion[iOld] = LastRegionNum;
							}					
							ThisRegionNum = LastRegionNum;
						}
						else if(ThisRegionNum < LastRegionNum)
						{
							Subsume(RegionData, HighRegionNum, SubsumedRegion, LastRegionNum, ThisRegionNum);
							for(int iOld = 0; iOld < MaxIndexCount; iOld++)
							{
								if(ThisRegion[iOld] == LastRegionNum) ThisRegion[iOld] = ThisRegionNum;
								if(LastRegion[iOld] == LastRegionNum) LastRegion[iOld] = ThisRegionNum;
							}					
							LastRegionNum = ThisRegionNum;
						}
					}

					ThisRegion[ThisIndex] = ThisRegionNum;
					LastRegion[LastIndex] = LastRegionNum;
					ThisIndex++;
					break;
					
				case 8:	//|    xxx|
						//|yyyy   |
					
					ThisRegion[ThisIndex] = ThisRegionNum;
					LastRegion[LastIndex] = LastRegionNum;
					ThisIndex++;
					break;
					
				default:
					ErrorFlag = -1;
			}	// end switch case

			if(ComputeData > 0)
			{
				for(int k = ThisStart; k <= ThisEnd; k++)
				{
					ThisSumX += (float) (k - 1);
					ThisSumXX += (float) (k - 1) * (k - 1);
				}
				float ImageRow = (float) (ThisRow - 1);

				ThisSumXY = ThisSumX * ImageRow;
				ThisSumY = ThisArea * ImageRow;
				ThisSumYY = ThisSumY * ImageRow;
					
				if(ThisStart - 1 < (int) ThisMinX) ThisMinX = (float) (ThisStart - 1);
				if(ThisMinX < (float) 0.0) ThisMinX = (float) 0.0;
				if(ThisEnd - 1 > (int) ThisMaxX) ThisMaxX = (float) (ThisEnd - 1);

				if(ImageRow < ThisMinY) ThisMinY = ImageRow;
				if(ThisMinY < (float) 0.0) ThisMinY = (float) 0.0;
				if(ImageRow > ThisMaxY) ThisMaxY = ImageRow;
			}

			if(ThisRegionNum >= 0)
			{
				if(ThisParent >= 0) { RegionData[ThisRegionNum][BLOBPARENT] = (float) ThisParent; }
				RegionData[ThisRegionNum][BLOBCOLOR] = (float) ThisColor;	// New code
				RegionData[ThisRegionNum][BLOBAREA] += ThisArea;
				RegionData[ThisRegionNum][BLOBPERIMETER] += ThisPerimeter;
				
				if(ComputeData > 0)
				{
					RegionData[ThisRegionNum][BLOBSUMX] += ThisSumX;
					RegionData[ThisRegionNum][BLOBSUMY] += ThisSumY;
					RegionData[ThisRegionNum][BLOBSUMXX] += ThisSumXX;
					RegionData[ThisRegionNum][BLOBSUMYY] += ThisSumYY;
					RegionData[ThisRegionNum][BLOBSUMXY] += ThisSumXY;
					RegionData[ThisRegionNum][BLOBPERIMETER] -= LastPerimeter;
					if(RegionData[ThisRegionNum][BLOBMINX] > ThisMinX) RegionData[ThisRegionNum][BLOBMINX] = ThisMinX;
					if(RegionData[ThisRegionNum][BLOBMAXX] < ThisMaxX) RegionData[ThisRegionNum][BLOBMAXX] = ThisMaxX;
					if(RegionData[ThisRegionNum][BLOBMINY] > ThisMinY) RegionData[ThisRegionNum][BLOBMINY] = ThisMinY;
					if(RegionData[ThisRegionNum][BLOBMAXY] < ThisMaxY) RegionData[ThisRegionNum][BLOBMAXY] = ThisMaxY;
				}
			}
		}	// end Main loop

		if(ErrorFlag != 0) return(ErrorFlag);
	}	// end Loop over all rows

	// Subsume regions that have too small area
	for(int HiNum = HighRegionNum; HiNum > 0; HiNum--)
	{
		if(SubsumedRegion[HiNum] < 1 && RegionData[HiNum][BLOBAREA] < (float) MinArea)
		{
			Subsume(RegionData, HighRegionNum, SubsumedRegion, HiNum, (int) RegionData[HiNum][BLOBPARENT]);
		}
	}

	// Condense the list
	int iNew = 0;
	for(int iOld = 0; iOld <= HighRegionNum; iOld++)
	{
		if(SubsumedRegion[iOld] < 1)	// This number not subsumed
		{
			// Move data from old region number to new region number
			for(int j = 0; j < BLOBDATACOUNT; j++) { RegionData[iNew][j] = RegionData[iOld][j]; }

			// Update and parent pointer if necessary
			for(int i = 0; i <= HighRegionNum; i++)
			{
				if(RegionData[i][BLOBPARENT] == iOld) { RegionData[i][BLOBPARENT] = iNew; }
			}
			iNew++;
		}	
	}
	HighRegionNum = iNew - 1;				// Update where the data ends
	RegionData[HighRegionNum + 1][0] = -1;	// and set end of array flag

	// Normalize summation fields into moments 
	for(ThisRegionNum = 0; ThisRegionNum <= HighRegionNum; ThisRegionNum++)
	{
		// Extract fields
		float Area = RegionData[ThisRegionNum][BLOBAREA];
		float SumX = RegionData[ThisRegionNum][BLOBSUMX];
		float SumY = RegionData[ThisRegionNum][BLOBSUMY];
		float SumXX = RegionData[ThisRegionNum][BLOBSUMXX];
		float SumYY = RegionData[ThisRegionNum][BLOBSUMYY];
		float SumXY = RegionData[ThisRegionNum][BLOBSUMXY];
	
		// Get averages
		SumX /= Area;
		SumY /= Area;
		SumXX /= Area;
		SumYY /= Area;
		SumXY /= Area;

		// Create moments
		SumXX -= SumX * SumX;
		SumYY -= SumY * SumY;
		SumXY -= SumX * SumY;
		if(SumXY > -1.0E-14 && SumXY < 1.0E-14)
		{
			SumXY = (float) 0.0; // Eliminate roundoff error
		}
		RegionData[ThisRegionNum][BLOBSUMX] = SumX;
		RegionData[ThisRegionNum][BLOBSUMY] = SumY;
		RegionData[ThisRegionNum][BLOBSUMXX] = SumXX;
		RegionData[ThisRegionNum][BLOBSUMYY] = SumYY;
		RegionData[ThisRegionNum][BLOBSUMXY] = SumXY;
	}

	for(ThisRegionNum = HighRegionNum; ThisRegionNum > 0 ; ThisRegionNum--)
	{
		// Subtract interior perimeters
		int ParentRegionNum = (int) RegionData[ThisRegionNum][BLOBPARENT];
		RegionData[ParentRegionNum][BLOBPERIMETER]
			-= RegionData[ThisRegionNum][BLOBPERIMETER];
	}

	return(HighRegionNum);
}

