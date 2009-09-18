@CLASS
test

@auto[]
$a[aaa]


@create[v][x]
$value[$v]

@create0[]

@m[x;y;z][a;b;c]
$value



@CLASS
test1

@BASE
test

@create1[v]

@GET_value[]
$v



@CLASS
test2

@BASE
test1

@create2[v]



@CLASS
test2

@auto[]
$a(1)
$b[2]

@create[]
$c(3)
$d[4]

@m[]
^if(^reflection:dynamical[]){dynamical}{statical}<br />
