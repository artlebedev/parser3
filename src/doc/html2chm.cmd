CD html
DEL graph_legend.dot
perl ..\chmhelper.pl
SET dest=..\chm
"C:\Program Files\HTML Help Workshop\hhc.exe" index.hhp
DEL %dest%\*.ch? 
COPY *.ch? %dest%\parser3src.*
DEL *.ch?
REM DEL index.hh?
EXIT