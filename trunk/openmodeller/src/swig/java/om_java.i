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




// ==========================
//  JNI Helper Functions
// ==========================


%inline %{

void JNIException(char * msg){
  fprintf(stderr, msg);
}

jmethodID JNI_getConstructor(JNIEnv * jenv, jclass clazz){
    jmethodID mID = jenv->GetMethodID(clazz, "<init>", "()V");
    if (mID == 0) { 
	JNIException("System could not find constructor for class.\n"); 
	return 0;
    }
    else return mID;
}

jmethodID JNI_getSetStringMethod(JNIEnv * jenv, jclass clazz, char * methodName){
    jmethodID mID = jenv->GetMethodID(clazz, methodName, "(Ljava/lang/String;)V");
    if (mID == 0) { 
	JNIException("System could not find method.\n"); 
        return 0;
    }
    else return mID;
}

%}

// ===========================
//  AlgMetadata typemaps
// ===========================

/* This allows a C function to return a AlgMetadata** as a Java array */
%typemap(out) AlgMetadata ** {
    int i;
    int len=0;
    jobject algMd;

    const jclass clazz = jenv->FindClass("AlgMetadata");
    if (clazz == 0) { JNIException("System could not find class AlgMetadata\n"); }

    jmethodID constrID = JNI_getConstructor(jenv, clazz);

    jmethodID mID_setId          = JNI_getSetStringMethod(jenv, clazz, "setId");
    jmethodID mID_setName        = JNI_getSetStringMethod(jenv, clazz, "setName");
    jmethodID mID_setVersion     = JNI_getSetStringMethod(jenv, clazz, "setVersion");
    jmethodID mID_setOverview    = JNI_getSetStringMethod(jenv, clazz, "setOverview");
    jmethodID mID_setDescription = JNI_getSetStringMethod(jenv, clazz, "setDescription");
    jmethodID mID_setAuthor      = JNI_getSetStringMethod(jenv, clazz, "setAuthor");
    jmethodID mID_setBiblio      = JNI_getSetStringMethod(jenv, clazz, "setBiblio");
    jmethodID mID_setCodeAuthor  = JNI_getSetStringMethod(jenv, clazz, "setCode_author");
    jmethodID mID_setContact     = JNI_getSetStringMethod(jenv, clazz, "setContact");

	// todo: add integer members

    while ($1[len]) len++;    
    jresult = (jobjectArray) jenv->NewObjectArray(len, clazz, NULL);
    if (jresult == 0) { JNIException("System could not allocate array of objects"); }

    for (i=0; i<len; i++) {
      algMd = jenv->NewObject(clazz, constrID);

      jstring sid = jenv->NewStringUTF((*(result + i))->id);
      jenv->CallVoidMethod(algMd, mID_setId, sid);

      jstring sname = jenv->NewStringUTF((*(result + i))->name);
      jenv->CallVoidMethod(algMd, mID_setName, sname);

      jstring sversion = jenv->NewStringUTF((*(result + i))->version);
      jenv->CallVoidMethod(algMd, mID_setVersion, sversion);

      jstring soverview = jenv->NewStringUTF((*(result + i))->overview);
      jenv->CallVoidMethod(algMd, mID_setOverview, soverview);

      jstring sdescription = jenv->NewStringUTF((*(result + i))->description);
      jenv->CallVoidMethod(algMd, mID_setDescription, sdescription);

      jstring sauthor = jenv->NewStringUTF((*(result + i))->author);
      jenv->CallVoidMethod(algMd, mID_setAuthor, sauthor);

      jstring sbiblio = jenv->NewStringUTF((*(result + i))->biblio);
      jenv->CallVoidMethod(algMd, mID_setBiblio, sbiblio);

      jstring scode_author = jenv->NewStringUTF((*(result + i))->code_author);
      jenv->CallVoidMethod(algMd, mID_setCodeAuthor, scode_author);

      jstring scontact = jenv->NewStringUTF((*(result + i))->contact);
      jenv->CallVoidMethod(algMd, mID_setContact, scontact);

	// todo: add integer members

      jenv->SetObjectArrayElement(jresult, i, algMd);

      jenv->DeleteLocalRef(sid);
      jenv->DeleteLocalRef(sname);
      jenv->DeleteLocalRef(sversion);
      jenv->DeleteLocalRef(soverview);
      jenv->DeleteLocalRef(sdescription);
      jenv->DeleteLocalRef(sauthor);
      jenv->DeleteLocalRef(sbiblio);
      jenv->DeleteLocalRef(scode_author);
      jenv->DeleteLocalRef(scontact);
      jenv->DeleteLocalRef(algMd);
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


// ===============================
//  AlgParameter typemaps
// ===============================

/* This tells SWIG to treat AlgParameter * as a special case when used as a parameter in a function call */
%typemap(in) AlgParameter * {

    // expect a Java array of String[2] as input
    // first string is the parameter id and the second is its value
    int i = 0;

    // get length of array
    size = jenv->GetArrayLength($input);
    $1 = (AlgParameter*) malloc((size+1)*sizeof(AlgParameter));
    /* make a copy of each parameter */
    for (i = 0; i<size; i++) {
        jobject obj = jenv->GetObjectArrayElement($input, i);

	jstring key   = (jstring) jenv->GetObjectArrayElement(obj, 0);
	jstring value = (jstring) jenv->GetObjectArrayElement(obj, 1);

	char * ckey = jenv->

	// turn hash into vector of AlgParameter objects
        $1[i].setId();
	$1[i].setValue(...);
    }
    $1[i] = 0;
}

/* This cleans up the memory we malloc'd before the function call */
%typemap(freearg) AlgParameter * {
    free((char *) $1);
}

/* These 3 typemaps tell SWIG what JNI and Java types to use */
%typemap(jni) AlgParameter * "jobjectArray"
%typemap(jtype) AlgParameter * "Hashtable"
%typemap(jstype) AlgParameter * "Hashtable"

/* This typemap handles the conversion of the jtype to jstype typemap type and visa versa */
%typemap(javain)  AlgParameter * "$javainput"



// ====== Returns a java object with list of parameters =========

%inline %{

PyObject * getParameterList(AlgMetadata * metadata)
{
    int i;
    PyObject * paramMetadata;
    PyObject * list = PyList_New(0);
    for (i = 0; i < metadata->nparam; i++)
    {
	paramMetadata = SWIG_NewPointerObj((void *) &(metadata->param[i]), 
                                           SWIGTYPE_p_AlgParamMetadata, 1);
        PyList_Append(list, paramMetadata);
    }

    return list;
}

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

