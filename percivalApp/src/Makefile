TOP=../..

include $(TOP)/configure/CONFIG

USR_INCLUDES += -I$(HDF5)/include
USR_INCLUDES += -I$(BOOST)/include

# ----------------------------------------
# Build a basic test application
#-----------------------------------------
PROD_IOC += testSend
testSend_SRCS += sendTest.cpp
testSend_SRCS += DataSender.cpp
testSend_SRCS += PercivalDebug.cpp
testSend_LIBS += boost_system
testSend_LIBS += boost_thread
boost_system_DIR = $(BOOST)/lib
boost_thread_DIR = $(BOOST)/lib

# ----------------------------------------
# Build a basic test application
#-----------------------------------------
PROD_IOC += testPackets
testPackets_SRCS += testPercivalPacketChecker.cpp
testPackets_SRCS += PercivalPacketChecker.cpp
testPackets_SRCS += PercivalDebug.cpp
testPackets_LIBS += boost_system
testPackets_LIBS += boost_thread
boost_system_DIR = $(BOOST)/lib
boost_thread_DIR = $(BOOST)/lib

# ----------------------------------------
# Build a basic test application
#-----------------------------------------
PROD_IOC += testConfig
testConfig_SRCS += testConfigurator.cpp
testConfig_SRCS += Configurator.cpp
testConfig_SRCS += PercivalDebug.cpp
testConfig_LIBS += boost_system
testConfig_LIBS += boost_thread
testConfig_LIBS += hdf5
hdf5_DIR         = $(HDF5)/lib
boost_system_DIR = $(BOOST)/lib
boost_thread_DIR = $(BOOST)/lib

# ----------------------------------------
# Build a basic test application
#-----------------------------------------
PROD_IOC += testPercServer
testPercServer_SRCS += testPercivalServer.cpp
testPercServer_SRCS += PercivalServer.cpp
testPercServer_SRCS += PercivalSubFrame.cpp
testPercServer_SRCS += PercivalBuffer.cpp
testPercServer_SRCS += PercivalBufferPool.cpp
testPercServer_SRCS += DataReceiver.cpp
testPercServer_SRCS += PercivalDebug.cpp
testPercServer_SRCS += PercivalPacketChecker.cpp
testPercServer_LIBS += boost_system
testPercServer_LIBS += boost_thread
boost_system_DIR = $(BOOST)/lib
boost_thread_DIR = $(BOOST)/lib

# ----------------------------------------
# Build a basic test application
#-----------------------------------------
PROD_IOC += testServer
testServer_SRCS += testUDPServer.cpp
testServer_LIBS += boost_system
testServer_LIBS += boost_thread
boost_system_DIR = $(BOOST)/lib
boost_thread_DIR = $(BOOST)/lib

# -------------------------------
# Build an Diamond Support Module
# -------------------------------

LIBRARY_IOC += percGen

# xxxRecord.h will be created from xxxRecord.dbd
#DBDINC += xxx.h

# xxxRecord.dbd will be installed into <top>/dbd
DBD += PercivalConfigurator.dbd
DBD += PercivalGenerator.dbd

# The following are compiled and added to the support library
percGen_SRCS += asynConfiguratorDriver.cpp
#percGen_SRCS += asynGeneratorDriver.cpp
#percGen_SRCS += asynTemporalGeneratorDriver.cpp
percGen_SRCS += asynSingleProcessGenerator.cpp
percGen_SRCS += Configurator.cpp
percGen_SRCS += DataSender.cpp
percGen_SRCS += PercivalDebug.cpp

percGen_LIBS += asyn
percGen_LIBS += hdf5
percGen_LIBS += boost_system
percGen_LIBS += boost_thread
hdf5_DIR         = $(HDF5)/lib
boost_system_DIR = $(BOOST)/lib
boost_thread_DIR = $(BOOST)/lib

# We need to link against the EPICS Base libraries
#percival_LIBS += $(EPICS_BASE_IOC_LIBS)

# -------------------------------
# Build an Diamond Support Module
# -------------------------------

LIBRARY_IOC += percival

# xxxRecord.h will be created from xxxRecord.dbd
#DBDINC += xxx.h

# xxxRecord.dbd will be installed into <top>/dbd
DBD += PercivalConfigurator.dbd
#DBD += PercivalGenerator.dbd
DBD += ADPercivalDriver.dbd

# The following are compiled and added to the support library
percival_SRCS += asynConfiguratorDriver.cpp
#percival_SRCS += asynGeneratorDriver.cpp
#percival_SRCS += asynTemporalGeneratorDriver.cpp
percival_SRCS += asynSingleProcessGenerator.cpp
percival_SRCS += ADPercivalDriver.cpp
percival_SRCS += Configurator.cpp
percival_SRCS += DataSender.cpp
percival_SRCS += DataReceiver.cpp
percival_SRCS += PercivalDebug.cpp
percival_SRCS += PercivalBuffer.cpp
percival_SRCS += PercivalBufferPool.cpp
percival_SRCS += PercivalServer.cpp
percival_SRCS += PercivalSubFrame.cpp
percival_SRCS += PercivalPacketChecker.cpp
percival_SRCS += CPUstats.cpp

#percival_LIBS += asyn
#percival_LIBS += hdf5
#percival_LIBS += ADBase
percival_LIBS += boost_system
percival_LIBS += boost_thread
#hdf5_DIR         = $(HDF5)/lib
boost_system_DIR = $(BOOST)/lib
boost_thread_DIR = $(BOOST)/lib

# We need to link against the EPICS Base libraries
#percival_LIBS += $(EPICS_BASE_IOC_LIBS)


# ------------------------
# Build an IOC Application
# ------------------------

#PROD_IOC = percival

# percival.dbd will be installed into <top>/dbd
#DBD += percival.dbd

# percival.dbd will be created from these files
#percival_DBD += base.dbd
#percival_DBD += xxx.dbd

# percival_registerRecordDeviceDriver.cpp will be created
# percival.dbd
#percival_SRCS += percival_registerRecordDeviceDriver.cpp

# These two lines are needed for non-vxWorks builds, such as Linux
#percival_SRCS_DEFAULT += percivalMain.cpp
#percival_SRCS_vxWorks += -nil-

# Add locally compiled object code
#percival_SRCS +=

# The following adds object code from base/src/vxWorks
#percival_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# This line says that this IOC Application depends on the
# xxx Support Module
#percival_LIBS += xxx

# We need to link this IOC Application against the EPICS Base libraries
#percival_LIBS += $(EPICS_BASE_IOC_LIBS)

# ---------------------------------------------------

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file
#
# The difference between a file with an "st" extension
# and one with an "stt" extension is:
# The "st" extension files get passed through the C preprocessor
# whereas the "stt" files do not.

#ifneq ($(SNCSEQ),)
    # This builds sncExample as a component of percival
#    percival_SNCFLAGS += +r
#    percival_DBD      += sncExample.dbd
#    percival_SRCS     += sncExample.stt
#    percival_LIBS     += seq pv

    # The following builds sncExample as a standalone application
#    PROD_HOST           += sncExample
#    sncExample_SNCFLAGS += +m
#    sncExample_SRCS     += sncExample.stt
#    sncExample_LIBS     += seq pv
    # Here, we need to specify the dependency on EPICS Base libraries
    # again because this is a standalone application and
    # not part of percival
#    sncExample_LIBS     += $(EPICS_BASE_HOST_LIBS)
#endif

include $(TOP)/configure/RULES
