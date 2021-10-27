// For testing: Compile with /SUBSYSTEM:CONSOLE linker option

#include "common.h"
#include "logging.h"

#include <stdio.h>

int main() {
  Logger Log;
  Log.CreateConsole();

  Log.Write("this is my first line\n");
  printf("testing\n");
  Log.Write("exiting");
  return 0;
}
