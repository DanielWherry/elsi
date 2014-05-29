import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import numpy as np
from optparse import OptionParser
import json

parser = OptionParser()

parser.add_option("-f", "--file", action="store", type="string", dest="filename")

(options,args) = parser.parse_args()

f = open(options.filename, 'r')
rank = []
open = []
generate = []
write = []
close = []
for line in f:
    dict = json.loads( line )
    rank.append( dict['rank'])
    open.append(dict['Open Time'])
    generate.append(dict['Generation Time'])
    write.append(dict['Write Time'])
    close.append(dict['Close Time'])
    	 
f.close()

plt.plot(rank,open)#,'r^',label="Open Time")
plt.plot(rank,generate)#,'ys', label="Generate Time")
plt.plot(rank,write)#,'gs', label="Write Time")
plt.scatter(rank,close)
plt.title('Makin\' graphs for Robert n stuff')
plt.xlabel('Rank ID')
plt.ylabel( 'Time(seconds)')
plt.savefig('FirstGraph.png')
