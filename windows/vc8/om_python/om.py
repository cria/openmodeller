# This file was created automatically by SWIG.
# Don't modify this file, modify the SWIG interface instead.
# This file is compatible with both classic and new-style classes.
import _om
def _swig_setattr(self,class_type,name,value):
    if (name == "this"):
        if isinstance(value, class_type):
            self.__dict__[name] = value.this
            if hasattr(value,"thisown"): self.__dict__["thisown"] = value.thisown
            del value.thisown
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    self.__dict__[name] = value

def _swig_getattr(self,class_type,name):
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError,name

import types
try:
    _object = types.ObjectType
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0


OM_WGS84 = _om.OM_WGS84
OM_COORDINATE_SYSTEM = _om.OM_COORDINATE_SYSTEM
Zero = _om.Zero
print_args = _om.print_args

print_alg_params = _om.print_alg_params

class Log(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Log, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Log, name)
    Debug = _om.Log_Debug
    Info = _om.Log_Info
    Warn = _om.Log_Warn
    Error = _om.Log_Error
    def __init__(self,*args):
        _swig_setattr(self, Log, 'this', apply(_om.new_Log,args))
        _swig_setattr(self, Log, 'thisown', 1)
    def __del__(self, destroy= _om.delete_Log):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C Log instance at %s>" % (self.this,)

class LogPtr(Log):
    def __init__(self,this):
        _swig_setattr(self, Log, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Log, 'thisown', 0)
        _swig_setattr(self, Log,self.__class__,Log)
_om.Log_swigregister(LogPtr)

class Serializable(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Serializable, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Serializable, name)
    def __init__(self,*args):
        _swig_setattr(self, Serializable, 'this', apply(_om.new_Serializable,args))
        _swig_setattr(self, Serializable, 'thisown', 1)
    def __del__(self, destroy= _om.delete_Serializable):
        try:
            if self.thisown: destroy(self)
        except: pass
    def serialize(*args): return apply(_om.Serializable_serialize,args)
    def deserialize(*args): return apply(_om.Serializable_deserialize,args)
    def toString(*args): return apply(_om.Serializable_toString,args)
    def fromString(*args): return apply(_om.Serializable_fromString,args)
    def toFile(*args): return apply(_om.Serializable_toFile,args)
    def fromFile(*args): return apply(_om.Serializable_fromFile,args)
    def __repr__(self):
        return "<C Serializable instance at %s>" % (self.this,)

class SerializablePtr(Serializable):
    def __init__(self,this):
        _swig_setattr(self, Serializable, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Serializable, 'thisown', 0)
        _swig_setattr(self, Serializable,self.__class__,Serializable)
_om.Serializable_swigregister(SerializablePtr)

class MapFormat(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, MapFormat, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, MapFormat, name)
    def __init__(self,*args):
        _swig_setattr(self, MapFormat, 'this', apply(_om.new_MapFormat,args))
        _swig_setattr(self, MapFormat, 'thisown', 1)
    def __del__(self, destroy= _om.delete_MapFormat):
        try:
            if self.thisown: destroy(self)
        except: pass
    def getWidth(*args): return apply(_om.MapFormat_getWidth,args)
    def getHeight(*args): return apply(_om.MapFormat_getHeight,args)
    def getXMin(*args): return apply(_om.MapFormat_getXMin,args)
    def getYMin(*args): return apply(_om.MapFormat_getYMin,args)
    def getXMax(*args): return apply(_om.MapFormat_getXMax,args)
    def getYMax(*args): return apply(_om.MapFormat_getYMax,args)
    def getNoDataValue(*args): return apply(_om.MapFormat_getNoDataValue,args)
    def getProjection(*args): return apply(_om.MapFormat_getProjection,args)
    def __repr__(self):
        return "<C MapFormat instance at %s>" % (self.this,)

class MapFormatPtr(MapFormat):
    def __init__(self,this):
        _swig_setattr(self, MapFormat, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, MapFormat, 'thisown', 0)
        _swig_setattr(self, MapFormat,self.__class__,MapFormat)
_om.MapFormat_swigregister(MapFormatPtr)

class Occurrences(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Occurrences, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Occurrences, name)
    def __init__(self,*args):
        _swig_setattr(self, Occurrences, 'this', apply(_om.new_Occurrences,args))
        _swig_setattr(self, Occurrences, 'thisown', 1)
    def __del__(self, destroy= _om.delete_Occurrences):
        try:
            if self.thisown: destroy(self)
        except: pass
    def name(*args): return apply(_om.Occurrences_name,args)
    def coordSystem(*args): return apply(_om.Occurrences_coordSystem,args)
    def insert(*args): return apply(_om.Occurrences_insert,args)
    def numAttributes(*args): return apply(_om.Occurrences_numAttributes,args)
    def numOccurrences(*args): return apply(_om.Occurrences_numOccurrences,args)
    def head(*args): return apply(_om.Occurrences_head,args)
    def next(*args): return apply(_om.Occurrences_next,args)
    def get(*args): return apply(_om.Occurrences_get,args)
    def remove(*args): return apply(_om.Occurrences_remove,args)
    def getRandom(*args): return apply(_om.Occurrences_getRandom,args)
    def printOccurrences(*args): return apply(_om.Occurrences_printOccurrences,args)
    def __repr__(self):
        return "<C Occurrences instance at %s>" % (self.this,)

class OccurrencesPtr(Occurrences):
    def __init__(self,this):
        _swig_setattr(self, Occurrences, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Occurrences, 'thisown', 0)
        _swig_setattr(self, Occurrences,self.__class__,Occurrences)
_om.Occurrences_swigregister(OccurrencesPtr)

class AlgParameter(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, AlgParameter, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, AlgParameter, name)
    def __init__(self,*args):
        _swig_setattr(self, AlgParameter, 'this', apply(_om.new_AlgParameter,args))
        _swig_setattr(self, AlgParameter, 'thisown', 1)
    def __del__(self, destroy= _om.delete_AlgParameter):
        try:
            if self.thisown: destroy(self)
        except: pass
    def id(*args): return apply(_om.AlgParameter_id,args)
    def idCopy(*args): return apply(_om.AlgParameter_idCopy,args)
    def setId(*args): return apply(_om.AlgParameter_setId,args)
    def value(*args): return apply(_om.AlgParameter_value,args)
    def valueReal(*args): return apply(_om.AlgParameter_valueReal,args)
    def valueCopy(*args): return apply(_om.AlgParameter_valueCopy,args)
    def setValue(*args): return apply(_om.AlgParameter_setValue,args)
    def __repr__(self):
        return "<C AlgParameter instance at %s>" % (self.this,)

class AlgParameterPtr(AlgParameter):
    def __init__(self,this):
        _swig_setattr(self, AlgParameter, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, AlgParameter, 'thisown', 0)
        _swig_setattr(self, AlgParameter,self.__class__,AlgParameter)
_om.AlgParameter_swigregister(AlgParameterPtr)

class FileParser(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, FileParser, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, FileParser, name)
    def __init__(self,*args):
        _swig_setattr(self, FileParser, 'this', apply(_om.new_FileParser,args))
        _swig_setattr(self, FileParser, 'thisown', 1)
    def __del__(self, destroy= _om.delete_FileParser):
        try:
            if self.thisown: destroy(self)
        except: pass
    def load(*args): return apply(_om.FileParser_load,args)
    def get(*args): return apply(_om.FileParser_get,args)
    def count(*args): return apply(_om.FileParser_count,args)
    def getAll(*args): return apply(_om.FileParser_getAll,args)
    def length(*args): return apply(_om.FileParser_length,args)
    def head(*args): return apply(_om.FileParser_head,args)
    def next(*args): return apply(_om.FileParser_next,args)
    def key(*args): return apply(_om.FileParser_key,args)
    def value(*args): return apply(_om.FileParser_value,args)
    def __repr__(self):
        return "<C FileParser instance at %s>" % (self.this,)

class FileParserPtr(FileParser):
    def __init__(self,this):
        _swig_setattr(self, FileParser, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, FileParser, 'thisown', 0)
        _swig_setattr(self, FileParser,self.__class__,FileParser)
_om.FileParser_swigregister(FileParserPtr)

class AreaStats(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, AreaStats, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, AreaStats, name)
    def __init__(self,*args):
        _swig_setattr(self, AreaStats, 'this', apply(_om.new_AreaStats,args))
        _swig_setattr(self, AreaStats, 'thisown', 1)
    def __del__(self, destroy= _om.delete_AreaStats):
        try:
            if self.thisown: destroy(self)
        except: pass
    def reset(*args): return apply(_om.AreaStats_reset,args)
    def addPrediction(*args): return apply(_om.AreaStats_addPrediction,args)
    def addNonPrediction(*args): return apply(_om.AreaStats_addNonPrediction,args)
    def getTotalArea(*args): return apply(_om.AreaStats_getTotalArea,args)
    def getAreaPredictedPresent(*args): return apply(_om.AreaStats_getAreaPredictedPresent,args)
    def getAreaPredictedAbsent(*args): return apply(_om.AreaStats_getAreaPredictedAbsent,args)
    def getAreaNotPredicted(*args): return apply(_om.AreaStats_getAreaNotPredicted,args)
    def __repr__(self):
        return "<C AreaStats instance at %s>" % (self.this,)

class AreaStatsPtr(AreaStats):
    def __init__(self,this):
        _swig_setattr(self, AreaStats, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, AreaStats, 'thisown', 0)
        _swig_setattr(self, AreaStats,self.__class__,AreaStats)
_om.AreaStats_swigregister(AreaStatsPtr)

class ConfusionMatrix(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, ConfusionMatrix, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, ConfusionMatrix, name)
    def __init__(self,*args):
        _swig_setattr(self, ConfusionMatrix, 'this', apply(_om.new_ConfusionMatrix,args))
        _swig_setattr(self, ConfusionMatrix, 'thisown', 1)
    def __del__(self, destroy= _om.delete_ConfusionMatrix):
        try:
            if self.thisown: destroy(self)
        except: pass
    def reset(*args): return apply(_om.ConfusionMatrix_reset,args)
    def calculate(*args): return apply(_om.ConfusionMatrix_calculate,args)
    def getValue(*args): return apply(_om.ConfusionMatrix_getValue,args)
    def getAccuracy(*args): return apply(_om.ConfusionMatrix_getAccuracy,args)
    def getOmissionError(*args): return apply(_om.ConfusionMatrix_getOmissionError,args)
    def getCommissionError(*args): return apply(_om.ConfusionMatrix_getCommissionError,args)
    def ready(*args): return apply(_om.ConfusionMatrix_ready,args)
    def __repr__(self):
        return "<C ConfusionMatrix instance at %s>" % (self.this,)

class ConfusionMatrixPtr(ConfusionMatrix):
    def __init__(self,this):
        _swig_setattr(self, ConfusionMatrix, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, ConfusionMatrix, 'thisown', 0)
        _swig_setattr(self, ConfusionMatrix,self.__class__,ConfusionMatrix)
_om.ConfusionMatrix_swigregister(ConfusionMatrixPtr)

class OccurrencesFile(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, OccurrencesFile, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, OccurrencesFile, name)
    def __init__(self,*args):
        _swig_setattr(self, OccurrencesFile, 'this', apply(_om.new_OccurrencesFile,args))
        _swig_setattr(self, OccurrencesFile, 'thisown', 1)
    def __del__(self, destroy= _om.delete_OccurrencesFile):
        try:
            if self.thisown: destroy(self)
        except: pass
    def addOccurrences(*args): return apply(_om.OccurrencesFile_addOccurrences,args)
    def numOccurrences(*args): return apply(_om.OccurrencesFile_numOccurrences,args)
    def head(*args): return apply(_om.OccurrencesFile_head,args)
    def tail(*args): return apply(_om.OccurrencesFile_tail,args)
    def next(*args): return apply(_om.OccurrencesFile_next,args)
    def get(*args): return apply(_om.OccurrencesFile_get,args)
    def remove(*args): return apply(_om.OccurrencesFile_remove,args)
    def printOccurrences(*args): return apply(_om.OccurrencesFile_printOccurrences,args)
    def __repr__(self):
        return "<C OccurrencesFile instance at %s>" % (self.this,)

class OccurrencesFilePtr(OccurrencesFile):
    def __init__(self,this):
        _swig_setattr(self, OccurrencesFile, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, OccurrencesFile, 'thisown', 0)
        _swig_setattr(self, OccurrencesFile,self.__class__,OccurrencesFile)
_om.OccurrencesFile_swigregister(OccurrencesFilePtr)

class AlgParamMetadata(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, AlgParamMetadata, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, AlgParamMetadata, name)
    __swig_setmethods__["id"] = _om.AlgParamMetadata_id_set
    __swig_getmethods__["id"] = _om.AlgParamMetadata_id_get
    if _newclass:id = property(_om.AlgParamMetadata_id_get,_om.AlgParamMetadata_id_set)
    __swig_setmethods__["name"] = _om.AlgParamMetadata_name_set
    __swig_getmethods__["name"] = _om.AlgParamMetadata_name_get
    if _newclass:name = property(_om.AlgParamMetadata_name_get,_om.AlgParamMetadata_name_set)
    __swig_setmethods__["type"] = _om.AlgParamMetadata_type_set
    __swig_getmethods__["type"] = _om.AlgParamMetadata_type_get
    if _newclass:type = property(_om.AlgParamMetadata_type_get,_om.AlgParamMetadata_type_set)
    __swig_setmethods__["overview"] = _om.AlgParamMetadata_overview_set
    __swig_getmethods__["overview"] = _om.AlgParamMetadata_overview_get
    if _newclass:overview = property(_om.AlgParamMetadata_overview_get,_om.AlgParamMetadata_overview_set)
    __swig_setmethods__["description"] = _om.AlgParamMetadata_description_set
    __swig_getmethods__["description"] = _om.AlgParamMetadata_description_get
    if _newclass:description = property(_om.AlgParamMetadata_description_get,_om.AlgParamMetadata_description_set)
    __swig_setmethods__["has_min"] = _om.AlgParamMetadata_has_min_set
    __swig_getmethods__["has_min"] = _om.AlgParamMetadata_has_min_get
    if _newclass:has_min = property(_om.AlgParamMetadata_has_min_get,_om.AlgParamMetadata_has_min_set)
    __swig_setmethods__["min"] = _om.AlgParamMetadata_min_set
    __swig_getmethods__["min"] = _om.AlgParamMetadata_min_get
    if _newclass:min = property(_om.AlgParamMetadata_min_get,_om.AlgParamMetadata_min_set)
    __swig_setmethods__["has_max"] = _om.AlgParamMetadata_has_max_set
    __swig_getmethods__["has_max"] = _om.AlgParamMetadata_has_max_get
    if _newclass:has_max = property(_om.AlgParamMetadata_has_max_get,_om.AlgParamMetadata_has_max_set)
    __swig_setmethods__["max"] = _om.AlgParamMetadata_max_set
    __swig_getmethods__["max"] = _om.AlgParamMetadata_max_get
    if _newclass:max = property(_om.AlgParamMetadata_max_get,_om.AlgParamMetadata_max_set)
    __swig_setmethods__["typical"] = _om.AlgParamMetadata_typical_set
    __swig_getmethods__["typical"] = _om.AlgParamMetadata_typical_get
    if _newclass:typical = property(_om.AlgParamMetadata_typical_get,_om.AlgParamMetadata_typical_set)
    def __init__(self,*args):
        _swig_setattr(self, AlgParamMetadata, 'this', apply(_om.new_AlgParamMetadata,args))
        _swig_setattr(self, AlgParamMetadata, 'thisown', 1)
    def __del__(self, destroy= _om.delete_AlgParamMetadata):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C AlgParamMetadata instance at %s>" % (self.this,)

class AlgParamMetadataPtr(AlgParamMetadata):
    def __init__(self,this):
        _swig_setattr(self, AlgParamMetadata, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, AlgParamMetadata, 'thisown', 0)
        _swig_setattr(self, AlgParamMetadata,self.__class__,AlgParamMetadata)
_om.AlgParamMetadata_swigregister(AlgParamMetadataPtr)

class AlgMetadata(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, AlgMetadata, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, AlgMetadata, name)
    __swig_setmethods__["id"] = _om.AlgMetadata_id_set
    __swig_getmethods__["id"] = _om.AlgMetadata_id_get
    if _newclass:id = property(_om.AlgMetadata_id_get,_om.AlgMetadata_id_set)
    __swig_setmethods__["name"] = _om.AlgMetadata_name_set
    __swig_getmethods__["name"] = _om.AlgMetadata_name_get
    if _newclass:name = property(_om.AlgMetadata_name_get,_om.AlgMetadata_name_set)
    __swig_setmethods__["version"] = _om.AlgMetadata_version_set
    __swig_getmethods__["version"] = _om.AlgMetadata_version_get
    if _newclass:version = property(_om.AlgMetadata_version_get,_om.AlgMetadata_version_set)
    __swig_setmethods__["overview"] = _om.AlgMetadata_overview_set
    __swig_getmethods__["overview"] = _om.AlgMetadata_overview_get
    if _newclass:overview = property(_om.AlgMetadata_overview_get,_om.AlgMetadata_overview_set)
    __swig_setmethods__["description"] = _om.AlgMetadata_description_set
    __swig_getmethods__["description"] = _om.AlgMetadata_description_get
    if _newclass:description = property(_om.AlgMetadata_description_get,_om.AlgMetadata_description_set)
    __swig_setmethods__["author"] = _om.AlgMetadata_author_set
    __swig_getmethods__["author"] = _om.AlgMetadata_author_get
    if _newclass:author = property(_om.AlgMetadata_author_get,_om.AlgMetadata_author_set)
    __swig_setmethods__["biblio"] = _om.AlgMetadata_biblio_set
    __swig_getmethods__["biblio"] = _om.AlgMetadata_biblio_get
    if _newclass:biblio = property(_om.AlgMetadata_biblio_get,_om.AlgMetadata_biblio_set)
    __swig_setmethods__["code_author"] = _om.AlgMetadata_code_author_set
    __swig_getmethods__["code_author"] = _om.AlgMetadata_code_author_get
    if _newclass:code_author = property(_om.AlgMetadata_code_author_get,_om.AlgMetadata_code_author_set)
    __swig_setmethods__["contact"] = _om.AlgMetadata_contact_set
    __swig_getmethods__["contact"] = _om.AlgMetadata_contact_get
    if _newclass:contact = property(_om.AlgMetadata_contact_get,_om.AlgMetadata_contact_set)
    __swig_setmethods__["categorical"] = _om.AlgMetadata_categorical_set
    __swig_getmethods__["categorical"] = _om.AlgMetadata_categorical_get
    if _newclass:categorical = property(_om.AlgMetadata_categorical_get,_om.AlgMetadata_categorical_set)
    __swig_setmethods__["absence"] = _om.AlgMetadata_absence_set
    __swig_getmethods__["absence"] = _om.AlgMetadata_absence_get
    if _newclass:absence = property(_om.AlgMetadata_absence_get,_om.AlgMetadata_absence_set)
    __swig_setmethods__["nparam"] = _om.AlgMetadata_nparam_set
    __swig_getmethods__["nparam"] = _om.AlgMetadata_nparam_get
    if _newclass:nparam = property(_om.AlgMetadata_nparam_get,_om.AlgMetadata_nparam_set)
    __swig_setmethods__["param"] = _om.AlgMetadata_param_set
    __swig_getmethods__["param"] = _om.AlgMetadata_param_get
    if _newclass:param = property(_om.AlgMetadata_param_get,_om.AlgMetadata_param_set)
    def getParameterList(*args): return apply(_om.AlgMetadata_getParameterList,args)
    def __init__(self,*args):
        _swig_setattr(self, AlgMetadata, 'this', apply(_om.new_AlgMetadata,args))
        _swig_setattr(self, AlgMetadata, 'thisown', 1)
    def __del__(self, destroy= _om.delete_AlgMetadata):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C AlgMetadata instance at %s>" % (self.this,)

class AlgMetadataPtr(AlgMetadata):
    def __init__(self,this):
        _swig_setattr(self, AlgMetadata, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, AlgMetadata, 'thisown', 0)
        _swig_setattr(self, AlgMetadata,self.__class__,AlgMetadata)
_om.AlgMetadata_swigregister(AlgMetadataPtr)

class Environment(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Environment, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Environment, name)
    def __init__(self,*args):
        _swig_setattr(self, Environment, 'this', apply(_om.new_Environment,args))
        _swig_setattr(self, Environment, 'thisown', 1)
    def __del__(self, destroy= _om.delete_Environment):
        try:
            if self.thisown: destroy(self)
        except: pass
    def numLayers(*args): return apply(_om.Environment_numLayers,args)
    def changeLayers(*args): return apply(_om.Environment_changeLayers,args)
    def changeMask(*args): return apply(_om.Environment_changeMask,args)
    def varTypes(*args): return apply(_om.Environment_varTypes,args)
    def normalize(*args): return apply(_om.Environment_normalize,args)
    def copyNormalizationParams(*args): return apply(_om.Environment_copyNormalizationParams,args)
    def get(*args): return apply(_om.Environment_get,args)
    def getRandom(*args): return apply(_om.Environment_getRandom,args)
    def check(*args): return apply(_om.Environment_check,args)
    def getRegion(*args): return apply(_om.Environment_getRegion,args)
    def getExtremes(*args): return apply(_om.Environment_getExtremes,args)
    def getCoordinateSystem(*args): return apply(_om.Environment_getCoordinateSystem,args)
    def getLayer(*args): return apply(_om.Environment_getLayer,args)
    def getMask(*args): return apply(_om.Environment_getMask,args)
    def getLayerFilename(*args): return apply(_om.Environment_getLayerFilename,args)
    def getMaskFilename(*args): return apply(_om.Environment_getMaskFilename,args)
    def __repr__(self):
        return "<C Environment instance at %s>" % (self.this,)

class EnvironmentPtr(Environment):
    def __init__(self,this):
        _swig_setattr(self, Environment, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Environment, 'thisown', 0)
        _swig_setattr(self, Environment,self.__class__,Environment)
_om.Environment_swigregister(EnvironmentPtr)

class OpenModeller(Serializable):
    __swig_setmethods__ = {}
    for _s in [Serializable]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, OpenModeller, name, value)
    __swig_getmethods__ = {}
    for _s in [Serializable]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, OpenModeller, name)
    def __init__(self,*args):
        _swig_setattr(self, OpenModeller, 'this', apply(_om.new_OpenModeller,args))
        _swig_setattr(self, OpenModeller, 'thisown', 1)
    def __del__(self, destroy= _om.delete_OpenModeller):
        try:
            if self.thisown: destroy(self)
        except: pass
    def setLogLevel(*args): return apply(_om.OpenModeller_setLogLevel,args)
    def getVersion(*args): return apply(_om.OpenModeller_getVersion,args)
    def getConfigFileName(*args): return apply(_om.OpenModeller_getConfigFileName,args)
    def getPluginPath(*args): return apply(_om.OpenModeller_getPluginPath,args)
    def setPluginPath(*args): return apply(_om.OpenModeller_setPluginPath,args)
    def resetPluginPath(*args): return apply(_om.OpenModeller_resetPluginPath,args)
    def loadAlgorithms(*args): return apply(_om.OpenModeller_loadAlgorithms,args)
    def availableAlgorithms(*args): return apply(_om.OpenModeller_availableAlgorithms,args)
    def algorithmMetadata(*args): return apply(_om.OpenModeller_algorithmMetadata,args)
    def numAvailableAlgorithms(*args): return apply(_om.OpenModeller_numAvailableAlgorithms,args)
    def getEnvironment(*args): return apply(_om.OpenModeller_getEnvironment,args)
    def getAlgorithm(*args): return apply(_om.OpenModeller_getAlgorithm,args)
    def setOccurrences(*args): return apply(_om.OpenModeller_setOccurrences,args)
    def setAlgorithm(*args): return apply(_om.OpenModeller_setAlgorithm,args)
    def setEnvironment(*args): return apply(_om.OpenModeller_setEnvironment,args)
    def setProjection(*args): return apply(_om.OpenModeller_setProjection,args)
    def setOutputMapByFile(*args): return apply(_om.OpenModeller_setOutputMapByFile,args)
    def setOutputMapByFormat(*args): return apply(_om.OpenModeller_setOutputMapByFormat,args)
    def setModelCallback(*args): return apply(_om.OpenModeller_setModelCallback,args)
    def setModelCommand(*args): return apply(_om.OpenModeller_setModelCommand,args)
    def setMapCallback(*args): return apply(_om.OpenModeller_setMapCallback,args)
    def setMapCommand(*args): return apply(_om.OpenModeller_setMapCommand,args)
    def createModel(*args): return apply(_om.OpenModeller_createModel,args)
    def run(*args): return apply(_om.OpenModeller_run,args)
    def createMapNative(*args): return apply(_om.OpenModeller_createMapNative,args)
    def createMapProj(*args): return apply(_om.OpenModeller_createMapProj,args)
    def getValue(*args): return apply(_om.OpenModeller_getValue,args)
    def error(*args): return apply(_om.OpenModeller_error,args)
    def getActualAreaStats(*args): return apply(_om.OpenModeller_getActualAreaStats,args)
    def getEstimatedAreaStats(*args): return apply(_om.OpenModeller_getEstimatedAreaStats,args)
    def serialize(*args): return apply(_om.OpenModeller_serialize,args)
    def deserialize(*args): return apply(_om.OpenModeller_deserialize,args)
    def __repr__(self):
        return "<C OpenModeller instance at %s>" % (self.this,)

class OpenModellerPtr(OpenModeller):
    def __init__(self,this):
        _swig_setattr(self, OpenModeller, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, OpenModeller, 'thisown', 0)
        _swig_setattr(self, OpenModeller,self.__class__,OpenModeller)
_om.OpenModeller_swigregister(OpenModellerPtr)


