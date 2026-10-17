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

# The POST is read after @conf in etc/auto.p. $form is accessed before that to see the GET fields. Test 044 checks that $form is refilled after the POST is read.
$fields-before-conf[$form:fields]

^use[$etc/auto.p]
