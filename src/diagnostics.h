#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

#include <Arduino.h>

namespace Diagnostics {
  void initialize();
  void printSystemInfo();
  void printHeartbeat();
  void runDiagnosticMode();
  const char* getResetReason();
}

#endif // DIAGNOSTICS_H
