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

parser = OptionParser()

parser.add_option("-f", "--file", action="store", type="string", dest="filename")

parser.add_option("-n", "--numNodes", action="store", type="int", dest="numNodes")

(options,args) = parser.parse_args()

f = open(options.filename, 'r')

fileContainingJobID = re.findall('\d+',options.filename) 
JOB_ID = int(fileContainingJobID[0])

rank = []
openT = []
generate = []
verify = []
read = []
write = []
close = []
fileSize = []

choice = "This hasn't been assigned"

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


completeFileSizeInString = re.sub('[MKGTB]','',fileSize[0])
completeFileSizeInInt = int(completeFileSizeInString)
fileSizeEnding = re.sub('[0123456789]','',fileSize[0])
rankFileSize = float(completeFileSizeInInt) / len(fileSize)
finalFileSize = str(rankFileSize) + fileSizeEnding

openTimeMean = np.mean(openT) 
openTimeDev = np.std(openT)  
closeTimeMean = np.mean(close)
closeTimeDev = np.std(close)
rankNumber = len(rank)
numberOfNodes = options.numNodes

figure(0)
pyplot.scatter(rank,openT, label = 'Open Time')
pyplot.title('Timing Report')
pyplot.xlabel('Rank ID')
pyplot.ylabel( 'Time(seconds)')
pyplot.legend()
pyplot.axis([rank[0], rank[-1], None, None])
openFileName = 'OpenTime.' + str(JOB_ID) + '.png'
pyplot.savefig(openFileName)

figure(1)
pyplot.scatter(rank,close, label = 'Close Time')
pyplot.title('Timing Report')
pyplot.xlabel('Rank ID')
pyplot.ylabel( 'Time(seconds)')
pyplot.legend()
pyplot.axis([rank[0], rank[-1], None, None])
closeFileName = 'CloseTime.' + str(JOB_ID) + '.png'
pyplot.savefig(closeFileName)

if choice == "The file is being verified":
    readTimeMean = np.mean(read)
    readTimeDev = np.std(read)
    figure(2)
    pyplot.scatter(rank,read, label = 'Read Time')
    pyplot.title('Timing Report')
    pyplot.xlabel('Rank ID')
    pyplot.ylabel( 'Time(seconds)')
    pyplot.legend()
    pyplot.axis([rank[0], rank[-1], None, None])
    readFileName = 'ReadTime.' + str(JOB_ID) + '.png'
    pyplot.savefig(readFileName)
  
    verifyTimeMean = np.mean(verify)
    verifyTimeDev = np.std(verify)
    figure(3)
    pyplot.scatter(rank,verify, label = 'Verify Time')
    pyplot.title('Timing Report')
    pyplot.xlabel('Rank ID')
    pyplot.ylabel( 'Time(seconds)')
    pyplot.legend()
    pyplot.axis([rank[0], rank[-1], None, None])
    verifyFileName = 'VerifyTime.' + str(JOB_ID) + '.png'
    pyplot.savefig(verifyFileName)

    message1 = """<tr><td><img src =%(verFile)s alt = "It's closing time..."align=middle></td>
<td><font size="5">This graph displays the time it took for each rank to verify a file.The average time it took for a rank to verify a file was %(verifyTime)f seconds, with a standard deviation of %(vDev)f seconds.</font></td></tr>
  <tr><td><img src =%(readFile)s alt = "It's closing time..."align=middle></td>
<td><font size="5">This graph displays the time it took for each rank to read a file.The average time it took for a rank to read a file was %(readTime)f seconds, with a standard deviation of %(rDev)f seconds.</font></td></tr>""" % {"verifyTime": verifyTimeMean, "readTime":readTimeMean, "rDev":readTimeDev, "vDev":verifyTimeDev,"verFile": verifyFileName, "readFile":readFileName}

    html(message1, openTimeMean, closeTimeMean, openTimeDev, closeTimeDev, rankNumber, numberOfNodes, finalFileSize, closeFileName, openFileName, JOB_ID)

if choice == "The file is being created":
    generateTimeMean = np.mean(generate)
    generateTimeDev = np.std(generate)
    figure(4)
    pyplot.scatter(rank,generate, label="Generate Time")
    pyplot.title('Timing Report')
    pyplot.xlabel('Rank ID')
    pyplot.ylabel( 'Time(seconds)')
    pyplot.legend()
    pyplot.axis([rank[0], rank[-1], None, None])
    generateFileName = 'GenerateTime.' + str(JOB_ID) + '.png'
    pyplot.savefig(generateFileName)

    writeTimeMean = np.mean(write)
    writeTimeDev = np.std(write)
    figure(5)
    pyplot.scatter(rank,write, label="Write Time")
    pyplot.title('Timing Report')
    pyplot.xlabel('Rank ID')
    pyplot.ylabel( 'Time(seconds)')
    pyplot.legend()
    pyplot.axis([rank[0], rank[-1], None, None])
    writeFileName = 'WriteTime.' + str(JOB_ID) + '.png'
    pyplot.savefig(writeFileName)

    message2 = """<tr><td><img src =%(genFile)s alt = "It's closing time..."align="left" /></td>
	<td><font size="5">This graph displays the time it took for  each rank to generate an array to make the file.The average time it took for each rank  to generate an array for a file was %(generateTime)f seconds, with a standard deviation of %(genDev)f seconds.</font></td></tr>
<tr><td><p align = "center"><img src =%(writeFile)s alt = "It's closing time..."align=left /></td>
<td><font size="5">This graph displays the time it took for each rank to write a file. The average time it took for each rank to  write to a file was %(writeTime)f seconds, with a standard deviation of %(wDev)f seconds. </font></td></tr>""" %{"generateTime":generateTimeMean, "writeTime":writeTimeMean, "wDev":writeTimeDev, "genDev":generateTimeDev, "genFile":generateFileName, "writeFile":writeFileName}

    html(message2, openTimeMean, closeTimeMean, openTimeDev, closeTimeDev, rankNumber, numberOfNodes, finalFileSize, closeFileName, openFileName, JOB_ID)

print choice
