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


/* This tells SWIG to treat char ** as a special case when used as a parameter in a function call */
%typemap(in) char ** (jint size) {
    int i = 0;
    size = jenv->GetArrayLength($input);
    $1 = (char **) malloc((size+1)*sizeof(char *));
    /* make a copy of each string */
    for (i = 0; i<size; i++) {
        jstring j_string = (jstring)jenv->GetObjectArrayElement($input, i);
        const char * c_string = jenv->GetStringUTFChars(j_string, 0);
        $1[i] = (char *) malloc((strlen(c_string)+1)*sizeof(const char *));
        strcpy($1[i], c_string);
        jenv->ReleaseStringUTFChars(j_string, c_string);
        jenv->DeleteLocalRef(j_string);
    }
    $1[i] = 0;
}

/* This cleans up the memory we malloc'd before the function call */
%typemap(freearg) char ** {
    int i;
    for (i=0; i<size$argnum-1; i++)
      free($1[i]);
    free($1);
}

/* This allows a C function to return a char ** as a Java String array */
%typemap(out) char ** {
    int i;
    int len=0;
    jstring temp_string;
    const jclass clazz = jenv->FindClass("java/lang/String");

    while ($1[len]) len++;    
    jresult = (jobjectArray) jenv->NewObjectArray(len, clazz, NULL);
    /* exception checking omitted */

    for (i=0; i<len; i++) {
      temp_string = jenv->NewStringUTF(*result++);
      jenv->SetObjectArrayElement(jresult, i, temp_string);
      jenv->DeleteLocalRef(temp_string);
    }
}

/* These 3 typemaps tell SWIG what JNI and Java types to use */
%typemap(jni) char ** "jobjectArray"
%typemap(jtype) char ** "String[]"
%typemap(jstype) char ** "String[]"

/* These 2 typemaps handle the conversion of the jtype to jstype typemap type and visa versa */
%typemap(javain) char ** "$javainput"
%typemap(javaout) char ** {
    return $jnicall;
  }

/* Now a few test functions */
%inline %{

int print_args(char **argv) {
    int i = 0;
    while (argv[i]) {
         printf("argv[%d] = %s\n", i, argv[i]);
         i++;
    }
    return i;
}

char **get_args() {
  static char *values[] = { "Dave", "Mike", "Susan", "John", "Michelle", 0};
  return &values[0];
}

%}





/* This allows a C function to return a AlgMetadata** as a Java array */
%typemap(out) AlgMetadata ** {
    int i;
    int len=0;
    jobject temp_obj;
    jstring temp_string;
    const jclass clazz = jenv->FindClass("AlgMetadata");
    if (clazz == 0) {
	fprintf(stderr, "System could not find class AlgMetadata\n");
	return 0;
    }

    jmethodID constrID = jenv->GetMethodID(clazz, "<init>", "()V");

    if (constrID == 0) {
      fprintf(stderr, "System could not find AlgMetadata constructor.\n");
      return 0;
    }

    jmethodID methodID = jenv->GetMethodID(clazz, "setId", 
                                          "(Ljava/lang/String;)V");

    if (methodID == 0) {
      fprintf(stderr, "System could not find method AlgMetadata::setId()\n");
      return 0;
    }

    while ($1[len]) len++;    
    jresult = (jobjectArray) jenv->NewObjectArray(len, clazz, NULL);
    /* exception checking omitted */

    for (i=0; i<len; i++) {

      char * cstr = (*(result + i))->id;
      printf("Id[%d]=%s\n", i, cstr); fflush(stdout);

      temp_string = jenv->NewStringUTF(cstr);
      temp_obj = jenv->NewObject(clazz, constrID);
      jenv->CallVoidMethod(temp_obj, methodID, temp_string);
      jenv->SetObjectArrayElement(jresult, i, temp_obj);
      jenv->DeleteLocalRef(temp_obj);
      jenv->DeleteLocalRef(temp_string);
    }
}

/* These 3 typemaps tell SWIG what JNI and Java types to use */
%typemap(jni) AlgMetadata ** "jobjectArray"
%typemap(jtype) AlgMetadata ** "AlgMetadata[]"
%typemap(jstype) AlgMetadata ** "AlgMetadata[]"

/* These 2 typemaps handle the conversion of the jtype to jstype typemap type and visa versa */
%typemap(javaout) AlgMetadata ** {
    return $jnicall;
  }


%include "../../inc/om_defs.hh"
%include "../../inc/om_serializable.hh"
%include "../../inc/om_control.hh"
%include "../../inc/environment.hh"
%include "../../inc/map_format.hh"
%include "../../inc/om_alg_parameter.hh"
%include "../../inc/om_algorithm_metadata.hh"
%include "../../inc/file_parser.hh"
%include "../../console/occurrences_file.hh"

