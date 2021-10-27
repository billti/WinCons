@ECHO OFF
SET LOCAL
IF NOT DEFINED VCToolsInstallDir EXIT /B -1

SET OUTDIR=%~dp0bin\
IF NOT EXIST "%OUTDIR%" MKDIR "%OUTDIR%"

SET CL=/Zi /Od /MDd /std:c++17 /EHsc /Fe:%OUTDIR% /Fo:%OUTDIR% /Fd:%OUTDIR%
SET CL=/nologo %CL% /permissive- /W3

SET LINK=user32.lib kernel32.lib advapi32.lib

cl.exe winmain.cpp /link /subsystem:windows
cl.exe main.cpp
