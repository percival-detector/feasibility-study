#!/dls_sw/prod/tools/RHEL6-x86_64/Python/2-7-3/prefix/bin/python

# Script that configures PERCIVAL.
# For use when the IOCs are up and running.


import optparse, sys, os, re, subprocess

class Worker(object):
    pvRoot = 'PERC:GEN-06:'

    def __init__(self):
        '''Constructor.  Actually performs all the work.'''
        self.setEnvironment()
        self.doConfig()

    def doConfig(self):
        '''Setup the Generator.'''
        self.caPut('GDTopLeftXChannel1',  0)
        self.caPut('GDTopLeftYChannel1',  0)
        self.caPut('GDBotRightXChannel1', 511)
        self.caPut('GDBotRightYChannel1', 4095)
        self.caPut('GDSubFramesChannel1', 5)
        self.caPut('GDPktSizeChannel1',   8000)
        self.caPut('Channel1Enable',      'Enabled')
        self.caPut('GDRemPortChannel1',   9881)
#        self.caPut('GDLocAddrChannel1',   '172.23.138.78')
        self.caPut('GDLocAddrChannel1',   '192.168.138.78')
        self.caPut('GDLocPortChannel1',   7279)
#        self.caPut('GDRemAddrChannel1',   'cs04r-sc-serv-77')
        self.caPut('GDRemAddrChannel1',   '192.168.138.77')

        self.caPut('GDTopLeftXChannel2',  512)
        self.caPut('GDTopLeftYChannel2',  0)
        self.caPut('GDBotRightXChannel2', 1023)
        self.caPut('GDBotRightYChannel2', 4095)

        self.caPut('GDTopLeftXChannel3',  1024)
        self.caPut('GDTopLeftYChannel3',  0)
        self.caPut('GDBotRightXChannel3', 1535)
        self.caPut('GDBotRightYChannel3', 4095)

        self.caPut('GDTopLeftXChannel4',  1536)
        self.caPut('GDTopLeftYChannel4',  0)
        self.caPut('GDBotRightXChannel4', 2047)
        self.caPut('GDBotRightYChannel4', 4095)

        self.caPut('GDTopLeftXChannel5',  2048)
        self.caPut('GDTopLeftYChannel5',  0)
        self.caPut('GDBotRightXChannel5', 2559)
        self.caPut('GDBotRightYChannel5', 4095)

        self.caPut('GDTopLeftXChannel6',  2560)
        self.caPut('GDTopLeftYChannel6',  0)
        self.caPut('GDBotRightXChannel6', 3071)
        self.caPut('GDBotRightYChannel6', 4095)

        self.caPut('GDTopLeftXChannel7',  3072)
        self.caPut('GDTopLeftYChannel7',  0)
        self.caPut('GDBotRightXChannel7', 3583)
        self.caPut('GDBotRightYChannel7', 4095)

        self.caPut('GDTopLeftXChannel8',  3584)
        self.caPut('GDTopLeftYChannel8',  0)
        self.caPut('GDBotRightXChannel8', 4095)
        self.caPut('GDBotRightYChannel8', 4095)

        self.caPut('FilePath',            '/dls_sw/work/R3.14.12.3/support/percival', True)
        self.caPut('FileName',            '4096x4096.hdf5', True)
        self.caPut('RawFileRead',         1)

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
