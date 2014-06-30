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
generate = []


for line in f:
    try:
         dict = json.loads( line )
         listOfDictionaries.append(dict)
    except ValueError:
        thisDoesntDoAnything = 10

f.close()



for x in listOfDictionaries:
  rank.append(x['rank'])
  generate.append(x['Generate Time'])

pyplot.scatter(rank,generate, label = 'Generate')
pyplot.title('Timing Report')
pyplot.xlabel('Rank ID')
pyplot.ylabel( 'Time(seconds)')
pyplot.legend()
pyplot.savefig('bimodalGenerateWith16Threads1Rank.png')






