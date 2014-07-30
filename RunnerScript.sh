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
      -f|--filename)
	 echo "hey"
         shift;
	 echo "hey"
	 shift;
	 echo "hey"
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

TOTAL_NUMBER_OF_RANKS=$(($NUMBER_OF_NODES * 16))
NUMBER_OF_IO_RANKS=$NUMBER_OF_NODES

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
export OMP_NUM_THREADS=1
aprun -n $TOTAL_NUMBER_OF_RANKS -N 16 -d \$OMP_NUM_THREADS ./striping-benchmark.titan.exe --size $SIZE_OF_FILE --numIORanks $NUMBER_OF_IO_RANKS --create $FILENAME > striping-titan.\$PBS_JOBID.txt
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








