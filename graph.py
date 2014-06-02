import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as pyplot
import numpy as np
from optparse import OptionParser
import json
from pylab import *

def html(message):
	this = open('stripingBenchmarkResults.html', 'w')
	finalMessage = """<html>
	<head>Report on stuff</head>
	<body>
	<p align="center"> <img src ="OpenTime.png" alt = "It's closing time...">
	This graph displays the time it took for each rank to open a file.
	<img src ="CloseTime.png" alt = "It's closing time...">
	This graph displays the time it took for each rank to close a file.
	%s</p>
	</body>
	</html>""" % message
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
   choice = 1
 elif "Generation Time" in x:
   generate.append(x['Generation Time'])
   write.append(x['Write Time'])
   choice = 2


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

if choice == 1:
  figure(2)
  pyplot.scatter(rank,read, label = 'Read Time')
  pyplot.title('Timing Report')
  pyplot.xlabel('Rank ID')
  pyplot.ylabel( 'Time(seconds)')
  pyplot.legend()
  pyplot.savefig('ReadTime.png')
 
  figure(3)
  pyplot.scatter(rank,verify, label = 'Verify Time')
  pyplot.title('Timing Report')
  pyplot.xlabel('Rank ID')
  pyplot.ylabel( 'Time(seconds)')
  pyplot.legend()
  pyplot.savefig('VerifyTime.png')
  message1 = """<img src ="VerifyTime.png" alt = "It's closing time...">
  This graph displays the time it took for each rank to verify a file.
  <img src ="ReadTime.png" alt = "It's closing time...">
  This graph displays the time it took for each rank to read a file."""
  html(message1)

if choice == 2:
  figure(4)
  pyplot.scatter(rank,generate, label="Generate Time")
  pyplot.title('Timing Report')
  pyplot.xlabel('Rank ID')
  pyplot.ylabel( 'Time(seconds)')
  pyplot.legend()
  pyplot.savefig('GenerateTime.png')
 
  figure(5)
  pyplot.scatter(rank,write, label="Write Time")
  pyplot.title('Timing Report')
  pyplot.xlabel('Rank ID')
  pyplot.ylabel( 'Time(seconds)')
  pyplot.legend()
  pyplot.savefig('WriteTime.png')
  message2 = """<img src ="GenerateTime.png" alt = "It's closing time...">
  This graph displays the time it took for each rank to generate an array to make the file.
  <img src ="WriteTime.png" alt = "It's closing time...">
  This graph displays the time it took for each rank to write a file."""
  html(message2)

