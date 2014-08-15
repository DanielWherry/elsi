#!/usr/bin/env python
import numpy as np
from optparse import OptionParser
import json
from pylab import *
import re

def findFileSizeForEachRank(fileSize):
	#Assigns numerical value of fileSize[0] to a string e.g. '100MB' -> '100'
	completeFileSizeInString = re.sub('[MKGTB]','',fileSize[0])
	#Converts string file size to integer e.g '100' -> 100
	completeFileSizeInInt = int(completeFileSizeInString)
	#Assigns byte-magnitude of fileSize[0] to a string e.g. '100MB' -> 'MB'
	fileSizeEnding = re.sub('[0123456789]','',fileSize[0])
	#Calculates size of chunk that each file will write to file, where len(fileSize) is how many ranks wrote to file 
	rankFileSize = float(completeFileSizeInInt) / len(fileSize)
	#Returns the size that each rank will write to a file 
	return str(rankFileSize) + fileSizeEnding

#Options to take in from command line
parser = OptionParser()

parser.add_option("-f", "--file", action="store", type="string", dest="filename")

parser.add_option("-n", "--numNodes", action="store", type="int", dest="numNodes")

(options,args) = parser.parse_args()

#Open JSON document for processing
f = open(options.filename, 'r')

#Finds Job ID to be used in .html file
fileContainingJobID = re.findall('\d+',options.filename) 
JOB_ID = int(fileContainingJobID[0])

#Explicitly initializing arrays that will be filled from data in the file
rank = []
openT = []
generate = []
verify = []
read = []
write = []
close = []
fileSize = []

choice = "This hasn't been assigned"

#Process lines in JSON file to find data, ignore lines that aren't JSON
for line in f:
    try:
	dict = json.loads( line )
    	rank.append(dict['rank'])
    	openT.append(dict['Open Time'])
    	close.append(dict['Close Time'])
    	fileSize.append(dict['File Size'])
    	if 'Read Time' in dict:
    		verify.append(dict['Verify Time'])
    		read.append(dict['Read Time']) 
    		choice = "The file is being verified"
    	if 'Generation Time' in dict:
    		generate.append(dict['Generation Time'])
    		write.append(dict['Write Time'])
    		choice = "The file is being created"

    except ValueError:
	thisDoesntDoAnything = 10	 	 
    
f.close()

#Find the size of the data file that will be created by createFile.c
finalFileSize = findFileSizeForEachRank(fileSize)

#Create dictionary of statistical information to pass around between functions
timingStats = ({
	'openTimeMean':0, 'openTimeDev':0, 
	'closeTimeMean':0, 'closeTimeDev':0,
	'readTimeMean':0, 'readTimeDev':0,
	'verifyTimeMean':0, 'verifyTimeDev':0, 
	'writeTimeMean':0, 'writeTimeDev':0,
	'generateTimeMean':0, 'generateTimeDev':0
	})
  
timingStats['openTimeMean'] = np.mean(openT) 
timingStats['openTimeDev'] = np.std(openT)  
timingStats['closeTimeMean'] = np.mean(close)
timingStats['closeTimeDev'] = np.std(close)

rankNumber = len(rank)
numberOfNodes = options.numNodes


if choice == "The file is being verified":
    
    timingStats['readTimeMean'] = np.mean(read)
    timingStats['readTimeDev'] = np.std(read)
    timingStats['verifyTimeMean'] = np.mean(verify)
    timingStats['verifyTimeDev'] = np.std(verify)

if choice == "The file is being created":
    
    timingStats['generateTimeMean'] = np.mean(generate)
    timingStats['generateTimeDev'] = np.std(generate)
    timingStats['writeTimeMean'] = np.mean(write)
    timingStats['writeTimeDev'] = np.std(write)

    print "\n----------------------------------------------------------"
    print "# of nodes: " + str(numberOfNodes)
    print "----------------------------------------------------------"
    print "# of I/O ranks: " + str(numberOfNodes) 
    print "----------------------------------------------------------"
    print "Size of file created: " + str(fileSize[0]) 
    print "----------------------------------------------------------"
    print "Size of chunk written by each I/O Rank: " + str(finalFileSize)
    print "----------------------------------------------------------"
    print "    Open file"
    print "        Mean:" + "   " + str(timingStats['openTimeMean'])
    print "     Std Dev:" + "   " + str(timingStats['openTimeDev']) 
    print "----------------------------------------------------------"
    print "Generate Data"
    print "        Mean:" + "   " + str(timingStats['generateTimeMean'])
    print "     Std Dev:" + "   " + str(timingStats['generateTimeDev']) 
    print "----------------------------------------------------------"
    print "Data Transfer"
    print "        Mean:" + "   " + str(timingStats['writeTimeMean'])
    print "     Std Dev:" + "   " + str(timingStats['writeTimeDev']) 
    print "----------------------------------------------------------"
    print "   Close File"
    print "        Mean:" + "   " + str(timingStats['closeTimeMean'])
    print "     Std Dev:" + "   " + str(timingStats['closeTimeDev']) 
    print "----------------------------------------------------------\n"





