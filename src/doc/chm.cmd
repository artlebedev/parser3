CD html
START /wait /b /low "" "D:\Program Files\HTML Help Workshop\hhc.exe" index.hhp
SET dest=..\chm
DEL %dest%\*.ch? 
MOVE *.ch? %dest%