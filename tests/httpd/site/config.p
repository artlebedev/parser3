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

# accessing $form before the POST is read (after @conf in etc/auto.p), and #44 checks it is refilled after the POST.
$fields-before-conf[$form:fields]

^use[$etc/auto.p]
