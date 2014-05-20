#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char ** argv){

	const int SIZE = 100;
	int* integers;
	integers = (int *)malloc(SIZE*sizeof(int));
	
	if(strcmp(argv[1],"--create") == 0){	
	
		FILE *outfile;

		outfile = fopen(argv[2],"wb");

		for(int i = 0; i < SIZE; i++){
			integers[i] = i;
			//fwrite(integers, sizeof(int), sizeof(integers),outfile);
		}
		fwrite(integers, sizeof(int), SIZE ,outfile);
		printf("Created\n");
	
		free(integers);
		fclose(outfile);
		return 0;
		
	}else if(strcmp(argv[1],"--verify") == 0){

		//int* value;
		FILE *infile;
	
		infile = fopen(argv[2],"r");

		for (int i = 0; i < SIZE; i++){
			if(i != fgetc(infile)){//fread(integers ,sizeof(int), SIZE, infile)){{
				printf("The files are not the same!!\n");
				fclose(infile);
				return 0;
			}
		}
		printf("The files are equivalent!\n");
		fclose(infile);
		printf("Verified\n");
		return 0;

	}	
	
	return 0;
}
	

	
