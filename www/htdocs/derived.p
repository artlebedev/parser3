#@USE
#/base.p

@CLASS
derived

@BASE
table

@auto[]
$derived_var[derived_var_val]

@create[aderived_param]
#^table:create{a
#aval}

@test[]
^^append{x} ^append{x}<br>
^^count[]=^count[]<br>
^offset(+1)
^$table:a=$table:a<br>
^$CLASS.derived_var=$CLASS.derived_var<br>

@test2[]
derived_test2