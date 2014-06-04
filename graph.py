import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as pyplot
import numpy as np
from optparse import OptionParser
import json
from pylab import *


def html(message, openTimeMean, closeTimeMean, openTimeDev, closeTimeDev):
	this = open('stripingBenchmarkResults.html', 'w')
	finalMessage = """<html>
	<head><title>This is a report</title></head>
	<body>
	<table><tr>
	<td><p align="center"> <img src ="OpenTime.png" alt = "It's closing time..."align=middle></td>
	<td><font size="5">This graph displays the time it took for each rank to open a file. The average time for opening a file was %(openTime)f seconds, with a standard deviation of %(oDev)f seconds.</font></p></td></tr>
	<tr><td><p align="center"><img src ="CloseTime.png" alt = "It's closing time..."align=middle></td>
	<td><font size="5">This graph displays the time it took for each rank to close a file.The average time for closing a file was %(closeTime)f seconds, with a standard deviation of %(cDev)f seconds.</font></p></td></tr>
	%(otherMessage)s
	</table>
	</body>
	</html>""" %{"otherMessage":message, "openTime":openTimeMean, "closeTime":closeTimeMean, "oDev":openTimeDev, "cDev":closeTimeDev}
	this.write(finalMessage)
	this.close()

parser = OptionParser()

parser.add_option("-f", "--file", action="store", type="string", dest="filename")

(options,args) = parser.parse_args()

f = open(options.filename, 'r')

listOfDictionaries = []
rank = []
openT = []
generate = []
verify = []
read = []
write = []
close = []

try:
   for line in f:
         dict = json.loads( line )	
         listOfDictionaries.append(dict)
except ValueError:
	thisDoesntDoAnything = 10	 	 
    
f.close()

sortedDictionaryList = sorted(listOfDictionaries, key=lambda k: k['rank'])


for x in sortedDictionaryList:

 rank.append(x['rank'])
 openT.append(x['Open Time'])
 close.append(x['Close Time'])
 if "Read Time" in x:
   verify.append(x['Verify Time'])
   read.append(x['Read Time']) 
   choice = "The file is being verified"
 elif "Generation Time" in x:
   generate.append(x['Generation Time'])
   write.append(x['Write Time'])
   choice = "The file is being created"

openTimeMean = np.mean(openT) 
openTimeDev = np.std(openT)  
closeTimeMean = np.mean(close)
closeTimeDev = np.std(close)


figure(0)
pyplot.scatter(rank,openT, label = 'Open Time')
pyplot.title('Timing Report')
pyplot.xlabel('Rank ID')
pyplot.ylabel( 'Time(seconds)')
pyplot.legend()
pyplot.savefig('OpenTime.png')

figure(1)
pyplot.scatter(rank,close, label = 'Close Time')
pyplot.title('Timing Report')
pyplot.xlabel('Rank ID')
pyplot.ylabel( 'Time(seconds)')
pyplot.legend()
pyplot.savefig('CloseTime.png')

if choice == "The file is being verified":
  readTimeMean = np.mean(read)
  readTimeDev = np.std(read)
  figure(2)
  pyplot.scatter(rank,read, label = 'Read Time')
  pyplot.title('Timing Report')
  pyplot.xlabel('Rank ID')
  pyplot.ylabel( 'Time(seconds)')
  pyplot.legend()
  pyplot.savefig('ReadTime.png')
  
  verifyTimeMean = np.mean(verify)
  verifyTimeDev = np.std(verify)
  figure(3)
  pyplot.scatter(rank,verify, label = 'Verify Time')
  pyplot.title('Timing Report')
  pyplot.xlabel('Rank ID')
  pyplot.ylabel( 'Time(seconds)')
  pyplot.legend()
  pyplot.savefig('VerifyTime.png')
  message1 = """<tr><td><img src ="VerifyTime.png" alt = "It's closing time..."align=middle></td>
<td><font size="5">This graph displays the time it took for each rank to verify a file.The average time it took for a rank to verify a file was %(verifyTime)f seconds, with a standard deviation of %(vDev)f seconds.</font></td></tr>
  <tr><td><img src ="ReadTime.png" alt = "It's closing time..."align=middle></td>
<td><font size="5">This graph displays the time it took for each rank to read a file.The average time it took for a rank to read a file was %(readTime)f seconds, with a standard deviation of %(rDev)f seconds.</font></td></tr>""" % {"verifyTime": verifyTimeMean, "readTime":readTimeMean, "rDev":readTimeDev, "vDev":verifyTimeDev}
  html(message1, openTimeMean, closeTimeMean, openTimeDev, closeTimeDev)

if choice == "The file is being created":
  generateTimeMean = np.mean(generate)
  generateTimeDev = np.std(generate)
  figure(4)
  pyplot.scatter(rank,generate, label="Generate Time")
  pyplot.title('Timing Report')
  pyplot.xlabel('Rank ID')
  pyplot.ylabel( 'Time(seconds)')
  pyplot.legend()
  pyplot.savefig('GenerateTime.png')

  writeTimeMean = np.mean(write)
  writeTimeDev = np.std(write)
  figure(5)
  pyplot.scatter(rank,write, label="Write Time")
  pyplot.title('Timing Report')
  pyplot.xlabel('Rank ID')
  pyplot.ylabel( 'Time(seconds)')
  pyplot.legend()
  pyplot.savefig('WriteTime.png')
  message2 = """<tr><td><img src ="GenerateTime.png" alt = "It's closing time..."align="left" /></td>
	<td><font size="5">This graph displays the time it took for  each rank to generate an array to make the file.The average time it took for each rank  to generate an array for a file was %(generateTime)f seconds, with a standard deviation of %(genDev)f seconds.</font></td></tr>
<tr><td><p align = "center"><img src ="WriteTime.png" alt = "It's closing time..."align=left /></td>
<td><font size="5">This graph displays the time it took for each rank to write a file. The average time it took for each rank to  write to a file was %(writeTime)f seconds, with a standard deviation of %(wDev)f seconds. </font></td></tr>""" %{"generateTime":generateTimeMean, "writeTime":writeTimeMean, "wDev":writeTimeDev, "genDev":generateTimeDev}
  html(message2, openTimeMean, closeTimeMean, openTimeDev, closeTimeDev)

