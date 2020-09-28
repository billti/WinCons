# WinCons

This is a minimal sample for logging to a console from a Windows executable.

Compile from a Developer Command Prompt with:

```bat
SET CL=/Zi /Od /MDd /std:c++17 /EHsc /Fe:app.exe
SET LINK=/SUBSYSTEM:WINDOWS user32.lib kernel32.lib
cl.exe winmain.cpp
```

Notice how:

- When launched from a command prompt, it will write to the existing console.
- When launched from File Explorer, it will create a new console.

# BUG

When attempting to redirect output, e.g. "app.exe > \temp\app.log", it will fail
with the error outline in the `DiagConsole::Write` function. (Note: If compiled
into a console executable, this issue does not occur).
