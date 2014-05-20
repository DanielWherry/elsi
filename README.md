striping-benchmark
==================

Tool to mimic I/O patterns for Filesystem Benchmarking

## Goals

- [x] Read / write 1 billion integers in serial.
- [x] Verify that data can be read back in correctly
- [ ] Use `MPI_Wtime` to measure how long reading / writing / verifying takes
- [ ] Use `N` MPI ranks to read/write/verify data in `N` different files.
- [ ] Use OpenMP to speed up generating/verifying data
- [ ] Allow users to specify on command line size of data to write 
- [ ] Use MPI-IO to have each rank read/write chunks of a single file.
- [ ] Enable staggering, so that I/O request happen in groups of `N` nodes.
- [ ] Use the GPU to speed up generating/verifying data
- [ ] Enable fan-in, fan-out so that only certain nodes do I/O and others communicate with them.
- [ ] Enable fan-in and staggering

## Reporting

With each goal, collect performance metric sby varying:

* Filesizes
* Stripe counts
* Job sizes
