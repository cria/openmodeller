%module om

%include "../../inc/om_defs.hh"

#undef dllexp
#undef dll_log

#define dllexp
#define dll_log

%{
#include "../../inc/om_defs.hh"
#include "../../inc/om_control.hh"
#include "../../inc/environment.hh"
#include "../../inc/map_format.hh"
#include "../../inc/om_algorithm_metadata.hh"
#include "../../inc/om_alg_parameter.hh"
#include "../../inc/om_serializable.hh"
#include "../../inc/om_area_stats.hh"
#include "../../inc/om_conf_matrix.hh"
#include "../../console/occurrences_file.hh"
%}

%inline %{
void cp(char * msg) { printf("cp: %s\n", msg); fflush(stdout); }
%}

/* This tells SWIG to treat char ** as a special case when used as a parameter in a function call */
%typemap(in) char ** (jint size) {
    if ($input){ 
        size = jenv->GetArrayLength($input);
        $1 = (char **) malloc((size+1)*sizeof(char *));
        /* make a copy of each string */
	int i;
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
    else {
    	$1 = 0; 
    }
}

/* This cleans up the memory we malloc'd before the function call */
%typemap(freearg) char ** {
    if ($1) {
        int i;
        for (i=0; i<size$argnum-1; i++)
            free($1[i]);
        free($1);
    }
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

jclass JNI_getClass(JNIEnv * jenv, char * className) {
    jclass clazz = jenv->FindClass(className);
    if (clazz == 0) { 
	JNIException("System could not find specified Java class.\n");
	return 0;
    }
    else return clazz;
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

jmethodID JNI_getSetIntMethod(JNIEnv * jenv, jclass clazz, char * methodName){
    jmethodID mID = jenv->GetMethodID(clazz, methodName, "(I)V");
    if (mID == 0) { 
	JNIException("System could not find method.\n"); 
        return 0;
    }
    else return mID;
}

jmethodID JNI_getSetDoubleMethod(JNIEnv * jenv, jclass clazz, char * methodName){
    jmethodID mID = jenv->GetMethodID(clazz, methodName, "(D)V");
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

    const jclass clazz = JNI_getClass(jenv, "br/org/cria/OpenModeller/AlgMetadata");
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
    jsize size = jenv->GetArrayLength($input);
    $1 = (AlgParameter*) malloc(size*sizeof(AlgParameter));
    memset($1, 0, size*sizeof(AlgParameter));
    /* make a copy of each parameter */
    for (i = 0; i<size; i++) {
        jobjectArray array = (jobjectArray) jenv->GetObjectArrayElement($input, i);
	jstring id    = (jstring) jenv->GetObjectArrayElement(array, 0);
	jstring value = (jstring) jenv->GetObjectArrayElement(array, 1);
	char * cid    = (char *) jenv->GetStringUTFChars(id, NULL);
	char * cvalue = (char *) jenv->GetStringUTFChars(value, NULL);
        $1[i].setId(cid);
	$1[i].setValue(cvalue);
	jenv->ReleaseStringUTFChars(id, cid);
	jenv->ReleaseStringUTFChars(value, cvalue);
    }
}

/* This cleans up the memory we malloc'd before the function call */
%typemap(freearg) AlgParameter * {
    free((char *) $1);
}

/* These 3 typemaps tell SWIG what JNI and Java types to use */
%typemap(jni) AlgParameter * "jobjectArray"
%typemap(jtype) AlgParameter * "String[][]"
%typemap(jstype) AlgParameter * "String[][]"
%typemap(javain)  AlgParameter * "$javainput"


%typemap(out) AlgParamMetadata ** {

    // return an array of AlgParam back to caller
    int i, len = 0;
    while ((*$1)[len].id) len++;    
    jclass clazz = JNI_getClass(jenv, "br/org/cria/OpenModeller/AlgParamMetadata");

    // get method ids for later use
    jmethodID constrID = JNI_getConstructor(jenv, clazz);

    jmethodID mID_setId          = JNI_getSetStringMethod(jenv, clazz, "setId");
    jmethodID mID_setName        = JNI_getSetStringMethod(jenv, clazz, "setName");
    jmethodID mID_setType        = JNI_getSetStringMethod(jenv, clazz, "setType");
    jmethodID mID_setOverview    = JNI_getSetStringMethod(jenv, clazz, "setOverview");
    jmethodID mID_setDescription = JNI_getSetStringMethod(jenv, clazz, "setDescription");
    jmethodID mID_setTypical     = JNI_getSetStringMethod(jenv, clazz, "setTypical");
    jmethodID mID_setHas_min     = JNI_getSetIntMethod(jenv, clazz, "setHas_min");
    jmethodID mID_setHas_max     = JNI_getSetIntMethod(jenv, clazz, "setHas_min");
    jmethodID mID_setMin         = JNI_getSetDoubleMethod(jenv, clazz, "setMin");
    jmethodID mID_setMax         = JNI_getSetDoubleMethod(jenv, clazz, "setMax");

    jresult = (jobjectArray)jenv->NewObjectArray(len, clazz, 0);
    if (jresult == 0) { JNIException("Could not allocate array of AlgParamMetadata.\n"); return 0; }

    for (i = 0; i < len; i++)
    {
	AlgParamMetadata * currParam = (*result + i);

	// create new AlgParameter object
        jobject algParam = jenv->NewObject(clazz, constrID);

        // set data members 
        jstring sid = jenv->NewStringUTF(currParam->id);
        jenv->CallVoidMethod(algParam, mID_setId, sid);

        jstring sname = jenv->NewStringUTF(currParam->name);
        jenv->CallVoidMethod(algParam, mID_setName, sname);

        jstring stype = jenv->NewStringUTF(currParam->type);
        jenv->CallVoidMethod(algParam, mID_setType, stype);

        jstring soverview = jenv->NewStringUTF(currParam->overview);
        jenv->CallVoidMethod(algParam, mID_setOverview, soverview);

        jstring sdescription = jenv->NewStringUTF(currParam->description);
        jenv->CallVoidMethod(algParam, mID_setDescription, sdescription);

        jstring stypical = jenv->NewStringUTF(currParam->typical);
        jenv->CallVoidMethod(algParam, mID_setTypical, stypical);

        jenv->CallVoidMethod(algParam, mID_setHas_min, currParam->has_min);
        jenv->CallVoidMethod(algParam, mID_setHas_max, currParam->has_max);
        jenv->CallVoidMethod(algParam, mID_setMin, currParam->min);
        jenv->CallVoidMethod(algParam, mID_setMax, currParam->max);

        // add it to main array
        jenv->SetObjectArrayElement(jresult, i, algParam);

        jenv->DeleteLocalRef(sid);
        jenv->DeleteLocalRef(sname);
        jenv->DeleteLocalRef(stype);
        jenv->DeleteLocalRef(soverview);
        jenv->DeleteLocalRef(sdescription);
        jenv->DeleteLocalRef(stypical);
    }
}

/* These 3 typemaps tell SWIG what JNI and Java types to use */
%typemap(jni) AlgParamMetadata ** "jobjectArray"
%typemap(jtype) AlgParamMetadata ** "AlgParamMetadata[]"
%typemap(jstype) AlgParamMetadata ** "AlgParamMetadata[]"
%typemap(javaout) AlgParamMetadata ** { 
	return $jnicall; 
}

%inline %{

AlgParamMetadata ** getParameterList(AlgMetadata * metadata)
{
    int n = metadata->nparam;
    AlgParamMetadata ** arrayCopy = new AlgParamMetadata*;
    *arrayCopy = new AlgParamMetadata[n + 1];
    memcpy((void *) *arrayCopy, metadata->param, n * sizeof(AlgParamMetadata));
    memset((void *) &((*arrayCopy)[n]), 0, sizeof(AlgParamMetadata));
    return arrayCopy;
}

%}

%include "../../inc/om_log.hh"
%include "../../inc/om_serializable.hh"
%include "../../inc/om_control.hh"
%include "../../inc/environment.hh"
%include "../../inc/map_format.hh"
%include "../../inc/om_algorithm_metadata.hh"
%include "../../inc/om_area_stats.hh"
%include "../../inc/om_conf_matrix.hh"
%include "../../console/occurrences_file.hh"

