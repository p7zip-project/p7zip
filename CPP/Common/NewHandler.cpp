// NewHandler.cpp
 
#include "StdAfx.h"

#include "../../C/Alloc.h"

#include <exception> 

#include <new>


#ifdef DONT_REDEFINE_NEW

int g_NewHandler = 0;

#else

/* An overload function for the C++ new */
void * operator new(size_t size) throw(std::bad_alloc)
{
  void *p;
  p = MyAlloc(size);
  if(size && (!p))
    throw std::bad_alloc();
  return p;
}

/* An overload function for the C++ new[] */
void * operator new[](size_t size) throw(std::bad_alloc)
{
  void *p;
  p = MyAlloc(size);
  if(size && (!p))
    throw std::bad_alloc();
  return p;
}

/* An overload function for the C++ delete */
void operator delete(void *pnt) throw()
{
    MyFree(pnt);
}

/* An overload function for the C++ delete[] */
void operator delete[](void *pnt) throw()
{
    MyFree(pnt);
}

#endif

