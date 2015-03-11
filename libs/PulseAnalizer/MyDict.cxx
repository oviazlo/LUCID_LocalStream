// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME MyDict

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Since CINT ignores the std namespace, we need to do so in this file.
namespace std {} using namespace std;

// Header files passed as explicit arguments
#include "include/PulseAnalizer.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static void delete_PulseAnalizer(void *p);
   static void deleteArray_PulseAnalizer(void *p);
   static void destruct_PulseAnalizer(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::PulseAnalizer*)
   {
      ::PulseAnalizer *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::PulseAnalizer >(0);
      static ::ROOT::TGenericClassInfo 
         instance("PulseAnalizer", ::PulseAnalizer::Class_Version(), "include/PulseAnalizer.h", 16,
                  typeid(::PulseAnalizer), DefineBehavior(ptr, ptr),
                  &::PulseAnalizer::Dictionary, isa_proxy, 4,
                  sizeof(::PulseAnalizer) );
      instance.SetDelete(&delete_PulseAnalizer);
      instance.SetDeleteArray(&deleteArray_PulseAnalizer);
      instance.SetDestructor(&destruct_PulseAnalizer);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::PulseAnalizer*)
   {
      return GenerateInitInstanceLocal((::PulseAnalizer*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::PulseAnalizer*)0x0); R__UseDummy(_R__UNIQUE_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr PulseAnalizer::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *PulseAnalizer::Class_Name()
{
   return "PulseAnalizer";
}

//______________________________________________________________________________
const char *PulseAnalizer::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PulseAnalizer*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int PulseAnalizer::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PulseAnalizer*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *PulseAnalizer::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PulseAnalizer*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *PulseAnalizer::Class()
{
   if (!fgIsA) { R__LOCKGUARD2(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PulseAnalizer*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void PulseAnalizer::Streamer(TBuffer &R__b)
{
   // Stream an object of class PulseAnalizer.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(PulseAnalizer::Class(),this);
   } else {
      R__b.WriteClassBuffer(PulseAnalizer::Class(),this);
   }
}

namespace ROOT {
   // Wrapper around operator delete
   static void delete_PulseAnalizer(void *p) {
      delete ((::PulseAnalizer*)p);
   }
   static void deleteArray_PulseAnalizer(void *p) {
      delete [] ((::PulseAnalizer*)p);
   }
   static void destruct_PulseAnalizer(void *p) {
      typedef ::PulseAnalizer current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::PulseAnalizer

namespace ROOT {
   static TClass *vectorlEdoublegR_Dictionary();
   static void vectorlEdoublegR_TClassManip(TClass*);
   static void *new_vectorlEdoublegR(void *p = 0);
   static void *newArray_vectorlEdoublegR(Long_t size, void *p);
   static void delete_vectorlEdoublegR(void *p);
   static void deleteArray_vectorlEdoublegR(void *p);
   static void destruct_vectorlEdoublegR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<double>*)
   {
      vector<double> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<double>),0);
      static ::ROOT::TGenericClassInfo 
         instance("vector<double>", -2, "vector", 214,
                  typeid(vector<double>), DefineBehavior(ptr, ptr),
                  &vectorlEdoublegR_Dictionary, isa_proxy, 0,
                  sizeof(vector<double>) );
      instance.SetNew(&new_vectorlEdoublegR);
      instance.SetNewArray(&newArray_vectorlEdoublegR);
      instance.SetDelete(&delete_vectorlEdoublegR);
      instance.SetDeleteArray(&deleteArray_vectorlEdoublegR);
      instance.SetDestructor(&destruct_vectorlEdoublegR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<double> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<double>*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEdoublegR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<double>*)0x0)->GetClass();
      vectorlEdoublegR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEdoublegR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEdoublegR(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<double> : new vector<double>;
   }
   static void *newArray_vectorlEdoublegR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<double>[nElements] : new vector<double>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEdoublegR(void *p) {
      delete ((vector<double>*)p);
   }
   static void deleteArray_vectorlEdoublegR(void *p) {
      delete [] ((vector<double>*)p);
   }
   static void destruct_vectorlEdoublegR(void *p) {
      typedef vector<double> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<double>

namespace {
  void TriggerDictionaryInitialization_MyDict_Impl() {
    static const char* headers[] = {
"include/PulseAnalizer.h",
0
    };
    static const char* includePaths[] = {
"/usr/include/root",
"/home/oviazlo/PhD_study/HW_project/LUCID/signalShapeAnalizer/libs/PulseAnalizer/",
0
    };
    static const char* fwdDeclCode = 
R"DICTFWDDCLS(
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
class __attribute__((annotate("$clingAutoload$include/PulseAnalizer.h")))  PulseAnalizer;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "include/PulseAnalizer.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"PulseAnalizer", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("MyDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_MyDict_Impl, {}, classesHeaders);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_MyDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_MyDict() {
  TriggerDictionaryInitialization_MyDict_Impl();
}
