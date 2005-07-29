@CLASS
bug_derived

@USE
/_bug.p

@BASE
bug

@create[param]
^BASE:create[$param]
#$_secret[$param]

#@GET_paf[]
#derived value
#=$_secret=

@SET_paf[value]
#^throw[debug;$value]
lala
#$_secret[$value]
#^throw[debug;$value,$_secret]
