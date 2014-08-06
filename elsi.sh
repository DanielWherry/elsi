.\" Manpage for ELSI tool
.\" Contact dwherry2@my.apsu.edu for information about tool or assistance

.TH elsi 1 "05 August 2014" "1.0" "Tool Usage"

.SH NAME
.B elsi 
\- striping impact estimator

.SH SYNOPSIS
.B elsi 
[OPTIONS]

.SH DESCRIPTION
.B elsi 
is a tool that is designed to estimate Lustre striping impact on scientific checkpointing applications

.SH OPTIONS
.IP -s,--stripeSize
Controls the chunk size that would be given to Lustre\'s setstripe utility. Specified by appending 'k', 'm', or 'g' to the size desired e.g. "50m" for 50 Megabytes or "2" for 2 Kilobytes

.IP -c,--stripeCount
Controls the number of chunks that would be given to Lustre\'s setstripe utility

.IP -S,--fileSize
Controls the total file size that you want to created. Specified by appending 'B', 'KB', 'MB', 'GB', or 'TB' to the size desired e.g. "50GB" for a 50 Gigabyte file or "2TB" for a 2 Terabyte file

.IP -f,--fileName
Name of file that you wish to produce. ".dat" will be appended to the name that you choose e.g. "integer" becomes "integer.dat"

.SH SEE ALSO
lfs setstripe  

.SH BUGS
Can only create files that are a small fraction of the size of a nodes allocatable memory

Can not verify files yet, only create them

Checking stuff for repo
.SH AUTHOR
.IP "Daniel Wherry (dwherry2@my.apsu.edu) - Department of Computer Science, Austin Peay State University" 
.IP "Robert D. French (frenchrd@ornl.gov) - Oak Ridge Leadership Computing Facility"

