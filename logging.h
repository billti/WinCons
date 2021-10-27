#pragma once

#include "common.h"
#include <string>

class Logger {
 public:
  void CreateConsole() {
    // In a console app, redirected or not, this returns a valid handle.
    // In a non-redirected GUI app, this returns NULL.
    // If redirected GUI (e.g. "winapp.exe > .\log.txt") this returns a valid
    // handle.
    hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdOut == NULL || hStdOut == INVALID_HANDLE_VALUE) {
      // If launched from a console, then Attach work, else must Alloc.
      if (!AttachConsole(ATTACH_PARENT_PROCESS)) {
        if (!AllocConsole()) {
          throw std::exception("Failed to allocate a console");
        }
      }
      hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
      if (hStdOut == INVALID_HANDLE_VALUE || hStdOut == NULL) {
        throw std::exception("Invalid stdout");
      }
    }
    hasConsole = true;
  }

  void Write(const std::string& msg) {
    DWORD written = 0;
    DWORD err = 0;
    if (hasConsole) {
      if (!WriteFile(hStdOut, msg.c_str(), (DWORD)msg.length(), &written, nullptr)) {
        // If output is being redirected for a GUI app, this fails with
        // the following if run from a PowerShell command-line.
        //   0xE8 ERROR_NO_DATA "The pipe is being closed."
        err = GetLastError();
        if (err != ERROR_NO_DATA) {
          throw std::exception("Failed to write to console");
        } else {
          // Don't crash in PowerShell. At least output to the debugger.
          OutputDebugStringA(msg.c_str());
        }
      }
    }
  }

 private:
  bool hasConsole = false;
  HANDLE hStdOut = INVALID_HANDLE_VALUE;
};
