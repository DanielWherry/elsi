import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as pyplot
import numpy as np
from optparse import OptionParser
import json
from pylab import *


parser = OptionParser()

parser.add_option("-f", "--file", action="store", type="string", dest="filename")

(options,args) = parser.parse_args()

f = open(options.filename, 'r')

listOfDictionaries = []
rank = []
open = []
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
 open.append(x['Open Time'])
 close.append(x['Close Time'])
 if "Generation Time" not in x:
   verify.append(x['Verify Time'])
   read.append(x['Read Time']) 
   choice = 1
 elif "Read Time" not in x:
   generate.append(x['Generation Time'])
   write.append(x['Write Time'])
   choice = 2

figure(0)
pyplot.plot(rank,open,'blue',label="Open Time")
pyplot.title('Timing Report')
pyplot.xlabel('Rank ID')
pyplot.ylabel( 'Time(seconds)')
pyplot.legend()
pyplot.savefig('OpenTime.png')

figure(1)
pyplot.plot(rank,close, 'green', label = 'Close Time')
pyplot.title('Timing Report')
pyplot.xlabel('Rank ID')
pyplot.ylabel( 'Time(seconds)')
pyplot.legend()
pyplot.savefig('CloseTime.png')

if choice == 1:
  figure(2)
  pyplot.plot(rank,read, 'purple', label = 'Read Time')
  pyplot.title('Timing Report')
  pyplot.xlabel('Rank ID')
  pyplot.ylabel( 'Time(seconds)')
  pyplot.legend()
  pyplot.savefig('ReadTime.png')
 
  figure(3)
  pyplot.plot(rank,verify, 'brown', label = 'Verify Time')
  pyplot.title('Timing Report')
  pyplot.xlabel('Rank ID')
  pyplot.ylabel( 'Time(seconds)')
  pyplot.legend()
  pyplot.savefig('VerifyTime.png')

if choice == 2:
  figure(4)
  pyplot.plot(rank,generate,'red', label="Generate Time")
  pyplot.title('Timing Report')
  pyplot.xlabel('Rank ID')
  pyplot.ylabel( 'Time(seconds)')
  pyplot.legend()
  pyplot.savefig('GenerateTime.png')
 
  figure(5)
  pyplot.plot(rank,write,'orange', label="Write Time")
  pyplot.title('Timing Report')
  pyplot.xlabel('Rank ID')
  pyplot.ylabel( 'Time(seconds)')
  pyplot.legend()
  pyplot.savefig('WriteTime.png')
