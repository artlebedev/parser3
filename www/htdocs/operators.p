@auto[]
$a[1]

@two[]
#$h[^hash::create[]]
#$a[x]
#$x
#$self.x

@three[]
^process{^^eval(2*2)^$a[1]^$a}

@touchit[]
$i[after]

@reada[]
^$a=$a

@include[filename][f]
$f[^file::load[text;$filename]]^process[$caller.CLASS]{^untaint{$f.text}}

