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
#include "include/Bi207Event.h"
#include "include/FadcEvent.h"
#include "include/FadcRead.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static void *new_FadcEvent(void *p = 0);
   static void *newArray_FadcEvent(Long_t size, void *p);
   static void delete_FadcEvent(void *p);
   static void deleteArray_FadcEvent(void *p);
   static void destruct_FadcEvent(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::FadcEvent*)
   {
      ::FadcEvent *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::FadcEvent >(0);
      static ::ROOT::TGenericClassInfo 
         instance("FadcEvent", ::FadcEvent::Class_Version(), "include/FadcEvent.h", 17,
                  typeid(::FadcEvent), DefineBehavior(ptr, ptr),
                  &::FadcEvent::Dictionary, isa_proxy, 4,
                  sizeof(::FadcEvent) );
      instance.SetNew(&new_FadcEvent);
      instance.SetNewArray(&newArray_FadcEvent);
      instance.SetDelete(&delete_FadcEvent);
      instance.SetDeleteArray(&deleteArray_FadcEvent);
      instance.SetDestructor(&destruct_FadcEvent);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::FadcEvent*)
   {
      return GenerateInitInstanceLocal((::FadcEvent*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::FadcEvent*)0x0); R__UseDummy(_R__UNIQUE_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_Bi207Event(void *p = 0);
   static void *newArray_Bi207Event(Long_t size, void *p);
   static void delete_Bi207Event(void *p);
   static void deleteArray_Bi207Event(void *p);
   static void destruct_Bi207Event(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::Bi207Event*)
   {
      ::Bi207Event *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::Bi207Event >(0);
      static ::ROOT::TGenericClassInfo 
         instance("Bi207Event", ::Bi207Event::Class_Version(), "include/Bi207Event.h", 17,
                  typeid(::Bi207Event), DefineBehavior(ptr, ptr),
                  &::Bi207Event::Dictionary, isa_proxy, 4,
                  sizeof(::Bi207Event) );
      instance.SetNew(&new_Bi207Event);
      instance.SetNewArray(&newArray_Bi207Event);
      instance.SetDelete(&delete_Bi207Event);
      instance.SetDeleteArray(&deleteArray_Bi207Event);
      instance.SetDestructor(&destruct_Bi207Event);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::Bi207Event*)
   {
      return GenerateInitInstanceLocal((::Bi207Event*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::Bi207Event*)0x0); R__UseDummy(_R__UNIQUE_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_FadcRead(void *p = 0);
   static void *newArray_FadcRead(Long_t size, void *p);
   static void delete_FadcRead(void *p);
   static void deleteArray_FadcRead(void *p);
   static void destruct_FadcRead(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::FadcRead*)
   {
      ::FadcRead *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::FadcRead >(0);
      static ::ROOT::TGenericClassInfo 
         instance("FadcRead", ::FadcRead::Class_Version(), "include/FadcRead.h", 24,
                  typeid(::FadcRead), DefineBehavior(ptr, ptr),
                  &::FadcRead::Dictionary, isa_proxy, 4,
                  sizeof(::FadcRead) );
      instance.SetNew(&new_FadcRead);
      instance.SetNewArray(&newArray_FadcRead);
      instance.SetDelete(&delete_FadcRead);
      instance.SetDeleteArray(&deleteArray_FadcRead);
      instance.SetDestructor(&destruct_FadcRead);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::FadcRead*)
   {
      return GenerateInitInstanceLocal((::FadcRead*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::FadcRead*)0x0); R__UseDummy(_R__UNIQUE_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr FadcEvent::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *FadcEvent::Class_Name()
{
   return "FadcEvent";
}

//______________________________________________________________________________
const char *FadcEvent::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::FadcEvent*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int FadcEvent::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::FadcEvent*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *FadcEvent::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::FadcEvent*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *FadcEvent::Class()
{
   if (!fgIsA) { R__LOCKGUARD2(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::FadcEvent*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr Bi207Event::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *Bi207Event::Class_Name()
{
   return "Bi207Event";
}

//______________________________________________________________________________
const char *Bi207Event::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::Bi207Event*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int Bi207Event::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::Bi207Event*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *Bi207Event::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::Bi207Event*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *Bi207Event::Class()
{
   if (!fgIsA) { R__LOCKGUARD2(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::Bi207Event*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr FadcRead::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *FadcRead::Class_Name()
{
   return "FadcRead";
}

//______________________________________________________________________________
const char *FadcRead::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::FadcRead*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int FadcRead::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::FadcRead*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *FadcRead::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::FadcRead*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *FadcRead::Class()
{
   if (!fgIsA) { R__LOCKGUARD2(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::FadcRead*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void FadcEvent::Streamer(TBuffer &R__b)
{
   // Stream an object of class FadcEvent.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(FadcEvent::Class(),this);
   } else {
      R__b.WriteClassBuffer(FadcEvent::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_FadcEvent(void *p) {
      return  p ? new(p) ::FadcEvent : new ::FadcEvent;
   }
   static void *newArray_FadcEvent(Long_t nElements, void *p) {
      return p ? new(p) ::FadcEvent[nElements] : new ::FadcEvent[nElements];
   }
   // Wrapper around operator delete
   static void delete_FadcEvent(void *p) {
      delete ((::FadcEvent*)p);
   }
   static void deleteArray_FadcEvent(void *p) {
      delete [] ((::FadcEvent*)p);
   }
   static void destruct_FadcEvent(void *p) {
      typedef ::FadcEvent current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::FadcEvent

//______________________________________________________________________________
void Bi207Event::Streamer(TBuffer &R__b)
{
   // Stream an object of class Bi207Event.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(Bi207Event::Class(),this);
   } else {
      R__b.WriteClassBuffer(Bi207Event::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_Bi207Event(void *p) {
      return  p ? new(p) ::Bi207Event : new ::Bi207Event;
   }
   static void *newArray_Bi207Event(Long_t nElements, void *p) {
      return p ? new(p) ::Bi207Event[nElements] : new ::Bi207Event[nElements];
   }
   // Wrapper around operator delete
   static void delete_Bi207Event(void *p) {
      delete ((::Bi207Event*)p);
   }
   static void deleteArray_Bi207Event(void *p) {
      delete [] ((::Bi207Event*)p);
   }
   static void destruct_Bi207Event(void *p) {
      typedef ::Bi207Event current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::Bi207Event

//______________________________________________________________________________
void FadcRead::Streamer(TBuffer &R__b)
{
   // Stream an object of class FadcRead.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(FadcRead::Class(),this);
   } else {
      R__b.WriteClassBuffer(FadcRead::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_FadcRead(void *p) {
      return  p ? new(p) ::FadcRead : new ::FadcRead;
   }
   static void *newArray_FadcRead(Long_t nElements, void *p) {
      return p ? new(p) ::FadcRead[nElements] : new ::FadcRead[nElements];
   }
   // Wrapper around operator delete
   static void delete_FadcRead(void *p) {
      delete ((::FadcRead*)p);
   }
   static void deleteArray_FadcRead(void *p) {
      delete [] ((::FadcRead*)p);
   }
   static void destruct_FadcRead(void *p) {
      typedef ::FadcRead current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::FadcRead

namespace ROOT {
   static TClass *vectorlEvectorlEdoublegRsPgR_Dictionary();
   static void vectorlEvectorlEdoublegRsPgR_TClassManip(TClass*);
   static void *new_vectorlEvectorlEdoublegRsPgR(void *p = 0);
   static void *newArray_vectorlEvectorlEdoublegRsPgR(Long_t size, void *p);
   static void delete_vectorlEvectorlEdoublegRsPgR(void *p);
   static void deleteArray_vectorlEvectorlEdoublegRsPgR(void *p);
   static void destruct_vectorlEvectorlEdoublegRsPgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<vector<double> >*)
   {
      vector<vector<double> > *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<vector<double> >),0);
      static ::ROOT::TGenericClassInfo 
         instance("vector<vector<double> >", -2, "vector", 214,
                  typeid(vector<vector<double> >), DefineBehavior(ptr, ptr),
                  &vectorlEvectorlEdoublegRsPgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<vector<double> >) );
      instance.SetNew(&new_vectorlEvectorlEdoublegRsPgR);
      instance.SetNewArray(&newArray_vectorlEvectorlEdoublegRsPgR);
      instance.SetDelete(&delete_vectorlEvectorlEdoublegRsPgR);
      instance.SetDeleteArray(&deleteArray_vectorlEvectorlEdoublegRsPgR);
      instance.SetDestructor(&destruct_vectorlEvectorlEdoublegRsPgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<vector<double> > >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<vector<double> >*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEvectorlEdoublegRsPgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<vector<double> >*)0x0)->GetClass();
      vectorlEvectorlEdoublegRsPgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEvectorlEdoublegRsPgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEvectorlEdoublegRsPgR(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<vector<double> > : new vector<vector<double> >;
   }
   static void *newArray_vectorlEvectorlEdoublegRsPgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<vector<double> >[nElements] : new vector<vector<double> >[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEvectorlEdoublegRsPgR(void *p) {
      delete ((vector<vector<double> >*)p);
   }
   static void deleteArray_vectorlEvectorlEdoublegRsPgR(void *p) {
      delete [] ((vector<vector<double> >*)p);
   }
   static void destruct_vectorlEvectorlEdoublegRsPgR(void *p) {
      typedef vector<vector<double> > current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<vector<double> >

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
"include/Bi207Event.h",
"include/FadcEvent.h",
"include/FadcRead.h",
0
    };
    static const char* includePaths[] = {
"/usr/include/root",
"/home/oviazlo/PhD_study/HW_project/LUCID/signalShapeAnalizer/libs/FadcEvent/",
0
    };
    static const char* fwdDeclCode = 
R"DICTFWDDCLS(
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
class __attribute__((annotate("$clingAutoload$include/FadcEvent.h")))  FadcEvent;
class __attribute__((annotate("$clingAutoload$include/Bi207Event.h")))  Bi207Event;
class __attribute__((annotate("$clingAutoload$include/FadcRead.h")))  FadcRead;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "include/Bi207Event.h"
#include "include/FadcEvent.h"
#include "include/FadcRead.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"Bi207Event", payloadCode, "@",
"FadcEvent", payloadCode, "@",
"FadcRead", payloadCode, "@",
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
