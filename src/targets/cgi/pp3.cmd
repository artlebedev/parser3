@ECHO off
CD Y:\parser3project\parser3\www\htdocs 

PREP /OM /FT /EXC nafxcwd.lib %1
if errorlevel == 1 goto done 
PROFILE %1 %2 %3 %4 %5 %6 %7 %8 %9 >parser3.output
if errorlevel == 1 goto done 
PREP /M %1
if errorlevel == 1 goto done 
PLIST /ST %1
:done
