@USE
/base.p

@CLASS
derived

@BASE
zbase

@create[aderived_param]
^BASE:create[base create actual param]
$derived_var[$aderived_param]

@test[]
^$derived_var=$derived_var<br>
^BASE:test[]

@test2[]
derived_test2
