$|=1;
print $info=`PSTAT.EXE |grep inetinfo`;
#0:00:08.231   0:00:07.250  1988    11427    8744  8  482  15 162 inetinfo.exe pid: a2 pri: 8 Hnd:  482 Pf:  11427 Ws:   1988K inetinfo.exe
($pid)=$info=~/(\d+)\s+inetinfo/i;
print $pid?`kill $pid`:"inetinfo not running\n";
$secs=1;
print "sleeping $secs secs... ";
sleep $secs;
print "done\n";