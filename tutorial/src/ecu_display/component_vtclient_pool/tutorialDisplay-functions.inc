void iObjectPool_tutorialDisplay_c::initAllObjectsOnce (MULTITON_INST_PARAMETER_DEF)
{
  if (b_initAllObjects) return;   // so the pointer to the ROM structures are only getting set once on initialization!
  int i_objCount = numObjects;
  int i_listIndex = 0;
  IsoAgLib::iVtObject_c::iVtObject_s* const HUGE_MEM * pps_sROMs = all_sROMs;
  do 
  {
    IsoAgLib::iVtObject_c* const HUGE_MEM * pc_objList = all_iVtObjectLists [i_listIndex];
    while (i_objCount)
    {
      ((__IsoAgLib::vtObject_c*)(*pc_objList))->init (*pps_sROMs MULTITON_INST_PARAMETER_USE_WITH_COMMA);
      ++pc_objList;
      ++pps_sROMs;
      --i_objCount;
    }
    i_objCount = numObjectsLang;
    ++i_listIndex;
  } while (all_iVtObjectLists[i_listIndex]);

  #include "tutorialDisplay-functions-origin.inc"

  b_initAllObjects = true;
}
