@USE
/base.p

@CLASS
derived

@BASE
base

@create[aderived_param]
^base:create[base create actual param]
$derived_var[$aderived_param]

@test[]
^$derived_var=$derived_var<br>
^base:test[]

@test2[]
derived_test2
