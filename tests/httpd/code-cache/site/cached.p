@CLASS
Cached

@OPTIONS
partial

@hello[]
hello from cached.p

@fail[]
^throw[test;;failed in cached.p]


@CLASS
Counter

@auto[]
$count(0)

@inc[]
$count($count+1)
$result($count)


@CLASS
Options

@OPTIONS
partial
locals

@check[]
$result[^if(def $Options:y){LEAKED}{kept}]
