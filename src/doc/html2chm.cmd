CD html
DEL graph_legend.dot
perl ..\chmhelper.pl
SET dest=..\chm
START /low /wait "" "C:\Program Files\HTML Help Workshop\hhc.exe" index.hhp
DEL %dest%\*.ch? 
COPY *.ch? %dest%\parser3src.*
DEL *.ch?
DEL index.hh?
EXIT