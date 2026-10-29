@conf[filespec]
$HTTPD[
	$.mode[$env:TEST_HTTPD_MODE]
	$.max-connections(^if(def $env:TEST_MAX_CONNECTIONS){$env:TEST_MAX_CONNECTIONS}{0})
]

@unhandled_exception[exception;stack]
$exception.type: $exception.comment (^file:basename[$exception.file]:$exception.lineno)


@CLASS
httpd

@main[]
^self.[$request:query][]

@quick[]
quick, connections: $status:connections

@slow[]
$connections($status:connections)
^sleep(0.5)
slow, connections: $connections vs $status:connections now
