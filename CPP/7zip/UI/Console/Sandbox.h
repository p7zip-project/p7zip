// Sandbox.h

#ifndef __SANDBOX_H
#define __SANDBOX_H

#include "../../../Common/StdOutStream.h"

struct Sandbox
{
  CStdOutStream *ErrorStream;
  FString *WritableDir;

  Sandbox():
      ErrorStream(NULL),
      WritableDir(NULL)
    {}

  void Enforce(void);
};

#endif
