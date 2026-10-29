@conf[filespec]
$HTTPD[
	$.mode[$env:TEST_HTTPD_MODE]
]
$LIMITS[
	$.max_loop(0)
]

@unhandled_exception[exception;stack]
$exception.type: $exception.comment^if(def $marks){$marks}^if($request:query eq 'slow-unhandled'){^sleep(8)}


@slow[p]
^sleep(8)

@CLASS
httpd

@main[]
$response:timeout(1)
^self.[$request:query][]

@quick[]
quick

@sleep[]
^sleep(8)
not reached

# the timeout is cancelled by 0 and restarted by the next assignment

@cancel[]
$response:timeout(0)
^sleep(1.5)
done after the timeout

@restart[]
# sum of sleeps is longer than the timeout
^for[i](1;9){
	$response:timeout(2)
	^sleep(0.25)
}
done, the timer was restarted

@loop[]
^for[i](1;2000000000){nothing}
not reached

@hang[]
^file::exec[/hang.sh]
not reached

@slow-unhandled[]
$f[^file::exec[/detach.sh]]
^sleep(8)
not reached

# cheking the interruption is not caught by ^try and does not run catch and finally code
@try[]
^try{
	^sleep(8)
}{
	$marks[ caught]
	$exception.handled(true)
}
not reached

@try-loop[]
^try{
	^for[i](1;2000000000){nothing}
}{
	$marks[ caught]
	$exception.handled(true)
}
not reached

@try-nested[]
^try{
	^try{
		^sleep(8)
	}{
		$marks[ inner]
	}{
		$marks[$marks inner-finally]
	}
}{
	$marks[$marks outer]
	$exception.handled(true)
}{
	$marks[$marks outer-finally]
}
not reached

# libxml calls parser://slow/ and the callback swallows the interruption, the next long operation must be interrupted again
@xml-callback[]
^try{
	$d[^xdoc::load[parser://slow/]]
}{
	$exception.handled(true)
}
^sleep(8)
not reached

# the connection is not answered, the http client waits for its timeout (5 s)
@self[]
$f[^file::load[binary;http://localhost:8103/?quick; $.timeout(5) ]]
not reached
