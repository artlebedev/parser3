CD html
START /low /wait "" "D:\Program Files\HTML Help Workshop\hhc.exe" index.hhp
DEL index.hh?
SET dest=..\chm
DEL %dest%\*.ch? 
COPY *.ch? %dest%\parser3.*
DEL *.ch?
