//MEMO: generate C++ and Java files with following command
//swig -java -c++ -package com.myskng.taglib -outdir /path/to/project/root/app/src/main/java/com/myskng/taglib -o swigout/swig_taglib_wrap.cpp taglib.i

%module taglib

%{
#include "tag.h"
#include "fileref.h"
#include "tpropertymap.h"
#include "tlist.h"
using namespace TagLib;
%}

// workaround for macros
#define TAGLIB_EXPORT  
#define TAGLIB_IGNORE_MISSING_DESTRUCTOR  

typedef const char *FileName;

// typemaps
%typemap(jstype) String "String"
%typemap(jni) String "jstring"
%typemap(jtype) String "String"
%typemap(javaout) String "{ return $jnicall; }"
%typemap(javain) String "$javainput"
%typemap(out) String {
    $result = jenv->NewStringUTF($1.toCString(true));
}
%typemap(in) String{
    $1 = new String(jenv->GetStringUTFChars($input, 0));
}

%typemap(jstype) String & "String"
%typemap(jni) String & "jstring"
%typemap(jtype) String & "String"
%typemap(javaout) String & "{ return $jnicall; }"
%typemap(javain) String & "$javainput"
%typemap(out) String & {
    $result = jenv->NewStringUTF($1->toCString(true));
}
%typemap(in) String & {
    $1 = new String(jenv->GetStringUTFChars($input, 0));
}

%typemap(jstype) std::vector<unsigned char> "byte[]"
%typemap(jni) std::vector<unsigned char> "jbyteArray"
%typemap(jtype) std::vector<unsigned char> "byte[]"
%typemap(javaout) std::vector<unsigned char> "{ return $jnicall; }"
%typemap(javain) std::vector<unsigned char> "$javainput"
%typemap(out) std::vector<unsigned char> {
    $result = jenv->NewByteArray($1.size());
    jenv->SetByteArrayRegion($result, 0, $1.size(), (jbyte *) $1.data());
}

// operator rename
%rename(get) operator[];
%rename(equals) operator==;

// list templates
%include "../toolkit/tlist.h"
%include "../toolkit/tlist.tcc"
%template(listString) TagLib::List<String>;

// include header file
%include "../toolkit/tstringlist.h"
%include "../toolkit/tpropertymap.h"
%include "../tag.h"
%include "../fileref.h"
%include "../audioproperties.h"