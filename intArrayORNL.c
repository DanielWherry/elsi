#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char ** argv){

	const int SIZE = 100;
	int* integers;
	integers = (int *)malloc(SIZE*sizeof(int));
	
	if(strcmp(argv[1],"--create") == 0){	
	
		FILE *outfile;

		outfile = fopen(argv[2],"w");

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

		int ch1, i;
		i = 0;
		FILE *infile;
	
		infile = fopen(argv[2],"r");
		ch1 = getc(infile);

		while((ch1 = fgetc(infile)) != EOF){
			printf("This is from loop: %d\n",i);
			if(ch1 != i ){	
				printf("This is from file: %d\n", ch1);
				printf("This is from loop: %d\n",i);			
				printf("The files are not the same!!\n");
				free(integers);
				fclose(infile);
				return 0;
				
			}i++;
		}
	
		
		printf("The files are equivalent!\n");
		free(integers);
		printf("Verified\n");
		fclose(infile);
		return 0;

	}	
	
	return 0;
}
	

	
