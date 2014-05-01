striping-benchmark
==================

Tool to mimic I/O patterns for Filesystem Benchmarking

## Goals

1. Read / write 1 GB file of junk data in serial.
1. Use ensemble of above application to write 1 GB per rank in separate files.
1. Use MPI-IO to have each rank read/write chunks of a single file.
1. Enable staggering, so that I/O request happen in groups of `N` nodes.
1. Enable fan-in, fan-out so that only certain nodes do I/O and others communicate with them.
1. Enable fan-in and staggering

## Reporting

With each goal, collect performance metricsby varying:

* Filesizes
* Stripe counts
* Job sizes
