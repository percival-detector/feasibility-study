
from iocbuilder import Device, AutoSubstitution, Architecture, SetSimulation
from iocbuilder.arginfo import *
from iocbuilder.modules.asyn import Asyn, AsynPort, AsynIP

class _PercivalConfigurator(AutoSubstitution):
    TemplateFile="PercivalConfigurator.template"

class PercivalConfigurator(AsynPort):
    """Create a Percival Configurator"""
    _BaseTemplate = _PercivalConfigurator
    _SpecificTemplate = None
    AutoInstantiate = True
    def __init__(self, **args):
        self.__super.__init__(args["PORT"])
        # Init the base class
        self.base = self._BaseTemplate(**filter_dict(args,
            self._BaseTemplate.ArgInfo.Names()))
        # Init the template class
        if self._SpecificTemplate:
            self.template = self._SpecificTemplate(**filter_dict(args,
                self._SpecificTemplate.ArgInfo.Names()))
            self.__dict__.update(self.template.args)
        # Update args
        self.__dict__.update(self.base.args)
        #self.__dict__.update(locals())
        
    # __init__ arguments
    ArgInfo = _BaseTemplate.ArgInfo
    LibFileList = ['percival']
    DbdFileList = ['PercivalConfigurator']
    SysLibFileList = []
    MakefileStringList = []
    epics_host_arch = Architecture()
    
    def Initialise(self):
        print '# ConfiguratorInit( portName )'
        print 'ConfiguratorInit( %(PORT)s )' % self.__dict__


class _PercivalGenerator(AutoSubstitution):
    TemplateFile="PercivalGenerator.template"

class PercivalGenerator(AsynPort):
    """Create a Percival Generator"""
    _BaseTemplate = _PercivalGenerator
    _SpecificTemplate = None
    AutoInstantiate = True
    def __init__(self, **args):
        self.__super.__init__(args["PORT"])
        # Init the base class
        self.base = self._BaseTemplate(**filter_dict(args,
            self._BaseTemplate.ArgInfo.Names()))
        # Init the template class
        if self._SpecificTemplate:
            self.template = self._SpecificTemplate(**filter_dict(args,
                self._SpecificTemplate.ArgInfo.Names()))
            self.__dict__.update(self.template.args)
        # Update args
        self.__dict__.update(self.base.args)
        #self.__dict__.update(locals())
        
    # __init__ arguments
    ArgInfo = _BaseTemplate.ArgInfo
    LibFileList = ['percival']
    DbdFileList = ['PercivalGenerator']
    SysLibFileList = []
    MakefileStringList = []
    epics_host_arch = Architecture()
    
    def Initialise(self):
        print '# GeneratorInit( portName )'
        print 'GeneratorInit( %(PORT)s )' % self.__dict__


