# the files it loads are compiled once, when the server starts (feature #1302)

@conf[filespec]
$HTTPD[
	$.mode[^if(def $env:TEST_HTTPD_MODE){$env:TEST_HTTPD_MODE}{sequental}]
	$.code-cache(!def $env:TEST_NO_CODE_CACHE)
]

@auto[]
^use[cached.p]
# made by run_tests.sh
^use[changing.p]
# other code under the name of a file the tests load: not to be cached as that file
^process{$processed(true)}[$.file[extend.p]]
# cached when the server starts, but not loaded by the config in requests: the tests meet them later
^if(!def $request:uri){
	^reflection:class_alias[Counter;Late_base]
	^use[late.p]
}

@unhandled_exception[exception;stack]
$exception.type: $exception.comment (^file:basename[$exception.file]:$exception.lineno)


@CLASS
httpd

@main[]
^use[tests.p]
$result[^MAIN:main[]]
