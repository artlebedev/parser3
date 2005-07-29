@CLASS
bug_derived

@USE
/_bug.p

@BASE
bug

#@create[param]
#^BASE:create[param]
#$_secret[$param]

#@get_paf[]
#=$_secret=

#@set_paf[value]
#^throw[debug;$value,$_secret]
#$_secret[$value]
#^throw[debug;$value,$_secret]
