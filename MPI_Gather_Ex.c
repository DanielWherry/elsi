#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv){

	MPI_Init(&argc, &argv);
	int rank, numProc;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numProc);

	int numNodes = 1;
	long long int i;
	long long int size;
	long long int* sendBuf;
	long long int* recvBuf;
	int ranksPerNode = numProc / numNodes;
	char sizeOfFile[] = "50GB";

	if(ranksPerNode == 1){
		size = (13543545600 * 2) / (numProc * 2); //bytes,  25 GB	
			
	}else if(ranksPerNode > 1 && ranksPerNode <=  4){
		size = 7500000000/ (numProc * 2);//7516192768; //bytes, 7 GB, actually ≈ 3.5

	}else if(ranksPerNode > 4 && ranksPerNode <= 8){
		size = 3500000000 / (numProc * 2); //bytes, 1.5 GB
	
	}else if(ranksPerNode > 8 && ranksPerNode <= 16){
		size = 1500000000/ (numProc); //bytes, .75GB	


	}

	long long int sizeOfFileWithoutLetters, sizeOfArray;
	char* pch = strchr(sizeOfFile, 'K');
	if(pch != NULL){
		char* sizeInString = strtok(sizeOfFile, "K");
		sizeOfFileWithoutLetters = atoi(sizeInString);
		sizeOfFileWithoutLetters *= 1024;// Right hand side is # of bytes in Kilobyte
		sizeOfArray = sizeOfFileWithoutLetters / 8;	
	}
	pch = strchr(sizeOfFile, 'M');
	if(pch != NULL){
		char* sizeInString = strtok(sizeOfFile, "M");
		sizeOfFileWithoutLetters = atoi(sizeInString);
		sizeOfFileWithoutLetters *= 1048576;// Right hand side is # of bytes in Megabyte
		sizeOfArray = sizeOfFileWithoutLetters / 8; 
	}
	pch = strchr(sizeOfFile, 'G');
	if(pch != NULL){
		char* sizeInString = strtok(sizeOfFile, "G");
		sizeOfFileWithoutLetters = atoi(sizeInString);
		sizeOfFileWithoutLetters *= 1073741824;// Right hand side is # of bytes in Gigabyte
		sizeOfArray = sizeOfFileWithoutLetters / 8;
	}
	pch = strchr(sizeOfFile, 'T');
	if(pch != NULL){
		char* sizeInString = strtok(sizeOfFile, "T");
		sizeOfFileWithoutLetters = atoi(sizeInString);
		sizeOfFileWithoutLetters *= 1099511627776; // Right hand side is # of bytes in Terabyte
		sizeOfArray = sizeOfFileWithoutLetters / 8;
	}
	pch = strchr(sizeOfFile, 'B');
	if(pch != NULL){
		char* sizeInString = strtok(sizeOfFile, "B");
		sizeOfFileWithoutLetters = atoi(sizeInString);
		sizeOfArray = sizeOfFileWithoutLetters / 8;
	}

	long long int baseAmount = sizeOfArray / numProc;
	long long int extraWork = sizeOfArray % numProc;
	long long int finalAmount, upperBound, lowerBound;

	if(rank < extraWork){
		finalAmount = baseAmount + 1;
		lowerBound = finalAmount * rank;
		upperBound = lowerBound + baseAmount;
	}else{
		finalAmount = baseAmount;
		lowerBound = finalAmount * rank + extraWork;
		upperBound = lowerBound + baseAmount;
	}

	
	long long int sizeForRank = upperBound - lowerBound + 1;
	long long int numChunks = sizeOfFileWithoutLetters / size;
	printf("SizeNoLetter: %lld, size: %lld\n",sizeOfFileWithoutLetters, size);
	sendBuf = (long long int*) malloc(size);
	if(rank == 0){
		recvBuf = (long long int*) malloc(size * numProc);
	}else{
		recvBuf = NULL;
	}	
	
	if(sizeOfFileWithoutLetters % size == 0){		
	

		for(i = 0; i < numChunks; i++){
			long long int j;
			long long int h = i * size;
			for( j = 0; j < size / 8; j++){
				sendBuf[j] = h;
				h++;
			}
			printf("yodel\n");
						
			MPI_Gather(sendBuf, size, MPI_LONG_LONG_INT, recvBuf, size, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);
		
		}
	}else if(sizeOfFileWithoutLetters % size != 0){
		int numNormalChunks = sizeOfFileWithoutLetters / size;
		int sizeOfExtraChunk = sizeOfFileWithoutLetters % size;

		for(i = 0; i < numChunks; i++){
			long long int j;
			long long int h = i * size;
			for(j = 0; j < size / 8; j++){
				sendBuf[j] = h;
				h++;
			}printf("1\n");
			MPI_Gather(sendBuf, size/ 8, MPI_LONG_LONG_INT, recvBuf, size / 8, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);
			printf("2\n");


			//MPI_File_open(MPI_COMM_WORLD, "MPI_Gather.dat", MPI_MODE_WRONLY|MPI_MODE_CREATE, MPI_INFO_NULL, &outfile);
			//displacement = sizeof(long long int) * rank * size;
			//MPI_File_set_view(outfile, displacement, MPI_LONG_LONG_INT, MPI_LONG_LONG_INT, "native", MPI_INFO_NULL);
			//MPI_File_seek(outfile, offset, MPI_SEEK_SET);
			//MPI_File_write(outfile, recvBuf, size / 8, MPI_LONG_LONG_INT, &status);
			//MPI_File_close(&outfile);
		}
		for(i = numChunks; i < numChunks + 1; i++){
			long long int j;
			long long int h = i * size;
			for(j = 0; j < sizeOfExtraChunk / 8; j++){
				sendBuf[j] = h;
				h++;
			}
			printf("yo\n");
			MPI_Gather(sendBuf, sizeOfExtraChunk / 8 , MPI_LONG_LONG_INT, recvBuf, sizeOfExtraChunk / 8, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);
		}
	} 

	//long long int* integers = (long long int*) malloc(sizeForRank * sizeof(long long int));

	// How much memory that can be allocated by each rank due to the number of ranks per node.
	// I will make these numbers smaller to make sure program doesn't crash
	//
	//
	//
	// 1 rankPerNode long long int size = 26843545600; // 25 GB	
	// 4 rankPerNode long long int size = 7516192768; // 7 GB
	// 8 rankPerNode long long int size = 3758096384; // 3.5 GB
	// 16 rankPerNode long long int size = 1879048192; // 1.75 GB	

	//long long int* recvBuf = (long long int*) malloc( sizeof(long long int) * size * numProc);

	//MPI_Gather(sendBuf, size, MPI_LONG_LONG_INT, recvBuf, size, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);
	free(sendBuf);
	free(recvBuf);
	MPI_Finalize();


	return 0;
}
