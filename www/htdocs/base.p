@CLASS
base

@create[abase]
$base_instance_var[$abase]

@test[]
^$base_instance_var=$base_instance_var<br>
^^test2[]=^test2[]<br>

@test2[]
base_test2



@do_include[]
^$includet2override_var=$includet2override_var<br>
^include[some.p]
@override[] me
in base