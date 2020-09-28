SET CL=/Zi /Od /MDd /std:c++17 /EHsc
SET LINK=user32.lib kernel32.lib

cl.exe winmain.cpp /Fe:app.exe /link /subsystem:windows

REM Build the console version for testing too
cl.exe cons.cpp /Fe:cons.exe
