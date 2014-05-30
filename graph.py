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
    generate.append(x['Generation Time'])
    write.append(x['Write Time'])
    close.append(x['Close Time'])

figure(0)
plt.plot(rank,open,'blue',label="Open Time")
plt.title('Timing Report')
plt.xlabel('Rank ID')
plt.ylabel( 'Time(seconds)')
plt.legend()
plt.savefig('OpenTime.png')

figure(1)
plt.plot(rank,generate,'red', label="Generate Time")
plt.title('Timing Report')
plt.xlabel('Rank ID')
plt.ylabel( 'Time(seconds)')
plt.legend()
plt.savefig('GenerateTime.png')

figure(2)
plt.plot(rank,write,'orange', label="Write Time")
plt.title('Timing Report')
plt.xlabel('Rank ID')
plt.ylabel( 'Time(seconds)')
plt.legend()
plt.savefig('WriteTime.png')

figure(3)
plt.plot(rank,close, 'green', label = 'Close Time')
plt.title('Timing Report')
plt.xlabel('Rank ID')
plt.ylabel( 'Time(seconds)')
plt.legend()
plt.savefig('CloseTime.png')
