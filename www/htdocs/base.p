@CLASS
base

@auto[]
$base_class_var[1]

@create[abase]
$base_instance_var[$abase]

@test[]
^$base_instance_var=$base_instance_var<br>
^^test2[]=^test2[]<br>

@test2[]
base_test2



@do_include[]
^$base_class_var=$base_class_var<br>
^$includet2derived_var=$includet2derived_var<br>
^$includet2override_var=$includet2override_var<br>
^include[some.p]
@override[] me
in base