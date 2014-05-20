#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char ** argv){

	const int SIZE = 100;
	
	if(strncmp(argv[1],"--create",8)){	
	
		int * integers;
		integers = (int *)malloc(SIZE*sizeof(int));

		FILE *outfile;

		outfile = fopen(argv[2],"w");

		for(int i = 0; i < SIZE; i++){
			integers[i] = i;
			fprintf(outfile,"%d\n", integers[i]);
		}
	
		fclose(outfile);
		return 0;
	}
	
	if(strncmp(argv[1],"--verify",8)){

		int* value;
		FILE *infile;
	
		infile = fopen(argv[2],"r");

		for (int i = 0; i < SIZE; i++){
			if(i != fscanf(infile,"%d",value)){
				printf("The files are not the same!!\n");
				fclose(infile);
				return 0;
			}
		}
		printf("The files are equivalent!\n");
		fclose(infile);
		return 0;
	}	
	
}
	

	
