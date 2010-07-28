@echo off
echo The system timezone should be GMT+3!

set PARSER_DIR=..\src\targets\cgi\release
set PARSER_EXECUTABLE=parser3.exe
set DIFF_DIR=..\..\win32\tools
set DIFF_EXECUTABLE=diff.exe -b
set PARSER_CONFIG=results\auto.p
set RUN_PARSER=run_parser.cmd

del /Q "outputs\*.processed"

for %%f in (*.html) do ( call :outputs %%f %%~nf )

for %%f in (*.html) do ( call :compares %%f %%~nf )

pause
goto :end


:outputs
echo %RUN_PARSER% %PARSER_DIR%\%PARSER_EXECUTABLE% %1 %PARSER_CONFIG% ^> outputs\%2.processed
(call %RUN_PARSER% %PARSER_DIR%\%PARSER_EXECUTABLE% %1 %PARSER_CONFIG%) > outputs\%2.processed
goto :end


:compares
echo %DIFF_EXECUTABLE% results\%2.processed outputs\%2.processed
%DIFF_DIR%\%DIFF_EXECUTABLE% results\%2.processed outputs\%2.processed
goto :end



:end
