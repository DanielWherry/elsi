elsi
==================

Tool to estimate Lustre s striping impact on scientific application checkpointing

## Reporting

With each goal, collect performance metrics by varying:

* Filesizes
* Stripe counts
* Stripe sizes
* Job sizes

## Usage

On Titan:

* Load the elsi module using "module load elsi"
* The tool has the ability to be run from inside an interactive job as well generating a script that you can submit
* Options for the tool are -s/--stripeSize, -c/--stripeCount, -S/--fileSize, -f/--fileName, -p/--PBSscript, -h/--help 
* -s: sets the stripe size using Lustre s "lfs setstripe" utility. Sizes are denoted by appending a lowercase k, m, or g to the desired size, i.e. "-s 3k" sets the stripe size to 3 Kilobytes. Remember, "lfs setstripe" can't change the striping information on a file that has already had its striping set, so change the name of the file you create for every run!
* -c: sets the stripe count using Lustre s "lfs setstripe" utility. Count is an integer value that, on Titan as of August 21, 2014, can not be greater than 160. An example usage would be "-c 12" which would create 12 stripes that make up your file
* -S: sets the size of the file that you wish to create. Sizes are denoted by appending B, KB, MB, GB, or TB to the desired size, i.e. "-S 30TB" creates a 30 Terabyte sized file. File sizes must be integers, e.g. "2.5TB" will not work but "2560GB" will
* -f: sets the name of the file that you wish to create. Limited to 50 characters. Example usage would be "-f thisFile.dat"
* -p: option with no argument that enables the generation of a submit script called "elsi.titan.pbs". This will cause several prompts to appear on the screen that will only ask for information that is needed in the submit script. The timing information will also be output into a file called "elsi-titan.oJOB_ID"
* -h: outputs the different options that are available
* The tool will run and output on the screen the average and standard deviation of how long it took to open the file, generate the data to write, write the data, and close the file. All timings are in seconds

## Examples 

Inside an interactive job:

* "module load elsi"
* "elsi -s 10m -c 50 -S 500GB -f integer.dat"
* Running these two commands would create a 500 Gigabyte file called integer.dat that had a stripe count of 50 and stripe size of 10 Megabytes

To Make a Submit Script:

* "module load elsi"
* "elsi -s 15m -c 2 -S 32MB -f smallFile.dat -p"
* "several prompts go here"
* "qsub elsi.titan.pbs"
* Running these three commands would create a file named smallFile.dat that was 32 Megabytes and that had a stripe count of 2 and a stripe size of 15 Megabytes. The output would then be found in a file called "elsi-titan.oXXXXXXX", where the X's represent the job id associated with your qsub





