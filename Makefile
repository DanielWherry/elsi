striping-benchmark.mac.exe: main.c
	mpicc -o striping-benchmark.mac.exe main.c 

striping-benchmark.rhea.exe: main.c
	mpicc -o striping-benchmark.rhea.exe main.c
	cp striping-benchmark.rhea.exe $(MEMBERWORK)/stf007

striping-benchmark.titan.exe: main.c
	cc -o striping-benchmark.titan.exe main.c -mp 
	cp striping-benchmark.titan.exe $(MEMBERWORK)/stf007
submit.titan.pbs:
	cp submit.titan.pbs $(MEMBERWORK)/stf007
