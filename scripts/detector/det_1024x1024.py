#!/dls_sw/prod/tools/RHEL6-x86_64/Python/2-7-3/prefix/bin/python

# Script that configures PERCIVAL.
# For use when the IOCs are up and running.


import optparse, sys, os, re, subprocess

class Worker(object):
    pvRoot = 'PERC:DET-01:'

    def __init__(self):
        '''Constructor.  Actually performs all the work.'''
        self.setEnvironment()
        self.doConfig()

    def doConfig(self):
        '''Setup the Detector.'''
#        self.caPut('PercAddr',           '172.23.138.77')
        self.caPut('PercAddr',           '192.168.138.77')
        self.caPut('PercPort',           9876)
        self.caPut('PercPacketBytes',    8966)
        self.caPut('PercSubFrame',       0)


        self.caPut('PercTopLeftXSF1',    0)
        self.caPut('PercTopLeftYSF1',    0)
        self.caPut('PercBotRightXSF1',   127)
        self.caPut('PercBotRightYSF1',   1023)
        self.caPut('PercSubFrameID1',    0)

        self.caPut('PercTopLeftXSF2',    128)
        self.caPut('PercTopLeftYSF2',    0)
        self.caPut('PercBotRightXSF2',   255)
        self.caPut('PercBotRightYSF2',   1023)
        self.caPut('PercSubFrameID2',    1)

        self.caPut('PercTopLeftXSF3',    256)
        self.caPut('PercTopLeftYSF3',    0)
        self.caPut('PercBotRightXSF3',   383)
        self.caPut('PercBotRightYSF3',   1023)
        self.caPut('PercSubFrameID3',    2)

        self.caPut('PercTopLeftXSF4',    384)
        self.caPut('PercTopLeftYSF4',    0)
        self.caPut('PercBotRightXSF4',   511)
        self.caPut('PercBotRightYSF4',   1023)
        self.caPut('PercSubFrameID4',    3)

        self.caPut('PercTopLeftXSF5',    512)
        self.caPut('PercTopLeftYSF5',    0)
        self.caPut('PercBotRightXSF5',   639)
        self.caPut('PercBotRightYSF5',   1023)
        self.caPut('PercSubFrameID5',    4)

        self.caPut('PercTopLeftXSF6',    640)
        self.caPut('PercTopLeftYSF6',    0)
        self.caPut('PercBotRightXSF6',   767)
        self.caPut('PercBotRightYSF6',   1023)
        self.caPut('PercSubFrameID6',    5)

        self.caPut('PercTopLeftXSF7',    768)
        self.caPut('PercTopLeftYSF7',    0)
        self.caPut('PercBotRightXSF7',   895)
        self.caPut('PercBotRightYSF7',   1023)
        self.caPut('PercSubFrameID7',    6)

        self.caPut('PercTopLeftXSF8',    896)
        self.caPut('PercTopLeftYSF8',    0)
        self.caPut('PercBotRightXSF8',   1023)
        self.caPut('PercBotRightYSF8',   1023)
        self.caPut('PercSubFrameID8',    7)

        self.caPut('Descramble',          'Descramble')

        self.caPut('PercFilePath',       '/dls_sw/work/R3.14.12.3/support/percival', True)
#        self.caPut('PercFilePath',       '/home/gnx91527', True)
        self.caPut('PercFileName',       'perc_1024x1024.hdf5', True)
        self.caPut('PercConfigFileRead', 1)

    def setEnvironment(self):
        os.environ['EPICS_HOST_ARCH'] = "linux-x86_64"
        os.environ['EPICS_BASE'] = "%s/base" % os.getcwd()
        os.environ['PATH'] += ":%s/extensions/bin/linux-x86_64:%s/python/dls_epicsparser/dls_epicsparser:%s/base/bin/linux-x86_64" % (os.getcwd(), os.getcwd(), os.getcwd())

    def caPut(self, pv, value, longString=False):
        opts = ""
        if longString:
            opts = "-S "
        os.system('caput %s%s%s %s' % (opts, self.pvRoot, pv, value))

    def caPutArray(self, pv, length, value):
        opts = "-a -w 10 "
        cmd = 'caput %s%s%s %s ' % (opts, self.pvRoot, pv, length)
        for item in value:
            cmd += '%s ' % item
        os.system(cmd)
        print repr(cmd)

    def caGet(self, pv):
        (stdout, stderr) = subprocess.Popen(['caget', self.pvRoot+pv], stdout=subprocess.PIPE).communicate()
        return stdout.split(None, 1)[1].strip()

def main():
    Worker()

if __name__ == "__main__":
    main()
