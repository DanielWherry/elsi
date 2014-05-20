#include <stdio.h>

int main(int argc, char ** argv){

	const int SIZE = 1000000000;
	int integers[SIZE];

	FILE *outfile;

	outfile = fopen(argv[2],"w");

	for(int i = 0; i < SIZE; i++){
		integers[i] = i;
		fprintf(outfile,"%d\n", integers[i]);
	}
	fclose(outfile);
	return 0;
}
	

	
