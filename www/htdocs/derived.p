@USE
/base.p

@CLASS
derived

@BASE
base

@auto[]
$aaa[1]

@create[aderived_param]
^BASE:create[base create actual param]
$derived_var[$aderived_param]

@test[][aaa]
$aaa[xxx]
^$CLASS.aaa=$CLASS.aaa<br>
^$aaa=$aaa<br>
^$derived_var=$derived_var<br>
^BASE:test[]

@test2[]
derived_test2
#^shit[]