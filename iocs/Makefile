TOP = ..
include $(TOP)/configure/CONFIG

ifeq ($(EPICS_HOST_ARCH), linux-x86)
	DIRS := $(wildcard example example_sim)
else ifeq ($(EPICS_HOST_ARCH), linux-x86_64)
	DIRS := percivalGeneratorMaster percivalGenerator01 percivalGenerator02 percivalGenerator03 percivalGenerator04 percivalGenerator05 percivalGenerator06 percivalGenerator07 percivalGenerator08 percivalDetector01 percivalDetector02 percivalDetector03 percivalDetector04 percivalDetector05 percivalDetector06 percivalDetector07 percivalDetector08
else ifeq ($(EPICS_HOST_ARCH), win32-x86)
	DIRS :=
endif

include $(TOP)/configure/RULES_DIRS
