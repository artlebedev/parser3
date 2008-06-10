@CLASS
a

@auto[]
$_a[static_a]

@create[]
$a[a]



@CLASS
b

@BASE
a

@auto[]
$_b[static_b]
#^throw[;b]

@create[]
^BASE:create[]
$b[b]




@CLASS
c

@BASE
b

@auto[]
$_c[static_c]
#^throw[;c]

@create[]
^BASE:create[]
$c[c]
