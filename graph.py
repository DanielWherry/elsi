import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import numpy as np
from optparse import OptionParser
import json
import os
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
	print ""	 	 
    
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
plt.plot(rank,open,'blue',label="Open Time")
plt.title('Timing Report')
plt.xlabel('Rank ID')
plt.ylabel( 'Time(seconds)')
plt.legend()
plt.savefig('OpenTime.png')

figure(1)
plt.plot(rank,close, 'green', label = 'Close Time')
plt.title('Timing Report')
plt.xlabel('Rank ID')
plt.ylabel( 'Time(seconds)')
plt.legend()
plt.savefig('CloseTime.png')

if choice == 1:
  figure(2)
  plt.plot(rank,read, 'purple', label = 'Read Time')
  plt.title('Timing Report')
  plt.xlabel('Rank ID')
  plt.ylabel( 'Time(seconds)')
  plt.legend()
  plt.savefig('ReadTime.png')
 
  figure(3)
  plt.plot(rank,verify, 'brown', label = 'Verify Time')
  plt.title('Timing Report')
  plt.xlabel('Rank ID')
  plt.ylabel( 'Time(seconds)')
  plt.legend()
  plt.savefig('VerifyTime.png')

if choice == 2:
  figure(4)
  plt.plot(rank,generate,'red', label="Generate Time")
  plt.title('Timing Report')
  plt.xlabel('Rank ID')
  plt.ylabel( 'Time(seconds)')
  plt.legend()
  plt.savefig('GenerateTime.png')
 
  figure(5)
  plt.plot(rank,write,'orange', label="Write Time")
  plt.title('Timing Report')
  plt.xlabel('Rank ID')
  plt.ylabel( 'Time(seconds)')
  plt.legend()
  plt.savefig('WriteTime.png')
