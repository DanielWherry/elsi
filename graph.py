import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as pyplot
import numpy as np
from optparse import OptionParser
import json
from pylab import *
import re


def html(message, openTimeMean, closeTimeMean, openTimeDev, closeTimeDev, numRanks, numberOfNodes, finalFileSize, closeFileName, openFileName, JOB_ID):

	nameOfSubmitScript = "submit.titan.%(name)d.pbs"%{"name":JOB_ID}	

	PBS_SCRIPT = open(nameOfSubmitScript,'r')

	contentsOfScript = PBS_SCRIPT.read()	
		
	PBS_SCRIPT.close()
	
	ranksPerNode = numRanks / numberOfNodes

	HTML = open('StripingBenchmarkResults.html', 'w')
	finalMessage = """<html>
	<head><title>REPORT ON TIMING</title><h1>This run was completed using %(ranks)d ranks at %(rpNode)d ranks per node to make %(fileSize)s sized files. Here was the script submitted to run this test: </h1><h2><pre><code>%(script)s</code></pre></h2></head>
	<body>
	<table><tr>
	<td><p align="center"> <img src =%(openFile)s alt = "It's closing time..."align=middle></td>
	<td><font size="5">This graph displays the time it took for each rank to open a file. The average time for opening a file was %(openTime)f seconds, with a standard deviation of %(oDev)f seconds.</font></p></td></tr>
	<tr><td><p align="center"><img src =%(closeFile)s alt = "It's closing time..."align=middle></td>
	<td><font size="5">This graph displays the time it took for each rank to close a file.The average time for closing a file was %(closeTime)f seconds, with a standard deviation of %(cDev)f seconds.</font></p></td></tr>
	%(otherMessage)s
	</table>
	</body>
	</html>""" %{"otherMessage":message, "openTime":openTimeMean, "closeTime":closeTimeMean, "oDev":openTimeDev, "cDev":closeTimeDev, "ranks":numRanks, "rpNode":ranksPerNode, "fileSize": finalFileSize,"script":contentsOfScript, "openFile":openFileName, "closeFile":closeFileName}
	HTML.write(finalMessage)
	HTML.close()

def plotResults(xAxis, yAxis, title, JOB_ID, typeOfTiming, figureID) 
	figure(figureID)
	pyplot.scatter(xAxis,yAxis, label = title)
	pyplot.title('Timing Report')
	pyplot.xlabel('Rank ID')
	pyplot.ylabel( 'Time(seconds)')
	pyplot.legend()
	pyplot.axis([min(xAxis), max(xAxis), None, None])
	fileName = typeOfTiming + str(JOB_ID) + '.png'
	pyplot.savefig(fileName)

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
completeFileSizeInString = re.sub('[MKGTB]','',fileSize[0])
completeFileSizeInInt = int(completeFileSizeInString)
fileSizeEnding = re.sub('[0123456789]','',fileSize[0])
rankFileSize = float(completeFileSizeInInt) / len(fileSize)
finalFileSize = str(rankFileSize) + fileSizeEnding

#Generate statistical data to be presented in the .html file
openTimeMean = np.mean(openT) 
openTimeDev = np.std(openT)  
closeTimeMean = np.mean(close)
closeTimeDev = np.std(close)
rankNumber = len(rank)
numberOfNodes = options.numNodes

#Begin plotting data as .png files, then include those in .html file with statistical data
plotResults(rank, openT, 'Open Time', JOB_ID, 'OpenTime.', 0)
plotResults(rank, close, 'Close Time', JOB_ID, 'CloseTime.', 1)

if choice == "The file is being verified":
    plotResults(rank, read, 'Read Time', JOB_ID, 'ReadTime.', 2)
    plotResults(rank, verify, 'Verify Time', JOB_ID, 'VerifyTime.', 3)
    
    readTimeMean = np.mean(read)
    readTimeDev = np.std(read)
    verifyTimeMean = np.mean(verify)
    verifyTimeDev = np.std(verify)
  
    message1 = """<tr><td><img src =%(verFile)s alt = "It's closing time..."align=middle></td>
<td><font size="5">This graph displays the time it took for each rank to verify a file.The average time it took for a rank to verify a file was %(verifyTime)f seconds, with a standard deviation of %(vDev)f seconds.</font></td></tr>
  <tr><td><img src =%(readFile)s alt = "It's closing time..."align=middle></td>
<td><font size="5">This graph displays the time it took for each rank to read a file.The average time it took for a rank to read a file was %(readTime)f seconds, with a standard deviation of %(rDev)f seconds.</font></td></tr>""" % {"verifyTime": verifyTimeMean, "readTime":readTimeMean, "rDev":readTimeDev, "vDev":verifyTimeDev,"verFile": verifyFileName, "readFile":readFileName}

    html(message1, openTimeMean, closeTimeMean, openTimeDev, closeTimeDev, rankNumber, numberOfNodes, finalFileSize, closeFileName, openFileName, JOB_ID)

if choice == "The file is being created":
    plotResults(rank, generate, 'Generate Time', JOB_ID, 'GenerateTime.', 4)
    plotResults(rank, write, 'Write Time', JOB_ID, 'WriteTime.', 5)
    
    generateTimeMean = np.mean(generate)
    generateTimeDev = np.std(generate)
    writeTimeMean = np.mean(write)
    writeTimeDev = np.std(write)

    message2 = """<tr><td><img src =%(genFile)s alt = "It's closing time..."align="left" /></td>
	<td><font size="5">This graph displays the time it took for  each rank to generate an array to make the file.The average time it took for each rank  to generate an array for a file was %(generateTime)f seconds, with a standard deviation of %(genDev)f seconds.</font></td></tr>
<tr><td><p align = "center"><img src =%(writeFile)s alt = "It's closing time..."align=left /></td>
<td><font size="5">This graph displays the time it took for each rank to write a file. The average time it took for each rank to  write to a file was %(writeTime)f seconds, with a standard deviation of %(wDev)f seconds. </font></td></tr>""" %{"generateTime":generateTimeMean, "writeTime":writeTimeMean, "wDev":writeTimeDev, "genDev":generateTimeDev, "genFile":generateFileName, "writeFile":writeFileName}

    html(message2, openTimeMean, closeTimeMean, openTimeDev, closeTimeDev, rankNumber, numberOfNodes, finalFileSize, closeFileName, openFileName, JOB_ID)
