@USE
/base.p

@CLASS
derived

@BASE
base

@auto[]
$derived_var[derived_var_val]

@create[aderived_param]
^base:create[base_create_param_val]

@test[]
inchild:^$base_instance_var=$base_instance_var<br>
^btest[]
$CLASS.derived_var

@test2[]
derived_test2