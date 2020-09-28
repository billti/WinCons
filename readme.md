# WinCons

This is a minimal sample for logging to a console from a Windows executable.

Compile from a Developer Command Prompt by running build.bat.

Notice how:

- When launched from a command prompt, it will write to the existing console.
- When launched from File Explorer, it will create a new console.

# BUG

When attempting to redirect output from a PowerShell command prompt, e.g.
`.\app.exe > .\app.log`, it will fail with the error outlined in the
`DiagConsole::Write` function. (Note: If compiled into a console executable, or
run from a `cmd.exe` prompt, this issue does not occur).
