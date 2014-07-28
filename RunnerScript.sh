#!/bin/bash

ARGS=$(getopt -o s:c:d:r:n:R:S:i:f -l "stripeSize:,stripeCount:,numOMPThreads:,ranks:,numNodes:,ranksPerNode:,fileSize:,numIORanks:,filename" -n "RunnerScript.sh" -- "$@");

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
      -d|--numOMPThreads)
         shift;
         if [ -n "$1" ]; then
	    NUMBER_OF_OMP_THREADS=$1;
	    shift;
 	 fi
	 ;;
      -r|--ranks)
         shift;
         if [ -n "$1" ]; then
	    TOTAL_NUMBER_OF_RANKS=$1;
	    shift;
 	 fi
	 ;;
      -R|--ranksPerNode)
         shift;
         if [ -n "$1" ]; then
	    RANKS_PER_NODE=$1;
	    shift;
 	 fi
	 ;;
      -n|--numNodes)
         shift;
         if [ -n "$1" ]; then
	    NUMBER_OF_NODES=$1;
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
      -i|--numIORanks)
         shift;
         if [ -n "$1" ]; then
	    NUMBER_OF_IO_RANKS=$1;
	    shift;
 	 fi
	 ;;
      -f|--filename)
         shift;
         if [ -n "$1" ]; then
	    FILENAME=$1;
	 #   shift;
 	 fi
	 #break;
	 ;;
       --)
	 shift;
	 break;
	 ;;
   esac
done
#STRIPESIZE=3m
#STRIPECOUNT=16
#NUMBER_OF_OMP_THREADS=1
#TOTAL_NUMBER_OF_RANKS=160
#RANKS_PER_NODE=16
#NUMBER_OF_NODES=10
#SIZE_OF_FILE=10GB
#NUMBER_OF_IO_RANKS=10
#FILENAME=integer

echo "
#!/bin/bash
#Begin PBS directives
#PBS -A stf007
#PBS -N striping-titan
#PBS -M wey@ornl.gov
#PBS -m e
#PBS -l walltime=0:15:00,nodes=$NUMBER_OF_NODES
#PBS -l gres=atlas1%atlas2
#End PBS directives and begin shell commands

cp \$PBS_O_WORKDIR/submit.titan.pbs \$MEMBERWORK/stf007/submit.titan.\$\PBS_JOBID.pbs
cd \$MEMBERWORK/stf007
rm integer.dat
lfs setstripe --size $STRIPESIZE -c $STRIPECOUNT integer.dat
export OMP_NUM_THREADS=$NUMBER_OF_OMP_THREADS
aprun -n $TOTAL_NUMBER_OF_RANKS -N $RANKS_PER_NODE -d $NUMBER_OF_OMP_THREADS ./striping-benchmark.titan.exe --size $SIZE_OF_FILE --numIORanks $NUMBER_OF_IO_RANKS --create $FILENAME > striping-titan.\$PBS_JOBID.txt
source \$MODULESHOME/init/bash
module load python_matplotlib
python graph.py --file striping-titan.\$PBS_JOBID.txt --numNodes $NUMBER_OF_NODES
mv StripingBenchmarkResults.html \$PBS_O_WORKDIR/Official-Reports/StripingBenchmarkResults.\$PBS_JOBID.html
mv OpenTime.* \$PBS_O_WORKDIR/Official-Reports/
mv CloseTime.* \$PBS_O_WORKDIR/Official-Reports/
[ -e GenerateTime.* ] && mv GenerateTime.* \$PBS_O_WORKDIR/Official-Reports/
[ -e WriteTime.* ] && mv WriteTime.* \$PBS_O_WORKDIR/Official-Reports/
[ -e ReadTime.* ] && mv GenerateTime.* \$PBS_O_WORKDIR/Official-Reports/
[ -e VerifyTime.* ] && mv GenerateTime.* \$PBS_O_WORKDIR/Official-Reports/
" > submit.testit.titan.pbs








