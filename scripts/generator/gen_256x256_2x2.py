#!/dls_sw/prod/tools/RHEL6-x86_64/Python/2-7-3/prefix/bin/python

# Script that configures PERCIVAL.
# For use when the IOCs are up and running.


import optparse, sys, os, re, subprocess

class Worker(object):
    pvRoot = 'test:gen:'

    def __init__(self):
        '''Constructor.  Actually performs all the work.'''
        self.setEnvironment()
        self.doConfig()

    def doConfig(self):
        '''Setup the Generator.'''
        self.caPut('GDTopLeftXChannel1',  0)
        self.caPut('GDTopLeftYChannel1',  0)
        self.caPut('GDBotRightXChannel1', 127)
        self.caPut('GDBotRightYChannel1', 127)
        self.caPut('GDSubFramesChannel1', 2)
        self.caPut('GDPktSizeChannel1',   5000)
        self.caPut('Channel1Enable',      'Enabled')

        self.caPut('GDTopLeftXChannel2',  128)
        self.caPut('GDTopLeftYChannel2',  0)
        self.caPut('GDBotRightXChannel2', 255)
        self.caPut('GDBotRightYChannel2', 127)
        self.caPut('GDSubFramesChannel2', 2)
        self.caPut('GDPktSizeChannel2',   5000)
        self.caPut('Channel2Enable',      'Enabled')

        self.caPut('GDTopLeftXChannel3',  0)
        self.caPut('GDTopLeftYChannel3',  128)
        self.caPut('GDBotRightXChannel3', 127)
        self.caPut('GDBotRightYChannel3', 255)
        self.caPut('GDSubFramesChannel3', 2)
        self.caPut('GDPktSizeChannel3',   5000)
        self.caPut('Channel3Enable',      'Enabled')

        self.caPut('GDTopLeftXChannel4',  128)
        self.caPut('GDTopLeftYChannel4',  128)
        self.caPut('GDBotRightXChannel4', 255)
        self.caPut('GDBotRightYChannel4', 255)
        self.caPut('GDSubFramesChannel4', 2)
        self.caPut('GDPktSizeChannel4',   5000)
        self.caPut('Channel4Enable',      'Enabled')

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
