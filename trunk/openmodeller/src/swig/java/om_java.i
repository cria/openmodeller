%module om

%{
#include "../../inc/om_defs.hh"
#include "../../inc/om_control.hh"
#include "../../inc/environment.hh"
#include "../../inc/map_format.hh"
#include "../../inc/om_alg_parameter.hh"
#include "../../inc/om_algorithm_metadata.hh"
#include "../../inc/file_parser.hh"
#include "../../inc/om_serializable.hh"
#include "../../console/occurrences_file.hh"
%}


%include "../../inc/om_defs.hh"
%include "../../inc/om_serializable.hh"
%include "../../inc/om_control.hh"
%include "../../inc/environment.hh"
%include "../../inc/map_format.hh"
%include "../../inc/om_alg_parameter.hh"
%include "../../inc/om_algorithm_metadata.hh"
%include "../../inc/file_parser.hh"
%include "../../console/occurrences_file.hh"

%include "arrays_java.i"

//JAVA_ARRAYSOFCLASSES(AlgMetadata)
//%typemap(out) AlgMetadata** SWIGTYPE_p_p_AlgMetadata;

%inline %{
AlgMetadata ** NextAlgMetadata(AlgMetadata ** p) {
  return (AlgMetadata **) ((*p) + 1);
}
%}
