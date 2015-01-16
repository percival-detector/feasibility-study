#!/bin/env python

from pysnmp.entity.rfc3413.oneliner import cmdgen
import json
from datetime import datetime
import sys

switchName = 'devswitch5920'
maxPort = 25

def saveCounters():
    oidDict = {
        'ifIndex'       : (1,3,6,1,2,1,2,2,1,1),
        'ifName'        : (1,3,6,1,2,1,31,1,1,1,1),
        'ifAlias'       : (1,3,6,1,2,1,31,1,1,1,18),
        'ifHCInOctets'  : (1,3,6,1,2,1,31,1,1,1,6),
        'ifHCOutOctets' : (1,3,6,1,2,1,31,1,1,1,10),
        'ifInUcastPkts' : (1,3,6,1,2,1,2,2,1,11),
        'ifOutUcastPkts' : (1,3,6,1,2,1,2,2,1,17),
        }
    
    cmdGen = cmdgen.CommandGenerator()
    
    errorIndication, errorStatus, errorIndex, t = cmdGen.nextCmd(
        cmdgen.CommunityData('server', 'public', 1),
        cmdgen.UdpTransportTarget((switchName, 161)),
        oidDict['ifName'],
        oidDict['ifIndex'],
        oidDict['ifInUcastPkts'],
        oidDict['ifOutUcastPkts']
    )
    
    # Check for errors and print out results
    if errorIndication:
        print(errorIndication)
    elif errorStatus:
        print(errorStatus)
    else:
        
        fileName = datetime.now().strftime("switchPkts-%Y%m%d-%H%M%S.json")
        packetCounters={}
        for line in t:
            idx = t.index(line)
            name = str(t[idx][0][1])
            if name != '':
                index = int(str(t[idx][1][1]))
                inPkts = int(str(t[idx][2][1]))
                outPkts = int(str(t[idx][3][1]))
                if index <= maxPort:
                    packetCounters[index] = (name, inPkts, outPkts)
                    #print index, name, inPkts, outPkts
        
        print "Saving", switchName, "packet counters to file", fileName            
        f = open(fileName, 'w')
        json.dump(packetCounters, f, sort_keys=True, indent=4, separators=(',', ':'))
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
    
    print "%3s%-26s %10s %10s" % ("Idx", " Name", "InPkts", "OutPkts")
    for index in sorted(packetCountersStart, key=lambda k: int(k)):
        name = packetCountersStart[index][0]
        inDelta  = packetCountersEnd[index][1] - packetCountersStart[index][1]
        outDelta = packetCountersEnd[index][2] - packetCountersStart[index][2]
        print "%02d %26s %10d %10d" % (int(index), name, inDelta, outDelta)
    
if __name__ == '__main__':
    
    if len(sys.argv) == 3:
        calculateDelta(sys.argv[1], sys.argv[2])
    else:
        saveCounters()