@auto[filespec][etc]

$etc[/../../../etc]

# for @conf in etc/auto.p
$CHARSETS[
  $.windows-1251[^file:dirname[$filespec]$etc/parser3.charsets/windows-1251.cfg]
  ]
$HTTPD[
  $.timeout(1)
# $.mode[parallel]
# $.mode[threaded]
]

^use[$etc/auto.p]
