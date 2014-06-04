striping-benchmark
==================

Tool to mimic I/O patterns for Filesystem Benchmarking

## Goals

- [x] Read / write 1 billion integers in serial.
- [x] Verify that data can be read back in correctly
- [x] Use `MPI_Wtime` to measure how long reading / writing / verifying takes
- [x] Use `N` MPI ranks to read/write/verify data in `N` different files.
- [ ] Use OpenMP to speed up generating/verifying data
- [x] Allow users to specify on command line size of data to write 
- [ ] Use MPI-IO to have each rank read/write chunks of a single file.
- [ ] Enable staggering, so that I/O request happen in groups of `N` nodes.
- [ ] Use the GPU to speed up generating/verifying data
- [ ] Enable fan-in, fan-out so that only certain nodes do I/O and others communicate with them.
- [ ] Enable fan-in and staggering

## Reporting

With each goal, collect performance metrics by varying:

* Filesizes
* Stripe counts
* Job sizes

## Usage

On Titan:

* The program that contains the benchmark can be compiled using the Makefile, with the command 'make striping-benchmark.titan.exe'
* The executable can be submitted to titan with the use of the batch script 'submit.titan.pbs'
* The command line options for the executable are: size of file, '-s 500MB' or '--size 12GB', sizes supported are B, KB, MB, GB, and TB; create/open file, '-c integer' or '--create mydoc', files are automatically .dat;verify the contents of a file, '-v integer' or '--verify mydoc', this just checks to make sure that the create function worked.
* The batch script automatically runs graph.py after the completion of the executable, creating five files: stripingBenchmarkResults.html,OpenTime.png, CloseTime.png, and either ReadTime.png and VerifyTime.png or GenerateTime.png and WriteTime.png.
* stripingBenchmarkResults.html can be opened in any web browser, and contains all 4 images that were created by the python script, as well as statistical information on the runtimes of the program. 

On Rhea:

* The program that contains the benchmark can be compiled using the Makefile, with the command 'make striping-benchmark.rhea.exe'
* The executable can be submitted to titan with the use of the batch script 'submit.rhea.pbs'
* The command line options for the executable are: size of file, '-s 500MB' or '--size 12GB', sizes supported are B, KB, MB, GB, and TB; create/open file, '-c integer' or '--create mydoc', files are automatically .dat;verify the contents of a file, '-v integer' or '--verify mydoc', this just checks to make sure that the create function worked.
* The batch script automatically runs graph.py after the completion of the executable, creating five files: stripingBenchmarkResults.html,OpenTime.png, CloseTime.png, and either ReadTime.png and VerifyTime.png or GenerateTime.png and WriteTime.png.
* stripingBenchmarkResults.html can be opened in any web browser, and contains all 4 images that were created by the python script, as well as statistical information on the runtimes of the program. 
