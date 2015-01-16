#!/bin/env python

from datetime import datetime
import sys
import subprocess


fileStem = "testThroughputMulti_"
remoteFilePath="/tmp"
timeStamp=datetime.now().strftime("%Y%m%d-%H%M%S")

for node in range(1,9):
    for mode in ['tx', 'rx']:
        
        node_name = 'te7aegnode%02d' % node
        remoteFileName = remoteFilePath + "/" + fileStem + mode + ".log"
        gatheredFileName = fileStem + mode + "-node" + str(node) + "-" + timeStamp + ".log"
        
        scp_cmd_str="/usr/bin/scp " + node_name + ":" + remoteFileName + " " + gatheredFileName
        print node, scp_cmd_str
        subprocess.call(scp_cmd_str, shell=True)