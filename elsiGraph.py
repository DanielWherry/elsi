#!/usr/bin/env python
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as pyplot
import numpy as np
from optparse import OptionParser
import json
from pylab import *
import re


def html(message, timingStats, numRanks, numberOfNodes, rankFileSize, JOB_ID, overallFileSize, fileNames):

	
	ranksPerNode = numRanks / numberOfNodes

	HTML = open('elsiResults.html', 'w')
	finalMessage = """
	<html>
	<head>
	<title>
	REPORT ON TIMING
	</title>
	<h1>
	This run was completed using %(numNodes)d I/O Ranks to make %(fileChunks)s sized chunks of a %(fileSize)s sized file. Here was the script submitted to run this test: 
	</h1>
	</head>
	<body>
	<table>
	<tr>
	<td>
	<p align="center">
	<img src =%(openFile)s alt = "It's closing time..."align=middle>
	</td>
	<td>
	<font size="5">This graph displays the time it took for each rank to open a file. The average time for opening a file was %(openTime)f seconds, with a standard deviation of %(oDev)f seconds.
	</font>
	</p>
	</td>
	</tr>
	<tr>
	<td>
	<p align="center">
	<img src =%(closeFile)s alt = "It's closing time..."align=middle>
	</td>
	<td>
	<font size="5">This graph displays the time it took for each rank to close a file.The average time for closing a file was %(closeTime)f seconds, with a standard deviation of %(cDev)f seconds.
	</font>
	</p>
	</td>
	</tr>
	%(otherMessage)s
	</table>
	</body>
	</html>""" % {"openFile":fileNames['openFileName'], "closeFile":fileNames['closeFileName'], "otherMessage":message, "openTime":timingStats['openTimeMean'], "closeTime":timingStats['closeTimeMean'], "oDev":timingStats['openTimeDev'], "cDev":timingStats['closeTimeDev'], "numNodes":numberOfNodes, "fileSize":overallFileSize, "fileChunks": rankFileSize}
	HTML.write(finalMessage)
	HTML.close()

def plotResults(xAxis, yAxis, title, JOB_ID, typeOfTiming, figureID): 
	figure(figureID)
	pyplot.scatter(xAxis,yAxis, label = title)
	pyplot.title('Timing Report')
	pyplot.xlabel('Rank ID')
	pyplot.ylabel( 'Time(seconds)')
	pyplot.legend()
	pyplot.axis([min(xAxis), max(xAxis), None, None])
	fileName = typeOfTiming + str(JOB_ID) + '.png'
	pyplot.savefig(fileName)

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

def verifyHTML(timingStats, fileNames):
	return  """
	<tr>
	<td>
	<img src =%(verFile)s alt = "It's closing time..."align=left />
	</td>
	<td>
    	<font size="5">This graph displays the time it took for each rank to verify a file.The average time it took for a rank to verify a file was %(verifyTime)f seconds, with a standard deviation of %(vDev)f seconds.
    	</font>
    	</td>
    	</tr>
    	<tr>
    	<td>
    	<img src =%(readFile)s alt = "It's closing time..."align=left>
    	</td>
    	<td>
    	<font size="5">This graph displays the time it took for each rank to read a file.The average time it took for a rank to read a file was %(readTime)f seconds, with a standard deviation of %(rDev)f seconds.
    	</font>
    	</td>
    	</tr>""" % {"verFile":fileNames['verifyFileName'], "readFile":fileNames['readFileName'], "verifyTime": timingStats['verifyTimeMean'], "readTime":timingStats['readTimeMean'], "rDev":timingStats['readTimeDev'], "vDev":timingStats['verifyTimeDev']}

def createHTML(timingStats, fileNames):
	return """
	<tr>
	<td>
	<img src =%(genFile)s alt = "It's closing time..."align=left />
	</td>
	<td>
	<font size="5">This graph displays the time it took for each rank to generate an array to make the file.The average time it took for each rank  to generate an array for a file was %(generateTime)f seconds, with a standard deviation of %(genDev)f seconds.
	</font>
	</td>
	</tr>
	<tr>
	<td>
	<p align = "center">
	<img src =%(writeFile)s alt = "It's closing time..."align=left />
	</td>
	<td>
	<font size="5">This graph displays the time it took for each rank to write a file. The average time it took for each rank to  write to a file was %(writeTime)f seconds, with a standard deviation of %(wDev)f seconds. 
	</font>
	</td>
	</tr>""" %{"genFile":fileNames['generateFileName'], "writeFile":fileNames['writeFileName'], "generateTime":timingStats['generateTimeMean'], "writeTime":timingStats['writeTimeMean'], "wDev":timingStats['writeTimeDev'], "genDev":timingStats['generateTimeDev']}


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

fileNames = ({
	'openFileName':'OpenTime.%d.png' % JOB_ID,
	'closeFileName':'CloseTime.%d.png' % JOB_ID,
	'readFileName':'ReadTime.%d.png' % JOB_ID,
	'verifyFileName':'VerifyTime.%d.png' % JOB_ID,
	'generateFileName':'GenerateTime.%d.png' % JOB_ID,
	'writeFileName':'WriteTime.%d.png' % JOB_ID
	})

#Begin plotting data as .png files, then include those in .html file with statistical data
plotResults(rank, openT, 'Open Time', JOB_ID, 'OpenTime.', 0)
plotResults(rank, close, 'Close Time', JOB_ID, 'CloseTime.', 1)

if choice == "The file is being verified":
    plotResults(rank, read, 'Read Time', JOB_ID, 'ReadTime.', 2)
    plotResults(rank, verify, 'Verify Time', JOB_ID, 'VerifyTime.', 3)
    
    timingStats['readTimeMean'] = np.mean(read)
    timingStats['readTimeDev'] = np.std(read)
    timingStats['verifyTimeMean'] = np.mean(verify)
    timingStats['verifyTimeDev'] = np.std(verify)
    verifyFileName = "VerifyTime.%(JOB_ID)d.png" % {"JOB_ID":JOB_ID} 
    readFileName = "ReadTime.%(JOB_ID)d.png" % {"JOB_ID":JOB_ID} 
  
    verifyHTMLScript = verifyHTML(timingStats, fileNames)

    html(verifyHTMLScript, timingStats, rankNumber, numberOfNodes, finalFileSize, JOB_ID, fileSize[0], fileNames)

if choice == "The file is being created":
    plotResults(rank, generate, 'Generate Time', JOB_ID, 'GenerateTime.', 4)
    plotResults(rank, write, 'Write Time', JOB_ID, 'WriteTime.', 5)
    
    timingStats['generateTimeMean'] = np.mean(generate)
    timingStats['generateTimeDev'] = np.std(generate)
    timingStats['writeTimeMean'] = np.mean(write)
    timingStats['writeTimeDev'] = np.std(write)
    generateFileName = "GenerateTime.%(JOB_ID)d.png" % {"JOB_ID":JOB_ID} 
    writeFileName = "WriteTime.%(JOB_ID)d.png" % {"JOB_ID":JOB_ID} 

    createHTMLScript = createHTML(timingStats, fileNames)

    html(createHTMLScript, timingStats, rankNumber, numberOfNodes, finalFileSize, JOB_ID, fileSize[0], fileNames)





