@CLASS
test187


@auto[]
$a[test187_a]


@update[]
$caller.self.a[$a]


@print[]
<hr />
^$a: '$a'<br />
^$caller.self.a: '$caller.self.a'<br />
^$caller.CLASS_NAME: '$caller.CLASS_NAME'<br />
^$caller.self.CLASS_NAME: '$caller.self.CLASS_NAME'<br />
