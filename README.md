elsi
==================

Tool to estimate Lustre s striping impact on scientific application checkpointing

## Usage

* Load the elsi module using `module load elsi`
* The tool has the ability to be run from inside an interactive job as well generating a script that you can submit
* Options for the tool are `-s/--stripeSize`, `-c/--stripeCount`, `-S/--fileSize`, `-f/--fileName`, `-p/--PBSscript`, `-h/--help` 
* `-s`: sets the stripe size using Lustre s `lfs setstripe` utility. Sizes are denoted by appending a lowercase k, m, or g to the desired size, e.g. `-s 3k` sets the stripe size to 3 KB. Remember, `lfs setstripe` can't change the striping information on a file that has already had its striping set, so change the name of the file you create for every run
* `-c`: sets the stripe count using Lustre s `lfs setstripe` utility. Count is an integer value that, on Titan as of August 21, 2014, can not be greater than 160. An example usage would be `-c 12` which would create 12 stripes that make up your file
* `-S`: sets the size of the file that you wish to create. Sizes are denoted by appending B, KB, MB, GB, or TB to the desired size, e.g. `-S 30TB` creates a 30 TB sized file. File sizes must be integers, e.g. 2.5TB will not work but 2560GB will. The file size can be a maximum of `25GB * NUMNODES`
* `-f`: sets the name of the file that you wish to create. Limited to 50 characters. Example usage would be `-f thisFile.dat`
* `-p`: option with no argument that enables the generation of a submit script called `elsi.titan.pbs`. This will cause several prompts to appear on the screen that will only ask for information that is needed in the submit script. The timing information will also be output into a file called `elsi-titan.oJOBID`
* `-h`: outputs the different options that are available
* The tool will run and output on the screen the average and standard deviation of how long it took to open the file, generate the data to write, write the data, and close the file. All timings are in seconds

## Examples 

Inside an Interactive Job:

* `module load elsi`
* `elsi -s 10m -c 50 -S 500GB -f integer.dat`
* Running these two commands would create a 500 Gigabyte file called integer.dat that had a stripe count of 50 and stripe size of 10 Megabytes

To Make a Submit Script:

* `module load elsi`
* `elsi -s 15m -c 2 -S 32MB -f smallFile.dat -p`
* You will be prompted here for information that is needed in the submit script, such as the email that you will be notified at when the job is finished, your project id, the number of nodes you are requesting and the walltime needed.
* `qsub elsi.titan.pbs`
* Running these three commands would create a file named smallFile.dat that was 32 MB and that had a stripe count of 2 and a stripe size of 15 MB. The output would then be found in a file called `elsi-titan.oJOBID`
 
## Run from Inside an Interactive Job:

```
wey@titan-batch6:/lustre/atlas/scratch/wey/stf007> module load elsi
wey@titan-batch6:/lustre/atlas/scratch/wey/stf007> elsi -s 10m -c 100 -S 625GB -f frosty.dat

----------------------------------------------------------
# of nodes: 25
----------------------------------------------------------
# of I/O ranks: 25
----------------------------------------------------------
Size of file created: 625GB
----------------------------------------------------------
Size of chunk written by each I/O Rank: 25.0GB
----------------------------------------------------------
    Open file
        Mean:   0.09376212
     Std Dev:   0.0335738356978
----------------------------------------------------------
Generate Data
        Mean:   2.56231128
     Std Dev:   0.0197928077453
----------------------------------------------------------
Data Transfer
        Mean:   126.60988604
     Std Dev:   3.56994139139
----------------------------------------------------------
   Close File
        Mean:   0.00044792
     Std Dev:   0.0007009258403
----------------------------------------------------------

wey@titan-batch6:/lustre/atlas/scratch/wey/stf007>
```
## Run from a Submit Script
```
wey@titan-ext7:~/elsi> module load elsi
wey@titan-ext7:~/elsi> elsi -s 5m -c 25 -S 100GB -f jake.dat -p
Enter desired number of nodes: 4
Enter your project ID: stf007
Enter your email address: wey@ornl.gov
Enter desired walltime(hh:mm:ss): 00:10:00
wey@titan-ext7:~/elsi> qsub elsi.titan.pbs
2048344
wey@titan-ext7:~/elsi> ls
elsi-titan.e2048344  elsi-titan.o2048344
wey@titan-ext7:~/elsi>
```
