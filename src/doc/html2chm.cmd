CD html
SET dest=..\chm
START /low /wait "" "D:\Program Files\HTML Help Workshop\hhc.exe" index.hhp
ATTRIB +h index.hh?
DEL %dest%\*.ch? 
COPY *.ch? %dest%\parser3.*
DEL *.ch?
