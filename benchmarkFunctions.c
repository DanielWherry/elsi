#include "benchmarkFunctions.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


//THIS FUNCTIONS CONVERTS A GIVEN FILE SIZE TO THE LENGTH OF AN ARRAY THAT WOULD EQUAL THAT SIZE
long long int setSize(char* commandLineArgument){

	long long int size, sizeOfArray;
	char* sizeInString;
	if(strchr(commandLineArgument, 'K') != NULL){
		sizeInString = strtok(commandLineArgument, "K");
		size = atoi(sizeInString);
		size *= 1024;// Right hand side is # of bytes in Kilobyte
		sizeOfArray = size / 8;	
	}
	else if(strchr(commandLineArgument, 'M') != NULL){
		sizeInString = strtok(commandLineArgument, "M");
		size = atoi(sizeInString);
		size *= 1048576;// Right hand side is # of bytes in Megabyte
		sizeOfArray = size / 8; 
	}
	else if(strchr(commandLineArgument, 'G') != NULL){
		sizeInString = strtok(commandLineArgument, "G");
		size = atoi(sizeInString);
		size *= 1073741824;// Right hand side is # of bytes in Gigabyte
		sizeOfArray = size / 8;
	}
	else if(strchr(commandLineArgument, 'T') != NULL){
		sizeInString = strtok(commandLineArgument, "T");
		size = atoi(sizeInString);
		size *= 1099511627776; // Right hand side is # of bytes in Terabyte
		sizeOfArray = size / 8;
	}
	else if(strchr(commandLineArgument, 'B') != NULL){
		sizeInString = strtok(commandLineArgument, "B");
		size = atoi(sizeInString);
		sizeOfArray = size / 8;
	}


	return sizeOfArray;
}

//THIS FUNCTION SETS THE BOUNDS OF THE ARRAY THAT EACH RANK WILL GENERATE
void setBoundsForRanks(int rank, int numProc, long long int arraySize, long long int* lowerBound, long long int* upperBound){

	long long int baseAmount = arraySize / numProc;
	long long int extraWork = arraySize % numProc;
	long long int finalAmount;

	if(rank < extraWork){
		finalAmount = baseAmount + 1;
		*lowerBound = finalAmount * rank;
		*upperBound = *lowerBound + baseAmount;
	}else{
		finalAmount = baseAmount;
		*lowerBound = finalAmount * rank + extraWork;
		*upperBound = *lowerBound + baseAmount;
	}

}


