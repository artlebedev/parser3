CD html
SET dest=..\chm
START /low /wait "" "C:\Program Files\HTML Help Workshop\hhc.exe" index.hhp
ATTRIB +h index.hh?
DEL %dest%\*.ch? 
COPY *.ch? %dest%\parser3src.*
DEL *.ch?
