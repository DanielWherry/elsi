createFile.o: createFile.c createFile.h
	mpicc -o createFile.o createFile.c -c 
	
createFile.titan.o: createFile.c createFile.h
	cc -o createFile.titan.o createFile.c -c 

verifyFile.o: verifyFile.c verifyFile.h
	mpicc -o verifyFile.o verifyFile.c -c
	
benchmarkFunctions.o: benchmarkFunctions.c benchmarkFunctions.h
	mpicc -o benchmarkFunctions.o benchmarkFunctions.c -c
	
benchmarkFunctions.titan.o: benchmarkFunctions.c benchmarkFunctions.h
	cc -o benchmarkFunctions.titan.o benchmarkFunctions.c -c

elsi.mac.exe: main.c createFile.o verifyFile.o benchmarkFunctions.o
	mpicc -o elsi.mac.exe main.c createFile.o verifyFile.o benchmarkFunctions.o 

elsi.rhea.exe: main.c createFile.o verifyFile.o benchmarkFunctions.o
	mpicc -o elsi.rhea.exe main.c createFile.o verifyFile.o benchmarkFunctions.o
	cp .rhea.exe $(MEMBERWORK)/stf007

elsi.titan.exe: main.c createFile.titan.o benchmarkFunctions.titan.o
	cc -o elsi.titan.exe main.c createFile.titan.o  benchmarkFunctions.titan.o -mp 

install:  	 
	mkdir $(PREFIX)/bin 
	mkdir $(PREFIX)/man 
	install elsi.titan.exe $(PREFIX)/bin
	install elsiGraph.py $(PREFIX)/bin
	install -m 775 elsi $(PREFIX)/bin
	install elsi.1 $(PREFIX)/man




