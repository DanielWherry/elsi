#!/bin/bash


ARGS=$(getopt -o s:c:n:S:f -l "stripeSize:,stripeCount:,numNodes:,fileSize:,filename" -n "RunnerScript.sh" -- "$@");

if [ $? -ne 0 ];
then
   exit 1
fi

eval set -- "$ARGS";

while true; do
   case "$1" in
      -s|--stripeSize)
         shift;
         if [ -n "$1" ]; then
	    STRIPESIZE=$1;
	    shift;
 	 fi
	 ;;
      -c|--stripeCount)
         shift;
         if [ -n "$1" ]; then
	    STRIPECOUNT=$1;
	    shift;
 	 fi
	 ;;
      -S|--fileSize)
         shift;
         if [ -n "$1" ]; then
	    SIZE_OF_FILE=$1;
	    shift;
 	 fi
	 ;;
      -f|--filename)
         shift;
	 shift;
         if [ -n "$1" ]; then
	    FILENAME=$1;
 	 fi
	 break;
	 ;;
	--)
	break;
	;;
   esac
done

TOTAL_NUMBER_OF_RANKS=$(($PBS_NUM_NODES * 16))
NUMBER_OF_IO_RANKS=$PBS_NUM_NODES

rm integer.dat
lfs setstripe --size $STRIPESIZE -c $STRIPECOUNT integer.dat
export OMP_NUM_THREADS=1
aprun -n $TOTAL_NUMBER_OF_RANKS -N 16 -d $OMP_NUM_THREADS ./striping-benchmark.titan.exe --size $SIZE_OF_FILE --numIORanks $NUMBER_OF_IO_RANKS --create $FILENAME > striping-titan.$PBS_JOBID.txt
source $MODULESHOME/init/bash
module load python_matplotlib
python graph.py --file striping-titan.$PBS_JOBID.txt --numNodes $PBS_NUM_NODES

if [ ! -d "./Official-Reports/" ]; then
	mkdir ./Official-Reports/
fi

mv StripingBenchmarkResults.html ./Official-Reports/StripingBenchmarkResults.$PBS_JOBID.html
mv OpenTime.* ./Official-Reports/
mv CloseTime.* ./Official-Reports/
[ -e GenerateTime.* ] && mv GenerateTime.* ./Official-Reports/
[ -e WriteTime.* ] && mv WriteTime.* ./Official-Reports/
[ -e ReadTime.* ] && mv GenerateTime.* ./Official-Reports/
[ -e VerifyTime.* ] && mv GenerateTime.* ./Official-Reports/








