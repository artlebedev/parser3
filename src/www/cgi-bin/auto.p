@main[]
^untaint[as-is]{^process[$request:query]}
^$request:query=$request:query<BR>
^$request:uri=$request:uri<BR>

$response:content-type[test]

@exception[origin;source;comment;type;code]
ERROR: ${origin}${source}${comment}${type}${code}
