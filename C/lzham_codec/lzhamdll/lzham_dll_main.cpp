// File: lzham_dll_main.cpp
// See Copyright Notice and license at the end of include/lzham.h
#include "lzham_core.h"

BOOL APIENTRY DllMain(HANDLE hModule, DWORD fdwReason, LPVOID lpReserved)
{
   hModule, fdwReason, lpReserved;

   switch( fdwReason ) 
   { 
      case DLL_PROCESS_ATTACH:
      {
         break;
      }
      case DLL_PROCESS_DETACH:
      {
         break;
      }
   }

   return TRUE;
}

