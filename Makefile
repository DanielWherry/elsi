striping-benchmark.mac.exe: intArrayORNL.c
	mpicc -o striping-benchmark.mac.exe intArrayORNL.c

striping-benchmark.rhea.exe: intArrayORNL.c
	mpicc -o striping-benchmark.rhea.exe intArrayORNL.c
	cp striping-benchmark.rhea.exe $(MEMBERWORK)/stf007

striping-benchmark.titan.exe: intArrayORNL.c
	cc -o striping-benchmark.titan.exe intArrayORNL.c
	cp striping-benchmark.titan.exe $(MEMBERWORK)/stf007
