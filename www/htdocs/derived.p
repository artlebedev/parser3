#@USE
#/base.p

@CLASS
derived

@BASE
table

@auto[]
$derived_var[derived_var_val]

@create[aderived_param]
^table:create{a
aval}

@test[]
$a[overridden] should be error
^^append{appeded} ^append{appeded}<br>
^^count[]=^count[]<br>
^offset(+1)
^$table:a=$table:a<br>
^$a=$a<br>
$x[x] ^$x=$x<br>
#^$table:x=$table:x<br> sould be error
^$CLASS.derived_var=$CLASS.derived_var<br>

@test2[]
derived_test2