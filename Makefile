createFile.o: createFile.c createFile.h
	mpicc -o createFile.o createFile.c -c 
	
createFile.titan.o: createFile.c createFile.h
	cc -o createFile.titan.o createFile.c -c 

verifyFile.o: verifyFile.c verifyFile.h
	mpicc -o verifyFile.o verifyFile.c -c
	
verifyFile.titan.o: verifyFile.c verifyFile.h
	cc -o verifyFile.titan.o verifyFile.c -c

benchmarkFunctions.o: benchmarkFunctions.c benchmarkFunctions.h
	mpicc -o benchmarkFunctions.o benchmarkFunctions.c -c
	
benchmarkFunctions.titan.o: benchmarkFunctions.c benchmarkFunctions.h
	cc -o benchmarkFunctions.titan.o benchmarkFunctions.c -c

striping-benchmark.mac.exe: main.c createFile.o verifyFile.o benchmarkFunctions.o
	mpicc -o striping-benchmark.mac.exe main.c createFile.o verifyFile.o benchmarkFunctions.o 

striping-benchmark.rhea.exe: main.c createFile.o verifyFile.o benchmarkFunctions.o
	mpicc -o striping-benchmark.rhea.exe main.c createFile.o verifyFile.o benchmarkFunctions.o
	cp striping-benchmark.rhea.exe $(MEMBERWORK)/stf007

striping-benchmark.titan.exe: main.c createFile.titan.o verifyFile.titan.o benchmarkFunctions.titan.o
	cc -o striping-benchmark.titan.exe main.c createFile.titan.o verifyFile.titan.o benchmarkFunctions.titan.o -mp 
	cp striping-benchmark.titan.exe $(MEMBERWORK)/stf007

