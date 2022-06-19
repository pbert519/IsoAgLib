#ifndef DECL_derived_iObjectPool_tutorialDisplay_c
#define DECL_derived_iObjectPool_tutorialDisplay_c

// forward declaration
extern IsoAgLib::iVtObject_c* const HUGE_MEM * const all_iVtObjectLists [];

class iObjectPool_tutorialDisplay_c : public IsoAgLib::iVtClientObjectPool_c {
public:
  void initAllObjectsOnce(MULTITON_INST_PARAMETER_DEF);
  iObjectPool_tutorialDisplay_c() : IsoAgLib::iVtClientObjectPool_c (all_iVtObjectLists, 130, 4,  ObjectPoolSettings_s(iVtClientObjectPool_c::ObjectPoolVersion4, 200, 60, 47) ) {}

};

#endif
