#!/bin/env python

import json
from datetime import datetime
import sys
import subprocess
import re

nodeList= { 'te7aegnode01' : ['p2p1', 'p2p2'],
            'te7aegnode02' : ['p2p1', 'p2p2'],
            'te7aegnode03' : ['em1',  'em2' ],
            'te7aegnode04' : ['em1',  'em2' ],
            'te7aegnode05' : ['em1',  'em2' ],
            'te7aegnode06' : ['em1',  'em2' ],
            'te7aegnode07' : ['em1',  'em2' ],
            'te7aegnode08' : ['em1',  'em2' ],
          }

def saveCounters():

    rxline_re = re.compile('RX\ packets')
    txline_re = re.compile('TX\ packets')
    counter_re = re.compile('\d+')
    
    nodeCounters={}
    
    fileName = datetime.now().strftime('nodePkts-%Y%m%d-%H%M%S.json')
      
    for node in sorted(nodeList):
        for interface in nodeList[node]:
            cmd_str = ["ssh", node, "/sbin/ifconfig", interface]
#            print cmd_str
            print "Retreiving packet counters for node", node, "interface", interface, "..."
            response = subprocess.Popen(cmd_str, stdout=subprocess.PIPE).communicate()[0]
#            print response
            for line in str.splitlines(response):
#                print ">>>", line
                if rxline_re.search(line):
                    rx_counters = [int(x) for x in counter_re.findall(line)]
                if txline_re.search(line):
                    tx_counters = [int(x) for x in counter_re.findall(line)]
                    
#            print rx_counters, tx_counters
            nodeIfaceKey = node + "_" + interface
            nodeCounters[nodeIfaceKey] = [rx_counters, tx_counters]
            
#    print nodeCounters
    print "Saving node counters to file", fileName
    f = open(fileName, 'w')
    json.dump(nodeCounters, f, sort_keys=True, indent=4, separators=(',', ":"))
    f.close()
    
def calculateDelta(fileName1, fileName2):
    
    print "Reading start counters from file", fileName1
    f1 = open(fileName1, 'r')
    packetCountersStart = json.load(f1)
    f1.close()
    
    print "Reading end counters from file", fileName2
    f2 = open(fileName2, 'r')
    packetCountersEnd = json.load(f2)
    f2.close()
    
    packetDelta={}
    print "{0:20}".format("Node"), " : ", " ".join("{0:>12}".format(rx_key) for rx_key in ("RX Packets", "RX Errors", "RX Dropped", "RX Overrun", "RX Frame")), \
      " ".join("{0:>12}".format(tx_key) for tx_key in ("TX Packets","TX Errors", "TX Dropped", "TX Overrun", "TX Carrier"))
    for nodeIface in sorted(packetCountersStart):
        #print nodeIface, packetCountersStart[nodeIface], packetCountersEnd[nodeIface]
        [rxStartCounters, txStartCounters] = packetCountersStart[nodeIface]
        [rxEndCounters, txEndCounters] = packetCountersEnd[nodeIface]
        rxDelta = [end_i - start_i for start_i, end_i in zip(rxStartCounters, rxEndCounters)]
        txDelta = [end_i - start_i for start_i, end_i in zip(txStartCounters, txEndCounters)]
        #print "%20s : %10d %10d %10d %10d %10d %10d %10d %10d %10d %10d" % (nodeIface, rxDelta, txDelta)
        print "{0:20}".format(nodeIface), " : ", " ".join("{0:12d}".format(rx_i) for rx_i in rxDelta), " ".join("{0:12d}".format(tx_i) for tx_i in txDelta)
        
if __name__ == '__main__':
    
    if len(sys.argv) == 3:
        calculateDelta(sys.argv[1], sys.argv[2])
    else:
        saveCounters()